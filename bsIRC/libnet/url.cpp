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

#include <util/regexp.h>

#include "url.h"

using namespace net;

/** \internal */
class Url::Private
{
public:
	Private()
	{
		m_cUrl = "";
		m_cProtocol = "";
		m_cHostName = "";
		m_cPath = "";
		m_cUsername = "";
		m_cPassword = "";
		m_nPort = 0;
	};
	os::String m_cUrl;
	os::String m_cProtocol;
	os::String m_cHostName;
	os::String m_cPath;
	os::String m_cUsername;
	os::String m_cPassword;
	uint32 m_nPort;
};

/**
 * Default constructor
 * \sa	Url( const os::String &cUrl )
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Url::Url( void )
{
	m = new Private();
}

/**
 * Constructor
 * \par Description:
 *		Creates a URL from a "Standard" URL in the form
 *      proto://[user:pass]www.example.com[:port][/path/to/file.txt]
 * \sa	SetUrl()
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
Url::Url( const os::String &cUrl )
{
	m = new Private();
	m->m_cUrl = cUrl;
	ParseUrl( cUrl );
}

Url::~Url( void )
{
	delete m;
}

os::String Url::GetUrl( void )
{
	return m->m_cUrl;
}

void Url::SetUrl( os::String &cUrl )
{
	m->m_cUrl = cUrl;
	ParseUrl( cUrl );
}

/**
 * \par Description:
 *		Returns the protocol part of the URL E.g. http
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::String Url::GetProtocol( void )
{
	return m->m_cProtocol;
}

void Url::SetProtocol( os::String &cProtocol )
{
	m->m_cProtocol = cProtocol;
}

/**
 * \par Description:
 *		Returns the hostname part of the URL E.g. www.example.com
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::String Url::GetHostName( void )
{
	return m->m_cHostName;
}

void Url::SetHostName( os::String &cHostName )
{
	m->m_cHostName = cHostName;
}

/**
 * \par Description:
 *		Returns the port part of the URL if one was specified E.g. 8080
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
uint32 Url::GetPort( void )
{
	return m->m_nPort;
}

void Url::SetPort( uint32 nPort )
{
	m->m_nPort = nPort;
}

/**
 * \par Description:
 *		Returns the path part of the URL if one was specified E.g. /file.txt
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::String Url::GetPath( void )
{
	return m->m_cPath;
}

void Url::SetPath( os::String &cPath )
{
	m->m_cPath = cPath;
}

/**
 * \par Description:
 *		Returns the username part of the URL if one was specified
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::String Url::GetUsername( void )
{
	return m->m_cUsername;
}

void Url::SetUsername( os::String &cUsername )
{
	m->m_cUsername = cUsername;
}

/**
 * \par Description:
 *		Returns the password part of the URL if one was specified
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::String Url::GetPassword( void )
{
	return m->m_cPassword;
}

void Url::SetPassword( os::String &cPassword )
{
	m->m_cPassword = cPassword;
}

void Url::ParseUrl( const os::String &cUrl )
{
	// Private method to parse a "standard" form URL into its component parts.  The URL must
	// be in the form
	//
	// proto://[user:pass]www.example.com[:port][/path/to/file.txt]
	//
	// There are probably better ways to do this but it seems to work for both simple and
	// complex URLs.  The order of the RegExp's are important as various characters can appear
	// in difference places depending on context E.g. : is used in three places.

	os::RegExp *pcRegexp;
	os::String cRemaining = cUrl;
	char *pnBuffer;
	size_t nUrlLength;

	pcRegexp = new os::RegExp();
	if( NULL == pcRegexp )
		return;

	nUrlLength = cUrl.Length() + 1;
	pnBuffer = (char*)malloc( nUrlLength );

	// Get the protocol
	pcRegexp->Compile( "[://]", false, true );
	if( pcRegexp->IsValid() == false )
		goto out;

	if( pcRegexp->Search( cRemaining ) == true )
	{
		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str(), pcRegexp->GetStart() );
		m->m_cProtocol = pnBuffer;

		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str() + pcRegexp->GetEnd() + 2, nUrlLength - pcRegexp->GetStart() );
		cRemaining = pnBuffer;
	}
	else
		goto out;

	// Get the path if one exists
	pcRegexp->Compile( "/", false, true );
	if( pcRegexp->IsValid() == false )
		goto out;

	if( pcRegexp->Search( cRemaining ) == true )
	{
		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str() + pcRegexp->GetStart(), nUrlLength - pcRegexp->GetStart() );
		m->m_cPath = pnBuffer;

		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str(), pcRegexp->GetStart() );
		cRemaining = pnBuffer;
	}

	// Find a username & password pair if they exist
	pcRegexp->Compile( "@", false, true );
	if( pcRegexp->IsValid() == false )
		goto out;

	if( pcRegexp->Search( cRemaining ) == true )
	{
		os::String cUserPass;

		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str(), pcRegexp->GetStart() );
		cUserPass = pnBuffer;

		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str() + pcRegexp->GetStart() + 1, nUrlLength - pcRegexp->GetStart() + 1 );
		cRemaining = pnBuffer;

		// Now split the username & password
		pcRegexp->Compile( ":", false, true );
		if( pcRegexp->IsValid() == false )
			goto out;

		if( pcRegexp->Search( cUserPass ) == true )
		{
			memset( pnBuffer, 0, nUrlLength );
			pnBuffer = strncpy( pnBuffer, cUserPass.c_str(), pcRegexp->GetStart() );
			m->m_cUsername = pnBuffer;

			memset( pnBuffer, 0, nUrlLength );
			pnBuffer = strncpy( pnBuffer, cUserPass.c_str() + pcRegexp->GetStart() + 1, nUrlLength - pcRegexp->GetStart() + 1 );
			m->m_cPassword = pnBuffer;
		}
	}

	// Find a port if one exists
	pcRegexp->Compile( ":", false, true );
	if( pcRegexp->IsValid() == false )
		goto out;

	if( pcRegexp->Search( cRemaining ) == true )
	{
		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str() + pcRegexp->GetStart() + 1, nUrlLength - pcRegexp->GetStart() + 1 );
		m->m_nPort = atoi( pnBuffer );

		memset( pnBuffer, 0, nUrlLength );
		pnBuffer = strncpy( pnBuffer, cRemaining.c_str(), pcRegexp->GetStart() );
		cRemaining = pnBuffer;
	}

	// Whatever is left must be the hostname
	m->m_cHostName = cRemaining;

out:
	free( pnBuffer );
	delete pcRegexp;
	return;
}


