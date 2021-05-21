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

#ifndef	__F_NET_HOST_H__
#define	__F_NET_HOST_H__ 1

#include "ipaddress.h"
#include "url.h"
#include <util/string.h>
#include <atheos/types.h>

namespace net{

class Host
{
public:
	Host( const os::String &cHost, uint32 nPort = 0 );
	Host( IPAddress &cAddress, uint32 nPort = 0 );
	Host( Url &cUrl );
	virtual ~Host(void);

	void SetHost( const os::String &cHost );
	os::String GetHost( void );
	void SetPort( uint32 nPort );
	uint32 GetPort( void );
	IPAddress* GetIPAddress( void );
private:
	IPAddress* Lookup( const os::String cHost );
private:
	virtual void	__HOS_reserved1__() {}
	virtual void	__HOS_reserved2__() {}
	virtual void	__HOS_reserved3__() {}
	virtual void	__HOS_reserved4__() {}
	virtual void	__HOS_reserved5__() {}
private:
	class Private;
	Private* m;
};

};

#endif	/* __F_NET_HOST_H__ */


