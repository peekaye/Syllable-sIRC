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

#include "httpstream.h"
#include "host.h"

using namespace net;

HTTPstream::HTTPstream(Url &cUrl) : Datastream( cUrl )
{
	uint8 pzBuffer[1024];
	std::string cGet = "GET http://";
	cGet += cUrl.GetHostName();
	cGet += cUrl.GetPath();
	cGet += " HTTP/1.0\r\n\r\n";

	Datastream::Write( cGet.c_str(), cGet.size() );
	WaitFor( "\r\n\r\n", pzBuffer, 1024 );
}

HTTPstream::~HTTPstream()
{
	Disconnect();
}

ssize_t HTTPstream::Write( const void* pBuffer, ssize_t nSize )
{
	return -EIO;
}

