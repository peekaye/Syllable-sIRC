
#ifndef COMMTHREAD_H
#define COMMTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <util/message.h>
#include <util/thread.h>
#include <util/string.h>
#include <util/messenger.h>

#include <libnet/host.h>
#include <libnet/socket.h>

class CommThread:public os::Thread
{
public:
	enum state_t { S_START, S_STOP };

public:
    CommThread( const os::Messenger &cTarget );
    virtual int32 Run();

	void Reinitialize( void );
	int Connect( const os::String stdServerName, const os::String stdServerPort );
	void Disconnect( void );
	void Send( const os::String stdOutgoingMessage );
	void SetState( state_t eState );
	const uint32 GetState();

private:
	void SendMessage( const os::String& cName ) const;

	os::Messenger m_cTarget;
	state_t m_eState;
	net::Socket *cSocket;
};

#endif

