
/**
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "commthread.h"
#include "messages.h"

//----------------------------------------------------------------------------------

CommThread::CommThread( const os::Messenger &cTarget ):os::Looper( "CommThread" )
{
	m_cTarget = cTarget;
	m_eState = S_STOP;
	sockfd = -1;
}

bool CommThread::OkToQuit()
{
	return ( true );
}

void CommThread::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			os::String cHost, cPort, cChannel, cNick, cUser, cPassword, cRealname;

			if( pcMessage->FindString( "host", &cHost ) != EOK ) break;
			if( pcMessage->FindString( "port", &cPort ) != EOK ) break;
			if( pcMessage->FindString( "channel", &cChannel ) != EOK ) break;
			if( pcMessage->FindString( "nick", &cNick ) != EOK ) break;
			if( pcMessage->FindString( "user", &cUser ) != EOK ) break;
			if( pcMessage->FindString( "password", &cPassword ) != EOK ) break;
			if( pcMessage->FindString( "realname", &cRealname ) != EOK ) break;

			if( Connect( cHost.c_str(), atoi( cPort.c_str() ) ) == EOK )
			{
				// authenticate to the server
				os::String cTempString = os::String().Format( "NICK %s\r\n", cNick.c_str() );
				Send( cTempString.c_str(), cTempString.Length() );
				cTempString.Format( "USER %s 0 * :%s\r\n", cUser.c_str(), cRealname.c_str() );
				Send( cTempString.c_str(), cTempString.Length() );

				// start Receive() looper
				m_eState = S_START;
				SendReceiveLoop();
			}
			break;
		}
		case MSG_TOLOOPER_STOP:
		{
			if( m_eState == S_START )
			{
				Disconnect();
				m_eState = S_STOP;
			}
			break;
		}
		case MSG_TOLOOPER_RECEIVE:
		{
			SendReceiveLoop();
			break;
		}
		default:
		{
			os::Looper::HandleMessage( pcMessage );
			break;
		}
	}
}

void CommThread::SendReceiveLoop( void )
{
	if( !SendReceiveLoop( m_eState ) )
		sleep( 1 );
	PostMessage( MSG_TOLOOPER_RECEIVE, this );
}

// return true if Receive::select() is ready for reading and
// Receive::recv() has bytes actually read into the buffer
bool CommThread::SendReceiveLoop( state_t eState )
{
	m_eState = eState;
	static char buf[MAXDATASIZE];

	switch ( m_eState )
	{
		case S_START:
		{
			if( Receive( buf, MAXDATASIZE ) > 0 )
			{
				PingPong( buf );
				SendMessage( buf );
				return ( true );
			}
			else
				return ( false );
		}
		case S_STOP:
			return ( false );
	}

	return ( false );
}

// skip Send::send() if not connected
void CommThread::Send( const os::String cSend )
{
	if( m_eState == S_STOP )
		return;

	Send( cSend.c_str(), cSend.Length() );
}

void CommThread::Send( const char* cData, const unsigned int nLen )
{
	fd_set writefds;
	struct timeval tv;

	FD_ZERO( &writefds );
	FD_SET( sockfd, &writefds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	const int rv = select( sockfd + 1, NULL, &writefds, NULL, &tv );

	if( rv == -1 )
	{
		perror( "(Write) select" );
	}
	else if( rv == 0 )
	{
		printf( "(Write) Timeout occured! Blocked after 2.5 seconds. \n" );
	}
	else
	{
		if( FD_ISSET( sockfd, &writefds ) )
		{
			if( send( sockfd, cData, nLen, MSG_DONTWAIT ) == -1 )
				perror( "send" );
		}
	}
}

int CommThread::Receive( char *cBuf, const unsigned int nLen )
{
	fd_set readfds;
	struct timeval tv;
	int nBytes = -1;

	FD_ZERO( &readfds );
	FD_SET( sockfd, &readfds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	const int rv = select( sockfd + 1, &readfds, NULL, NULL, &tv );

	if( rv == -1 )
	{
		perror( "(Read) select" );
		m_eState = S_STOP;
	}
	else if( rv == 0 )
	{
		printf( "(Read) Timeout occured! No data after 2.5 seconds. \n" );
	}
	else
	{
		if( FD_ISSET( sockfd, &readfds ) )
		{
			bzero( cBuf, nLen );
			switch ( (nBytes = recv( sockfd, cBuf, nLen, 0 )) )
			{
				case -1:
					perror( "recv" );
					// fall thru to next case label
				case  0:
					m_eState = S_STOP;
					break;
				default:
					cBuf[nBytes] = '\0';
					return ( nBytes );
			}
		}
	}

	return ( 0 );
}

// skip connect() if already connected
int CommThread::Connect( const char* cHost, const unsigned int nPort )
{
	if( m_eState == S_START )
		return ( 1 );

	struct hostent *he;
	struct sockaddr_in their_addr;	// connector's address information 

	if( ( he = gethostbyname( cHost ) ) == NULL )	// get the host info 
	{
		herror( "gethostbyname" );
		return ( 1 );
	}

	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		perror( "socket" );
		return ( 1 );
	}

	their_addr.sin_family = AF_INET;	// host byte order 
	their_addr.sin_port = htons( nPort );	// short, network byte order 
	their_addr.sin_addr = *( ( struct in_addr * )he->h_addr );
	memset( &( their_addr.sin_zero ), '\0', 8 );	// zero the rest of the struct 

	if( connect( sockfd, ( struct sockaddr * )&their_addr, sizeof( struct sockaddr ) ) == -1 )
	{
		perror( "connect" );
		return ( 1 );
	}

	return ( 0 );
}

// block disconnect() if not connected
void CommThread::Disconnect( void )
{
	if( m_eState == S_START )
	{
		m_eState = S_STOP;
		close( sockfd );
	}
}

// return true if ping command found
bool CommThread::PingPong( const os::String cData )
{
	std::string str( cData );

	if( str.at( 0 ) != ':' )
	{
		if( str.find( "PING", 0 ) != std::string::npos )
		{
			std::cout << "Ping? Pong!" << std::endl;
			Send( os::String().Format( "PONG%s", str.substr( 4 ).c_str() ) );
			return ( true );
		}
	}

	return ( false );
}

// send server data to the MainView textview
void CommThread::SendMessage( const os::String& cName )
{
	try
	{
		os::Message cMsg( MSG_FROMLOOPER_NEW_MESSAGE );
		cMsg.AddString( "name", cName );
		m_cTarget.SendMessage( &cMsg );
	}
	catch( ... ) { }
}

// return true if connected to a server
bool CommThread::IsConnected( void )
{
	return (m_eState == S_STOP) ? false:true;
}

