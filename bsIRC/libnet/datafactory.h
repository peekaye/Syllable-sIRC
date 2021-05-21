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

#ifndef	__F_NET_DATAFACTORY_H__
#define	__F_NET_DATAFACTORY_H__ 1

#include <storage/seekableio.h>
#include "url.h"

namespace net{

class Datafactory
{
public:
	Datafactory();
	~Datafactory();

	os::SeekableIO* CreateStream( Url &cUrl );
};

};

#endif	/* __F_NET_DATAFACTORY_H__ */

