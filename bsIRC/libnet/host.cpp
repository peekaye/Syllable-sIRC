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

#include "host.h"
#include "ipaddress.h"
#include "url.h"
#include <arpa/inet.h>
#include <netdb.h>

using namespace net;

/** \internal */
class Host::Private
{
public:
	Private()
	{
		m_pcAddress = new IPAddress();
		m_pcUrl = new Url();
		m_cHost = "";
		m_nPort = 0;
	}
	~Private()
	{
		delete m_pcAddress;
		delete m_pcUrl;
	}
	void SetAddress( IPAddress *pcAddress )
	{
		if( m_pcAddress )
			delete m_pcAddress;
		m_pcAddress = pcAddress;
	}
	void SetAddress( IPAddress &cAddress )
	{
		if( m_pcAddress )
			delete m_pcAddress;
		m_pcAddress = new IPAddress(cAddress);
	}
	void SetUrl( Url &pcUrl )
	{
		if( m_pcUrl )
			delete m_pcUrl;
		m_pcUrl = new Url(pcUrl.GetUrl());
	}

	IPAddress* m_pcAddress;
	Url* m_pcUrl;
	os::String m_cHost;
	uint32 m_nPort;
};

/**
 * Default constructor
 * \par		Description:
 *		Create a Host from a simple hostname and port.  If the hostname is valid
 *		then the IPAddress returned by GetIPAddress() will be valid.
 * \param cHost The hostname E.g. www.example.com
 * \param nPort Port number to connect to
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Host::Host( const os::String &cHost, uint32 nPort )
{
	m = new Private();
	m->m_cHost = cHost;
	m->m_nPort = nPort;
	m->SetAddress( Lookup( cHost ) );
}

/**
 * Constructor
 * \par		Description:
 * 		Creates a Host from an IPAddress.  The hostname retrived via. GetHost()
 *		will be invalid unless it is explicitly set via. SetHost()
 * \param cAddress The IPAddress of the host
 * \param nPort Port number to connect to
 * \sa GetHost(), SetHost()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Host::Host( IPAddress &cAddress, uint32 nPort )
{
	m = new Private();
	m->m_cHost = "";
	m->m_nPort = nPort;
	m->SetAddress( cAddress );
}

/**
 * Constructor
 * \par		Description:
 *		Creates a Host from a URL.  The URL can be used to specify the port number,
 *		if required.  If the hostname is valid then the IPAddress returned by
 *		GetIPAddress() will be valid.
 * \param cUrl The URL of the host
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Host::Host( Url &cUrl )
{
	m = new Private();
	m->SetUrl( cUrl );
	m->m_cHost = m->m_pcUrl->GetHostName();
	m->SetAddress( Lookup( m->m_cHost ) );
	m->m_nPort = m->m_pcUrl->GetPort();
}

Host::~Host(void)
{
	delete m;
}

void Host::SetHost( const os::String &cHost )
{
	m->m_cHost = cHost;
	m->SetAddress( Lookup( cHost ) );
}

os::String Host::GetHost( void )
{
	return m->m_cHost;
}

void Host::SetPort( uint32 nPort )
{
	m->m_nPort = nPort;
}

uint32 Host::GetPort( void )
{
	return m->m_nPort;
}

/**
 * Get the IP address of the current host
 * \par Description:
 *		Returns an IPAddress for the current host.
 * \sa GetHost()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
IPAddress* Host::GetIPAddress( void )
{
	return m->m_pcAddress;
}

IPAddress* Host::Lookup( const os::String cHost )
{
	struct hostent *sHostEntry;
	IPAddress *pcAddress = new IPAddress(0);

	sHostEntry = gethostbyname( cHost.c_str() );
	if( NULL == sHostEntry )
		return pcAddress;

	pcAddress->SetAddress(ntohl(inet_network(inet_ntoa((*(struct in_addr*)sHostEntry->h_addr)))));
	return pcAddress;
}

