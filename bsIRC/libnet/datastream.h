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

#ifndef	__F_NET_DATASTREAM_H__
#define	__F_NET_DATASTREAM_H__ 1

#include <storage/seekableio.h>
#include "host.h"
#include "url.h"
#include "socket.h"

namespace net{

class Datastream : public os::SeekableIO
{
public:
	Datastream(Host &cHost);
	Datastream(Url &cUrl);
	virtual ~Datastream();

    virtual ssize_t Read( void* pBuffer, ssize_t nSize );
    virtual ssize_t Write( const void* pBuffer, ssize_t nSize );

    virtual ssize_t ReadPos( off_t nPos, void* pBuffer, ssize_t nSize );
    virtual ssize_t WritePos( off_t nPos, const void* pBuffer, ssize_t nSize );

    virtual off_t Seek( off_t nPos, int nMode );

	virtual status_t SetBlocking( bool bBlocking = true );
	virtual bool IsBlocking( void );

	virtual ssize_t ReadLine( uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout = SOCKET_DEFAULT_TIMEOUT );
	virtual status_t WaitFor( char *pzString, uint8 *pnBuffer, ssize_t nMax, bigtime_t nTimeout = SOCKET_DEFAULT_TIMEOUT );

	virtual status_t Connect( void );
	virtual bool IsConnected( void );
	virtual status_t Disconnect( void );

private:
	class Private;
	Private* m;
};

};

#endif	/* __F_NET_DATASTREAM_H__ */

