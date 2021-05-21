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

#include "datastream.h"
#include "host.h"

using namespace net;

/** \internal */
class Datastream::Private
{
public:
	Private()
	{
	}
	~Private()
	{
		if( m_pcSocket )
			delete m_pcSocket;
	}
	Socket* m_pcSocket;
};

Datastream::Datastream(Host &cHost)
{
	m = new Private();
	if( NULL == m )
		return;
	m->m_pcSocket = new Socket( cHost );
	if( NULL == m->m_pcSocket )
		return;
	// We have to assume this works.  If Socket::Connect() or Socket::Socket()
	// threw an exception this would be easier to catch & cascade.
	m->m_pcSocket->Connect();
}

Datastream::Datastream(Url &cUrl)
{
	Host cHost( cUrl );
	m = new Private();
	if( NULL == m )
		return;
	m->m_pcSocket = new Socket( cHost );
	if( NULL == m->m_pcSocket )
		return;
	// We have to assume this works.  If Socket::Connect() or Socket::Socket()
	// threw an exception this would be easier to catch & cascade.
	m->m_pcSocket->Connect();
}

Datastream::~Datastream()
{
	delete m;
}

ssize_t Datastream::Read( void* pBuffer, ssize_t nSize )
{
	if( m->m_pcSocket->IsConnected() )
	{
		return m->m_pcSocket->Recieve( pBuffer, nSize );
	}
	else
		return 0;
}

ssize_t Datastream::Write( const void* pBuffer, ssize_t nSize )
{
	if( m->m_pcSocket->IsConnected() )
	{
		return m->m_pcSocket->Send( pBuffer, nSize );
	}
	else
		return 0;
}

ssize_t Datastream::ReadPos( off_t nPos, void* pBuffer, ssize_t nSize )
{
	return -EIO;
}

ssize_t Datastream::WritePos( off_t nPos, const void* pBuffer, ssize_t nSize )
{
	return -EIO;
}

off_t Datastream::Seek( off_t nPos, int nMode )
{
	return -EIO;
}

/**
 * \par	Description
 *		Set the blocking mode of the Socket associated with the Datastream.  If the Socket
 *		is blocking then I/O methods such as Read() and Write() will not return until either
 *		data arrives or the operation times out.  If the Socket is non-blocking then these
 *		operations will return imediatly if no data is available.
 * \param bBlocking false Make the Socket non-blocking, true makes the Socket
 * blocking.  Default is true.
 * \return 0 on success.
 * \sa IsBlocking()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
status_t Datastream::SetBlocking( bool bBlocking )
{
	return m->m_pcSocket->SetBlocking( bBlocking );
}

bool Datastream::IsBlocking( void )
{
	return m->m_pcSocket->IsBlocking();
}

ssize_t Datastream::ReadLine( uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout )
{
	return m->m_pcSocket->ReadLine( pnBuffer, nMax, nTimeout );
}

status_t Datastream::WaitFor( char *pzString, uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout )
{
	return m->m_pcSocket->WaitFor( pzString, pnBuffer, nMax, nTimeout );
}

status_t Datastream::Connect( void )
{
	return m->m_pcSocket->Connect();
}

bool Datastream::IsConnected( void )
{
	return m->m_pcSocket->IsConnected();
}

status_t Datastream::Disconnect( void )
{
	return m->m_pcSocket->Disconnect();
}

