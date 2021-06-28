/*  colorbutton.cpp - reusable component for AtheOS
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

#include "colorbutton.h"

using namespace os;


void ColorButton::Paint( const Rect& cUpdateRect )
{
    Rect cBounds = GetNormalizedBounds();
    font_height sHeight;
    GetFontHeight( &sHeight );
    float vStrWidth = GetStringWidth( GetLabel() );
    float vCharHeight = sHeight.ascender - sHeight.descender + sHeight.line_gap;
    float y = floor( 2 + (cBounds.Height()+1.0f)*0.5f - vCharHeight*0.5f + sHeight.ascender + sHeight.line_gap / 2 );
    float x = floor((cBounds.Width()+1.0f) / 2.0f - vStrWidth / 2.0f);

    if( GetValue() )
    {
        y += 1.0f;
        x += 1.0f;
    }

    SetEraseColor( sBgColor );
    DrawFrame( cBounds, (GetValue()) ? FRAME_RECESSED : FRAME_RAISED );

    View::SetFgColor( sTxtColor );
    View::SetBgColor( sBgColor );
    MovePenTo( x, y );
    DrawString( GetLabel() );
}
