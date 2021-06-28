/*  textview2.cpp - reusable component for AtheOS
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

#include "textview2.h"

// ---------------------------------------------------------------------------

TextView2::TextView2( const Rect& cFrame, const char* pzTitle, const char* pzText,
       uint32 nResizeMask, uint32 nFlags) : TextView(cFrame,pzTitle,pzText,nResizeMask,nFlags)
{    
    // Create a context menu
    m_pcContextMenu = new Menu( Rect(0,0,10,10), "", ITEMS_IN_COLUMN );
//  m_pcContextMenu->AddItem( "Cut",   new Message(ID_EDIT_CUT) );
    m_pcContextMenu->AddItem( "Copy",  new Message(ID_EDIT_COPY) );
//  m_pcContextMenu->AddItem( "Paste", new Message(ID_EDIT_PASTE) );
//  m_pcContextMenu->AddItem( "Clear", new Message(ID_EDIT_CLEAR) );
//  m_pcContextMenu->AddItem( "Select All", new Message(ID_EDIT_SELECT_ALL) );
    m_pcContextMenu->DetachedFromWindow();                         // Detached from the top of the window.
}

TextView2::~TextView2()
{
   if( m_pcContextMenu != NULL )  delete m_pcContextMenu;
}

void TextView2::MouseDown( const Point &cPosition, uint32 nButtons )
{
    if( nButtons == 2 )
    {
//      bool b = GetReadOnly();
//	m_pcContextMenu->FindItem( ID_EDIT_CUT )->Enable( ! b );
//	m_pcContextMenu->FindItem( ID_EDIT_PASTE )->Enable( b );
//	m_pcContextMenu->FindItem( ID_EDIT_CLEAR )->Enable( ! b );
//	m_pcContextMenu->FindItem( ID_EDIT_SELECT_ALL )->Enable( ! b );
        m_pcContextMenu->SetTargetForItems( this );
        m_pcContextMenu->Open( ConvertToScreen(cPosition) );	// Open the menu where the mouse is.
    }
    else
    {
	TextView::MouseDown( cPosition, nButtons );
    }
}

void TextView2::HandleMessage( Message* pcMessage )
{
    switch( pcMessage->GetCode() )  // Get the custom message code.
    {
        case ID_EDIT_CUT:
//          assert( ! GetReadOnly() );
            Cut();
            break;
        case ID_EDIT_COPY:
            if( GetReadOnly() )
	    {
                SelectAll();
                Copy();
                ClearSelection();
	    }
            else
	    {
                Copy();
	    }
            break;
        case ID_EDIT_PASTE:
//          assert( ! GetReadOnly() );
            Paste();
            break;
        case ID_EDIT_CLEAR:
//          assert( ! GetReadOnly() );
            Clear();
            break;
     case ID_EDIT_SELECT_ALL:
//          assert( ! GetReadOnly() );
            SelectAll();
            break;
        default:
            TextView::HandleMessage( pcMessage );
            break;
    }
}

// ---------------------------------------------------------------------------
