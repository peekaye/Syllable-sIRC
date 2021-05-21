
/*  libnet.so - the highlevel network API library for Syllable
 *  Copyright (C) 2004 Kristian Van Der Vliet
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of version 2 of the GNU Library
 *  General Public License as published by the Free Software
 *  Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 *  MA 02111-1307, USA
 */

#include "socket.h"
#include "host.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

// C callback for sigaction() alarm.  I don't know if we can easily use C++
// methods instead, I'll look into it.
extern "C"{
static void __socket_sig_handler( int nSignal, siginfo_t *psInfo, void *pData );
};

using namespace net;

/** \internal */
class Socket::Private
{
public:
	Private( Host &cHost )
	{
		m_pcHost = new Host(cHost.GetHost(), cHost.GetPort());
		m_nType = SOCK_STREAM;
		m_bConnected = false;
		m_bIsBlocking = true;
	}
	void SetHost( Host &cHost )
	{
		if( m_pcHost )
			delete m_pcHost;
		m_pcHost = new Host(cHost.GetHost(), cHost.GetPort());
	}
	int m_nSocket;
	int m_nType;
	bool m_bConnected;
	Host* m_pcHost;
	struct sigaction m_sOldSigaction;
	bool m_bIsBlocking;
};

/**
 * Default constructor
 * \par	Description:
 *		Create a Socket for a specified Host.
 * \param cHost The hostname E.g. www.example.com:80  The Socket class retrieves both the
 * domain name and the port number from the Host name.
 * \param bDatagram false creates a TCP socket, true creates a UDP socket.  Default is false
 * \sa Connect()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Socket::Socket( Host &cHost, bool bDatagram )
{
	m = new Private(cHost);
	// if this is a UDP socket then override the default type
	if( bDatagram )
		m->m_nType = SOCK_DGRAM;

	// Create the socket.  We have to assume this succeeds; maybe we should throw
	// an exception if it fails
	m->m_nSocket = socket( PF_INET, m->m_nType, 0 );

	// Intialise signal handling for timeouts.  Again we assume this succeeds and
	// the same comment re. excpetions applies.
	SetupSigHandler();

	// Clear flags
	m_bTimeout = false;
}

/**
 * Destructor
 * \par	Description:
 *		Destroy a Socket().  Will disconnect the Socket for you if it is connected.
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Socket::~Socket( void )
{
	// Disconnect if we need to
	if( m->m_bConnected )
		Disconnect();

	// Remove the timeout signal handler
	TeardownSigHandler();

	delete m;
}

/**
 * \par	Description:
 *		Set the Host to connect the Socket to.
 * \param cHost The hostname E.g. www.example.com:80  The Socket class retrieves both the
 * domain name and the port number from the Host name.  The Host and port can only be changed
 * before the Socket is connected.
 * \sa Connect()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
void Socket::SetHost( Host &cHost )
{
	m->SetHost( cHost );
}

Host Socket::GetHost( void )
{
	Host cHost( m->m_pcHost->GetHost(), m->m_pcHost->GetPort() );
	return cHost;
}

/**
 * \par	Description:
 *		Connect the Socket to the Host.  The Host (Domain name and port) must have already
 * 		been specified.
 * \sa Socket(), SetHost(), Disconnect(), SetBlocking()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
status_t Socket::Connect( void )
{
	struct sockaddr_in sAddr;
	int nError = 0;

	if( SOCK_DGRAM == m->m_nType )
		return EINVAL;	// UDP sockets can not be Connected

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons( m->m_pcHost->GetPort() );
	sAddr.sin_addr.s_addr = m->m_pcHost->GetIPAddress()->GetAddress();
	memset( &sAddr.sin_zero, 0, 8 );

	// Start timer
	StartTimer( SOCKET_DEFAULT_TIMEOUT );

	// Attempt to connect
	nError = connect( m->m_nSocket, (struct sockaddr*)&sAddr, sizeof( struct sockaddr ) );
	if( nError || m_bTimeout )
		return nError;

	// Stop timer
	StopTimer();

	// We're connected
	m->m_bConnected = true;
	return 0;
}

bool Socket::IsConnected( void )
{
	return m->m_bConnected;
}

/**
 * \par	Description:
 *		Disconnects the Socket, if connected.
 * \sa Connect()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
status_t Socket::Disconnect( void )
{
	if( !m->m_bConnected )
		return EINVAL;	// There is nothing to disconnect

	// Close the socket
	close( m->m_nSocket );

	m->m_bConnected = false;
	return 0;
}

/**
 * \par	Description
 *		Send data to the server.  Send() is a blocking operation; it will not return
 *		until either all of the data has been sent or the operation times out.
 * \param pBuffer A buffer containing the data to be sent.
 * \param nSize The number of bytes to be sent.
 * \return -EINVAL if pBuffer was NULL or nSize was invalid, -ENOTCONN if the Socket
 * is a TCP socket but it is not yet connected, number of bytes sent if successful.
 * \sa Connect(), SetBlocking()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
ssize_t Socket::Send( const void *pBuffer, ssize_t nSize )
{
	ssize_t nBytes;

	if( NULL == pBuffer || nSize == 0 )
		return -EINVAL;

	if( m->m_nType == SOCK_STREAM & !IsConnected() )
		return -ENOTCONN;

	// FIXME: Handle UDP sockets
	StartTimer( SOCKET_DEFAULT_TIMEOUT );
	nBytes = send( m->m_nSocket, pBuffer, nSize, 0 );
	StopTimer();

	return nBytes;
}

/**
 * \par	Description
 *		Recieves data from the server.  Recieve() is a blocking operation; it will not return
 *		until either all of the data has been recieved or the operation times out.
 * \param pBuffer A buffer to hold the data being recieved.
 * \param nSize The number of bytes to be received.
 * \return -EINVAL if pBuffer was NULL or nSize was invalid, -ENOTCONN if the Socket
 * is a TCP socket but it is not yet connected, number of bytes read if successful.
 * \sa Connect(), SetBlocking()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
ssize_t Socket::Recieve( void *pBuffer, ssize_t nSize )
{
	ssize_t nBytes;

	if( NULL == pBuffer || nSize == 0 )
		return -EINVAL;

	if( m->m_nType == SOCK_STREAM & !IsConnected() )
		return -ENOTCONN;

	// FIXME: Handle UDP sockets
	StartTimer( SOCKET_DEFAULT_TIMEOUT );
	nBytes = recv( m->m_nSocket, pBuffer, nSize, 0 );
	StopTimer();

	return nBytes;
}

/**
 * \par	Description
 *		Recieves data from the server until the sequence \r\n (CRLF) is encountered.
 *		This allows you to easily recieve a single line of data from a server.
 *		ReadLine() is a blocking operation; it will not return until either all of
 *		the data has been recieved or the operation times out.
 * \param pnBuffer A buffer to hold the data being recieved.
 * \param nMax The maximum amount of data to recieve from the server.  nMax must
 * not be larger than the size of pnBuffer.
 * \param nTimeout Time to wait before the operation is abandoned.  Default is 60 seconds.
 * \return -EINVAL if pBuffer was NULL or nMax was invalid, -ENOTCONN if the Socket
 * is a TCP socket but it is not yet connected, -ETIME if nTimeout was reached before
 * \r\n was sent by the server, number of bytes read if successful.
 * \sa Recieve(), WaitFor()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
ssize_t Socket::ReadLine( uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout )
{
	ssize_t nBytes = 0, nTotal = 0;
	bool bCR = false, bEol = false;

	if( NULL == pnBuffer || nMax < 0 )
		return -EINVAL;

	if( m->m_nType == SOCK_STREAM & !IsConnected() )
		return -ENOTCONN;

	// Read data one byte at a time until we reach the sequence \r\n
	// FIXME: Handle UDP sockets
	StartTimer( nTimeout );
	while(!bEol)
	{
		nBytes = recv( m->m_nSocket, (void*)pnBuffer, 1, 0 );
		if( nBytes < 1 )
			break;

		switch( *pnBuffer )
		{
			case '\r':
				bCR = true;			// A \r character has been found.  Note its
				break;				// existence in the stream.

			case '\n':
			{
				if( bCR )			// Was this \n procedded by an \r?
					bEol = true;	// Yes.  The sequence \r\n has been found
				break;
			}

			default:				// No \r or \n character was found.  Clear the \r flag
				bCR = false;
		}
		nTotal++;
		if( nTotal >= nMax )		// Ensure we don't overflow the provided buffer
			break;
		pnBuffer++;
	}

	if( m_bTimeout )
	{
		StopTimer();
		return -ETIME;
	}

	StopTimer();
	return nTotal;
}

/**
 * \par	Description
 *		Recieves data from the server until the sequence specified by pzString is encountered.
 *		This allows you to easily wait for a specific response from a server.
 *		WaitFor() is a blocking operation; it will not return until either all of
 *		the data has been recieved or the operation times out.
 * \param pzString The sequence to wait for the server to send.
 * \param pnBuffer A buffer to hold the data being recieved.
 * \param nMax The maximum amount of data to recieve from the server.  nMax must
 * not be larger than the size of pnBuffer.
 * \param nTimeout Time to wait before the operation is abandoned.  Default is 60 seconds.
 * \return -EINVAL if pBuffer was NULL or nMax was invalid, -ENOTCONN if the Socket
 * is a TCP socket but it is not yet connected, -ETIME if nTimeout was reached before
 * \r\n was sent by the server, number of bytes read if successful.
 * \sa Recieve(), ReadLine()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
status_t Socket::WaitFor( char *pzString, uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout )
{
	int nSize;
	ssize_t nBytes = 0, nTotal = 0;
	bool bMatched = false;

	if( NULL == pzString || NULL == pnBuffer || nMax == 0 )
		return -EINVAL;

	if( m->m_nType == SOCK_STREAM & !IsConnected() )
		return -ENOTCONN;

	nSize = strlen( pzString );
	if( nSize >= nMax )
		return -EINVAL;

	// Prime the buffer with enough data to match it with the string we're searching for
	// FIXME: Handle UDP sockets
	StartTimer( nTimeout );
	nBytes = recv( m->m_nSocket, (void*)pnBuffer, nSize, 0 );
	nTotal = nBytes;
	if( nBytes < nSize || m_bTimeout )
	{
		StopTimer();
		return nTotal;
	}
	StopTimer();
	pnBuffer += nBytes;

	StartTimer( nTimeout );
	// Compare the string we're searching for to the buffer.  If it doesn't match, read
	// one byte and append it to the buffer.
	while( !bMatched )
	{
		if( strncmp( (const char*)(pnBuffer - nSize), pzString, nSize ) == 0 )
		{
			bMatched = true;
			break;
		}

		nBytes = recv( m->m_nSocket, (void*)pnBuffer, 1, 0 );
		if( nBytes < 1 )
			break;

		nTotal++;
		if( nTotal >= nMax )
			break;
		pnBuffer++;
	}

	if( m_bTimeout )
	{
		StopTimer();
		return -ETIME;
	}

	StopTimer();
	if( !bMatched )
		return -EIO;
	return nTotal;
}

/**
 * \par	Description
 *		Set the blocking mode of the Socket.  If the Socket is blocking then I/O
 *		methods such as Send() and Recieve() will not return until either data arrives
 *		or the operation times out.  If the Socket is non-blocking then these operations
 *		will return imediatly if no data is available.
 * \param bBlocking false Make the Socket non-blocking, true makes the Socket
 * blocking.  Default is true.
 * \return 0 on success.
 * \sa Connect(), IsBlocking()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
status_t Socket::SetBlocking( bool bBlocking )
{
	int nFlags, nRet = 0;
	if( bBlocking )
	{
		if( !IsBlocking() )
		{
			nFlags = fcntl( m->m_nSocket, F_GETFL );
			nRet = fcntl( m->m_nSocket, F_SETFL, nFlags |= O_NONBLOCK);
		}
	}
	else
	{
		if( IsBlocking() )
		{
			nFlags = fcntl( m->m_nSocket, F_GETFL );
			nRet = fcntl( m->m_nSocket, F_SETFL, nFlags &= ~O_NONBLOCK);
		}

	}
	return nRet;
}

bool Socket::IsBlocking( void )
{
	return m->m_bIsBlocking;
}

status_t Socket::SetupSigHandler( void )
{
	struct sigaction sSigAction;

	// Setup the signal handler to catch SIGALRM timeout signals
	sSigAction.sa_sigaction = __socket_sig_handler;
	sSigAction.sa_flags = SA_NOMASK | SA_SIGINFO;
	if( sigaction( SIGALRM, &sSigAction, &m->m_sOldSigaction ) < 0 )
		return -1;
	return 0;
}

void Socket::TeardownSigHandler( void )
{
	// Reset the current signal handlers to their previous configuration
	sigaction( SIGALRM, &m->m_sOldSigaction, NULL );
}

void Socket::StartTimer( bigtime_t nTimeout )
{
	// Clear flag
	m_bTimeout = false;

	// Setup a timer to generate SIGALRM after the timeout period elapses
	struct itimerval sAlarm;
	sAlarm.it_interval.tv_sec = nTimeout;
	setitimer( ITIMER_REAL, &sAlarm, (itimerval*)this );
}

void Socket::StopTimer( void )
{
	// Clear a currently running timer
	struct itimerval sAlarm;
	sAlarm.it_interval.tv_sec = 0;
	setitimer( ITIMER_REAL, &sAlarm, NULL );

	// Clear flag
	if( m_bTimeout )
		m_bTimeout = false;
}

extern "C" {
static void __socket_sig_handler( int nSignal, siginfo_t *psInfo, void *pData )
{
	/* Handle signals; only deal with SIGALRM */
	switch( psInfo->si_signo )
	{
		case SIGALRM:
		{
			Socket *psSocket = (Socket*)pData;
			if( psSocket )
				psSocket->m_bTimeout = true;
		}
	}
}
}; // extern "C"
