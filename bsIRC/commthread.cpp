
#include "commthread.h"
#include "messages.h"

CommThread::CommThread( const os::Messenger &cTarget ):os::Thread( "CommThread" )
{
	m_cTarget = cTarget;
}

//www.liqwyd.com/downloads/mercury-0.1.1.src.tgz
void CommThread::Reinitialize( void )
{
	Stop();
	Initialize( "comm_worker" );
}

//www.liqwyd.com/downloads/libnet-alpha-1.tgz
int32 CommThread::Run()
{
	static char pzBuffer[1025];
	ssize_t nBytes = 0;

	memset( pzBuffer, '\0', 1025 );

	if( cSocket->IsConnected() )
	{
		while( ( nBytes = cSocket->ReadLine( (uint8 *)pzBuffer, 1024 ) ) > 0 )
		{
			pzBuffer[nBytes] = '\0';
			SendMessage( os::String( pzBuffer ) );
			memset( pzBuffer, '\0', 1025 ); //clear the buffer
		}
	}

	return( 0 );
}

//www.liqwyd.com/downloads/libnet-alpha-1.tgz
int CommThread::Connect( const os::String stdServerName, const os::String stdServerPort )
{
	net::Host cHost( stdServerName.c_str(), atoi( stdServerPort.c_str() ) );
	cSocket = new net::Socket( cHost );

	cSocket->SetBlocking();
	cSocket->Connect();
	return cSocket->IsConnected();
}

void CommThread::Disconnect( void )
{
	// wait 2.5sec for server exit message
	Delay( 2500000 );

	if( cSocket->IsConnected() )
		cSocket->Disconnect();
}

//www.liqwyd.com/downloads/libnet-alpha-1.tgz
void CommThread::Send( const os::String stdOutgoingMessage )
{
	os::String cTempString = stdOutgoingMessage;

	if( cSocket->IsConnected() )
	{
		cTempString = stdOutgoingMessage + os::String( "\r\n" );
		if( cSocket->Send( (void *)cTempString.c_str(), cTempString.Length() ) == -1 )
		{
			perror( "Socket::Send" );
		}
	}
}

bool CommThread::PingPong( os::String cName )
{
	if ( cName.substr( 0, 1 ) != ":" )
	{
		if( cName.substr( 0, 4 ).Compare( "PING" ) == EOK )
		{
			Send( os::String().Format( "PONG%s", cName.substr( 4 ).c_str() ) );
			return ( true );
		}
	}

	return ( false );
}

//www.jarvoll.se/syllable/files/looper.zip
void CommThread::SendMessage( const os::String& cName )
{
	if( PingPong( cName ) || cName.empty() )
		return;

	try
	{
		os::Message cMsg( MSG_FROMLOOPER_NEW_MESSAGE );
		cMsg.AddString( "name", cName );
		m_cTarget.SendMessage( &cMsg );
	}
	catch( ... ) { }
}

void CommThread::SetState( state_t eState )
{
	m_eState = eState;
}

const uint32 CommThread::GetState()
{
	return m_eState;
}

