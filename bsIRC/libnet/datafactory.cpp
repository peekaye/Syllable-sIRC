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

#include "datafactory.h"
#include "httpstream.h"
#include <util/string.h>
#include <storage/file.h>

using namespace net;

Datafactory::Datafactory()
{

}

Datafactory::~Datafactory()
{

}

/**
 * \par		Description:
 *		Creates a SeekableIO stream of the appropriate type specified by the protocol part
 *		of the cUrl.  E.g. the URL "http://www.example.com/file.html" will cause an HTTPstream
 *		to be created, whilst "file:///home/user/file.txt" will cause an os::File object to be
 *		created.  Currently only http:// and file:// are understood.
 * \param cUrl The URL of the file.  The protocol must be valid and one of either http:// or file://
 * \notes The SeekableIO object returned by this method is owned by the caller and should be deleted
 *		when the stream is no longer required.
 * \author Kristian Van Der Vliet (vanders@liqwyd.com)
 *****************************************************************************/
os::SeekableIO* Datafactory::CreateStream( Url &cUrl )
{
	os::String cProtocol = cUrl.GetProtocol();

	// In the final implementation we'll need a much quicker & less messy way to
	// do this, but when you only have two protocols to worry about this will do.
	//
	// For the final we might want to use some form of hash-based LUT?
	if( cProtocol == "http" )
	{
		return new HTTPstream( cUrl );
	}
	else if( cProtocol == "file" )
	{
		return new os::File( cUrl.GetPath() );
	}
	else
		return NULL;
}

