
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

CommThread::CommThread( const os::Messenger &cTarget ):os::Looper( "comm_worker" )
{
	m_cTarget = cTarget;
	m_eState = S_STOP;
}

bool CommThread::OkToQuit()
{
	return true;
}

void CommThread::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			if( pcMessage->FindString( "host", &m_cHost ) != EOK ) break;
			if( pcMessage->FindString( "port", &m_cPort ) != EOK ) break;
			if( pcMessage->FindString( "channel", &m_cChannel ) != EOK ) break;
			if( pcMessage->FindString( "nick", &m_cNick ) != EOK ) break;
			if( pcMessage->FindString( "user", &m_cUser ) != EOK ) break;
			if( pcMessage->FindString( "password", &m_cPassword ) != EOK ) break;
			if( pcMessage->FindString( "realname", &m_cRealname ) != EOK ) break;

			if( m_eState == S_STOP )
			{
				if( Connect() == EOK )
				{
					m_eState = S_START;
					SendReceiveLoop();
				}
			}
			break;
		}
		case MSG_TOLOOPER_STOP:
		{
			if( m_eState == S_START )
			{
				Send( "QUIT\r\n" );
				sleep( 3 ); // wait for server exit message
				Disconnect();
				m_eState = S_STOP;
			}
			break;
		}
		case MSG_TOLOOPER_RECEIVE:
		{
			Receive();
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
	os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
	PostMessage( msg );
}

void CommThread::Send( const os::String cSendMessage )
{
	if( m_eState == S_STOP ) // must be connected
		return;

	Send( cSendMessage.c_str(), cSendMessage.Length() );
}

void CommThread::Send( const char* msg, const unsigned int len )
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
			if( send( sockfd, msg, len, MSG_DONTWAIT ) == -1 )
				perror( "send" );
		}
	}
}

void CommThread::Receive()
{
	fd_set readfds;
	struct timeval tv;
	int numbytes;
	char buf[MAXDATASIZE];

	FD_ZERO( &readfds );
	FD_SET( sockfd, &readfds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	const int rv = select( sockfd + 1, &readfds, NULL, NULL, &tv );

	if( rv == -1 )
	{
		perror( "(Read) select" );
	}
	else if( rv == 0 )
	{
		printf( "(Read) Timeout occured! No data after 2.5 seconds. \n" );
		SendReceiveLoop();
	}
	else
	{
		if( FD_ISSET( sockfd, &readfds ) )
		{
			bzero( buf, MAXDATASIZE );
			if( ( numbytes = recv( sockfd, buf, MAXDATASIZE, 0 ) ) < 1 )
			{
				if( numbytes == -1 )
					perror( "recv" );
				return;
			}
			buf[numbytes] = '\0';
			PingPong( buf );
			SendMessage( buf );
		}
	}
}

int CommThread::Connect( void )
{
	struct hostent *he;
	struct sockaddr_in their_addr;	// connector's address information 

	if( ( he = gethostbyname( m_cHost ) ) == NULL )	// get the host info 
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
	their_addr.sin_port = htons( atoi( m_cPort ) );	// short, network byte order 
	their_addr.sin_addr = *( ( struct in_addr * )he->h_addr );
	memset( &( their_addr.sin_zero ), '\0', 8 );	// zero the rest of the struct 

	if( connect( sockfd, ( struct sockaddr * )&their_addr, sizeof( struct sockaddr ) ) == -1 )
	{
		perror( "connect" );
		return ( 1 );
	}

	//authenticate to the server
	os::String cTempString = "NICK ";
	cTempString += m_cNick;
	cTempString += "\r\n";
	Send( cTempString.c_str(), cTempString.Length() );

	cTempString = "USER ";
	cTempString += m_cUser;
	cTempString += " 0 * :";
	cTempString += m_cRealname;
	cTempString += "\r\n";
	Send( cTempString.c_str(), cTempString.Length() );

	return ( 0 );
}

void CommThread::Disconnect( void )
{
	if( m_eState == S_START ) // test for connected
	{
		m_eState = S_STOP;
		close( sockfd );
	}
}

void CommThread::PingPong( const os::String cBufString )
{
	if( cBufString[0] != ':' )
	{
		const std::string::size_type pos = cBufString.find( "PING", 0 );
		if( pos != std::string::npos )
		{
			std::cout << "Ping? Pong!" << std::endl;
			os::String cTempString = "PONG :";
			cTempString += m_cNick;
			cTempString += "\r\n";
			Send( cTempString );
		}
	}
}

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

