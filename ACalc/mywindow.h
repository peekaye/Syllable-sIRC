/*  mywindow.h - the main window for the AtheOS Calculator v0.3
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
#include <gui/button.h>
#include <gui/menu.h>
#include "gui/textview2.h"
#include "gui/colorbutton.h"
#include <util/message.h>
#include <strings.h>
#include <stdio.h>

using namespace os;

/** The LCD display */
class MyTextView : public TextView2
{
    static unsigned int const D_SIZE = 8; // Maximum Number of digits on screen.
    bool m_bHasPoint;                     // Indicates if "." was already pressed.
    bool m_bHasDigit;                     // Indicates if "0" can be added to the right of the current number.
    std::string m_buffer;                 // Current display's content.
  public:
    MyTextView( const Rect&, const char*, const char*, uint32, uint32);
    void refreshDisplay();
    void appendDisplay( char* );
    void appendOperator( int );
    void clearDisplay();
    void setDisplay( char*);
    void setDisplay( float );
    double getOperand();
    bool isEmpty()   { return (m_buffer.size() == 0); }
};

class MyButton;

class MyWindow : public Window
{
    int     m_nOpcode;              // Current arithmetic operation.
    double  m_vOperand;             // Last result.
    // Gadgets.
    MyButton *m_pcZeroB,  *m_pcOneB,  *m_pcTwoB,   *m_pcThreeB, *m_pcFourB, *m_pcFiveB, *m_pcSixB, *m_pcSevenB,
             *m_pcEightB, *m_pcNineB, *m_pcPointB, *m_pcAddB,   *m_pcSubB,  *m_pcMultB, *m_pcDivB, *m_pcEqualB,
             *m_pcClearB;
    MyTextView* m_pcDisplay;        // The visible LCD display.

    void refreshDisplay();
    void appendDisplay( char* );
    void appendOperator( int );
    void clearDisplay();
    void setDisplay( char*);
    void setDisplay( float );
    double getOperand();
  public:
    MyWindow( const Rect& cFrame );
    ~MyWindow();
    virtual void HandleMessage( Message* pcMessage );
    virtual bool OkToQuit( void );
    bool keyUp( const char * pzString,  const char * pzRawString,  uint32 nQualifiers );
    enum widget_ids
    {
        ID_KEY_0,    ID_KEY_1,   ID_KEY_2,   ID_KEY_3,     ID_KEY_4,     ID_KEY_5,   ID_KEY_6,
        ID_KEY_7,    ID_KEY_8,   ID_KEY_9,   ID_KEY_DOT, ID_KEY_EQUAL, ID_KEY_ADD, ID_KEY_SUB,
        ID_KEY_MULT, ID_KEY_DIV, ID_KEY_CLR,
	ID_EDIT_COPY
    };
};

class MyButton : public ColorButton
{
    MyWindow* m_pcParent;
    bool needsFocus;                // Indicates if this button must always try to retain focus.
  public:
    MyButton( const Rect& cFrame, char* s1, char* s2, Message* m ) : ColorButton(cFrame,s1,s2,m)
    {
       needsFocus = false;
       m_pcParent = NULL;
    }
    virtual ~MyButton()  { }

    virtual void KeyUp( const char * pzString,  const char * pzRawString,  uint32 nQualifiers )
    {
        // Transmit signal to main window, or to button's base class.
        if( ! m_pcParent->keyUp( pzString, pzRawString, nQualifiers ) )
        {
            ColorButton::KeyUp( pzString, pzRawString, nQualifiers );
        }    
    }
    virtual void Activated( bool b )
    {
        if( ! b  &&  needsFocus )
        {
            MakeFocus( true );       // Ensure focus is never lost.
        }
    }

    /** Marks this button as the chosen one for keyboard focus. */
    void setNeedsFocus( MyWindow* parent )
    {
       needsFocus = true;
       m_pcParent = parent;
       MakeFocus( true );
    }
};
