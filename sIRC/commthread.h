
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

#ifndef SIRC_COMMTHREAD_H
#define SIRC_COMMTHREAD_H

#include <stdio.h> // printf(), stderr(), perror()
#include <stdlib.h> // atoi()
#include <unistd.h> // close(), sleep()
#include <posix/errno.h> // EOK
#include <string.h> // memset()
#include <strings.h> // bzero()

#include <string>
#include <iostream>

#include <util/message.h>
#include <util/looper.h>
#include <util/string.h>
#include <util/messenger.h>

#include <sys/socket.h> // socket(), connect(), recv(), send(), AF_INET
#include <sys/types.h> // timeval()
#include <sys/select.h> // select(), FD_SET, FD_ZERO, FD_ISSET
#include <netinet/in.h> // sockaddr(), sockaddr_in(), in_addr()
#include <arpa/inet.h> // htons(), 
#include <netdb.h> // gethostbyname(), hostent()

#define MAXDATASIZE 50000 // max number of bytes we can get at once

//----------------------------------------------------------------------------------

class CommThread:public os::Looper
{
public:
	enum state_t { S_START, S_STOP };

public:
	CommThread( const os::Messenger &cTarget );
	virtual void HandleMessage( os::Message *pcMessage );
//	virtual bool Idle();
	virtual bool OkToQuit();

	void SendReceiveLoop( void );
	void Send( const os::String cSend );
	bool IsConnected( void );

private:
	bool SendReceiveLoop( state_t eState );
	void Send( const char* cData, const unsigned int nLen );
	int Receive( char *cBuf, const unsigned int nLen );
	int Connect( const char* cHost, const unsigned int nPort );
	void Disconnect( void );
	bool PingPong( const os::String cData );
	void SendMessage( const os::String& cName );

	os::Messenger m_cTarget;
	state_t m_eState;
	int sockfd;
};

#endif

