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

#ifndef	__F_NET_URL_H__
#define	__F_NET_URL_H__ 1

#include <util/string.h>

namespace net{

class Url
{
public:
	Url( void );
	Url( const os::String &cUrl );
	virtual ~Url( void );

	os::String GetUrl( void );
	void SetUrl( os::String &cUrl );

	os::String GetProtocol( void );
	void SetProtocol( os::String &cProtocol );

	os::String GetHostName( void );
	void SetHostName( os::String &cHostName );

	uint32 GetPort( void );
	void SetPort( uint32 nPort );

	os::String GetPath( void );
	void SetPath( os::String &cPath );

	os::String GetUsername( void );
	void SetUsername( os::String &cUsername );

	os::String GetPassword( void );
	void SetPassword( os::String &cPassword );

private:
	void ParseUrl( const os::String &cUrl );
private:
	virtual void	__URL_reserved1__() {}
	virtual void	__URL_reserved2__() {}
	virtual void	__URL_reserved3__() {}
	virtual void	__URL_reserved4__() {}
	virtual void	__URL_reserved5__() {}
private:
	class Private;
	Private *m;
};

};

#endif	/* __F_NET_URL_H__ */

