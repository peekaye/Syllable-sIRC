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

#ifndef	__F_NET_IPADDRESS_H__
#define	__F_NET_IPADDRESS_H__ 1

#include <atheos/types.h>
#include <util/string.h>

namespace net{

class IPAddress
{
public:
	IPAddress( void );
	IPAddress( uint32 nAddress );
	IPAddress( const os::String &cAddress );
	virtual ~IPAddress();

	virtual uint32 GetAddress( void );
	virtual void SetAddress( uint32 nAddress );
	virtual void SetAddress( const os::String &cAddress );

	virtual IPAddress& operator=(IPAddress &cAddress);
	virtual IPAddress& operator=(IPAddress *pcAddress);
private:
	uint32 StringToAddress( const os::String  &cString );
private:
	virtual void	__IPA_reserved1__() {}
	virtual void	__IPA_reserved2__() {}
	virtual void	__IPA_reserved3__() {}
	virtual void	__IPA_reserved4__() {}
	virtual void	__IPA_reserved5__() {}
private:
	class Private;
	Private *m;
};

};

#endif /* __F_NET_IPADDRESS_H__ */
