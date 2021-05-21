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

#ifndef	__F_NET_SOCKET_H__
#define	__F_NET_SOCKET_H__ 1

#include "host.h"
#include <atheos/types.h>
#include <posix/time.h>

namespace net{

#define SOCKET_DEFAULT_TIMEOUT	60	// One minute

class Socket
{
public:
	Socket( Host &cHost, bool bDatagram = false );
	virtual ~Socket( void );

	virtual void SetHost( Host &cHost );
	virtual Host GetHost( void );

	virtual status_t Connect( void );
	virtual bool IsConnected( void );
	virtual status_t Disconnect( void );

	virtual ssize_t Send( const void *pBuffer, ssize_t nSize );
	virtual ssize_t Recieve( void *pBuffer, ssize_t nSize );

	virtual ssize_t ReadLine( uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout = SOCKET_DEFAULT_TIMEOUT );
	virtual status_t WaitFor( char *pzString, uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout = SOCKET_DEFAULT_TIMEOUT );

	virtual status_t SetBlocking( bool bBlocking = true );
	virtual bool IsBlocking( void );
private:
	status_t SetupSigHandler( void );
	void TeardownSigHandler( void );
	void StartTimer( bigtime_t nTimeout );
	void StopTimer( void );
private:
	virtual void	__SOC_reserved1__() {}
	virtual void	__SOC_reserved2__() {}
	virtual void	__SOC_reserved3__() {}
	virtual void	__SOC_reserved4__() {}
	virtual void	__SOC_reserved5__() {}
private:
	class Private;
	Private* m;
public:
	bool m_bTimeout;
};

};

#endif	/* __F_NET_SOCKET_H__ */


