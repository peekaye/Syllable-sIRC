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

#include "ipaddress.h"
#include <arpa/inet.h>
#include <netdb.h>

using namespace net;

/** \internal */
class IPAddress::Private
{
public:
	Private(uint32 nAddress)
	{
		m_nAddress = nAddress;
	}
	uint32 m_nAddress;
};

/**
 * Default constructor
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
IPAddress::IPAddress( void )
{
	m = new Private(0);
}

IPAddress::IPAddress( uint32 nAddress )
{
	m = new Private(nAddress);
}

/**
 * Constructor
 * \par Description:
 *		Creates an IP address from a standard dotted-decimal
 *		address representation E.g. "127.0.0.1"
 * \sa	SetAddress()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
IPAddress::IPAddress( const os::String &cAddress )
{
	m = new Private( StringToAddress( cAddress ) );
}

IPAddress::~IPAddress()
{
	delete m;
}

uint32 IPAddress::GetAddress( void )
{
	return m->m_nAddress;
}

void IPAddress::SetAddress( uint32 nAddress )
{
	m->m_nAddress = nAddress;
}

void IPAddress::SetAddress( const os::String &cAddress )
{
	m->m_nAddress = StringToAddress( cAddress );
}

IPAddress& IPAddress::operator=(IPAddress &cAddress)
{
	m->m_nAddress = cAddress.m->m_nAddress;
	return *this;
}

IPAddress& IPAddress::operator=(IPAddress *pcAddress)
{
	if( pcAddress )
		m->m_nAddress = pcAddress->m->m_nAddress;
	return *this;
}

uint32 IPAddress::StringToAddress( const os::String &cString )
{
	struct in_addr sAddr;

	if( inet_aton( cString.c_str(), &sAddr ) )
		return sAddr.s_addr;
	return 0;
}


