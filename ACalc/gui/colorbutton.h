/*  colorbutton.h - reusable component for AtheOS
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

#include <gui/window.h>
#include <gui/textview.h>
#include <gui/button.h>
#include <util/message.h>
#include <strings.h>
#include <stdio.h>

using namespace os;


class ColorButton : public Button
{
    Color32_s sTxtColor;
    Color32_s sBgColor;
  public:
    void SetFgColor( int r, int g, int b )          // Override baseclass definition.
    {
       sTxtColor.red = r;  sTxtColor.green = g;  sTxtColor.blue = b;
    }
    void SetBgColor( int r, int g, int b )          // Override baseclass definition.
    {
       sBgColor.red = r;  sBgColor.green = g;  sBgColor.blue = b;
    }
    void SetBgColor( Color32_s c )                  // Override baseclass definition.
    {
       sBgColor.red = c.red;  sBgColor.green = c.green;  sBgColor.blue = c.blue;
    }

    ColorButton( const Rect& cFrame, char* s1, char* s2, Message* m ) : Button(cFrame,s1,s2,m)
    {
       SetFgColor( 0, 0, 0 );
       SetBgColor( get_default_color( COL_NORMAL ) );
    }
    virtual ~ColorButton()  { }

    virtual void Paint( const Rect& cUpdateRect );
};
