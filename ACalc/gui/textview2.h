/*  textview2.h - reusable component for AtheOS
 *  Copyright (C) 2001 Chir
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include <gui/textview.h>
#include <gui/menu.h>
#include <util/message.h>
#include <strings.h>
#include <stdio.h>

using namespace os;


class TextView2 : public TextView
{
    enum widget_ids
    {
       ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_EDIT_SELECT_ALL, ID_EDIT_CLEAR
    };
    Menu* m_pcContextMenu;
  public:
    TextView2( const Rect&, const char*, const char*, uint32, uint32);
    virtual ~TextView2();
    virtual void MouseDown( const Point& cPosition, uint32 nButtons );
    virtual void HandleMessage( Message* pcMessage );
};

