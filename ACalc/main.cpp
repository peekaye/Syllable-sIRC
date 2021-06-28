/*  main.cpp - the main program for the AtheOS Calculator v0.3
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

#include <cassert>

#include "myapp.h"

// ---------------------------------------------------------------------------

MyTextView::MyTextView( const Rect& cFrame, const char* pzTitle, const char* pzText,
       uint32 nResizeMask, uint32 nFlags) : TextView2(cFrame,pzTitle,pzText,nResizeMask,nFlags)
{
    Font* f = new Font();
    f->SetFamilyAndStyle("LCD2", "Normal" );  f->SetSize( 15 );
    SetFont( f );
    f->Release();    // Decrease reference count.   // delete f;
    SetReadOnly( true );  SetMultiLine( false );
       
    m_bHasPoint = false;  m_bHasDigit = false;
}

/** Copies the content of the internal buffer, into the Text gadget. */
void MyTextView::refreshDisplay()
{
    Set( (m_buffer.size()==0)  ?  "0"  :  m_buffer.c_str() );
}

/** Clears both the internal buffer and the Text gadget. */
void MyTextView::clearDisplay()
{
    m_buffer = "";
    m_bHasPoint = false;  m_bHasDigit = false;
    refreshDisplay();
}

/** Adds one digit to the right of the current display. */
void MyTextView::appendDisplay( char* s )
{
    assert( strlen(s) == 1 );
    if( m_buffer.size() <= D_SIZE )
    {
        char c = s[0];
       
        if( ( c != '0'  ||  m_bHasDigit )  &&  ( c != '.'  ||  ! m_bHasPoint ) )
        {
            m_buffer += s;
            refreshDisplay();
        }
        if( c == '.' )  { m_bHasPoint = true;  m_bHasDigit = true; }
        if( c >= '1'  &&  c <= '9' )  m_bHasDigit = true;
    }
}

/** Changes the content of the internal buffer. */
void MyTextView::setDisplay( char* s )
{
    m_buffer = s;
    if( m_buffer.size() == 0 )
    {
        m_bHasPoint = false;  m_bHasDigit = false;
    }
}

/** Changes the content of the internal buffer. */
void MyTextView::setDisplay( float x )
{
    char s[80];
   
    sprintf( s, "%g", x );
    setDisplay( s );
}

/** Obtains the value currently shown. */
double MyTextView::getOperand()
{
    return atof( m_buffer.c_str() );
}

// ---------------------------------------------------------------------------

MyWindow::MyWindow(const Rect& cFrame) : Window( cFrame, "main_window", "Calculator", WND_NOT_RESIZABLE )
{
    int const X0 = 10  ,  Y0 = 20  ,  L = 25  ,  D = L + 1;
    
    // --- Create gadgets ---
    m_pcZeroB   = new MyButton( Rect(    X0, Y0+5*D,   X0+D+L, Y0+5*D+L) , "zero",  "0", new Message(ID_KEY_0)     );
    m_pcOneB    = new MyButton( Rect(    X0, Y0+4*D,     X0+L, Y0+4*D+L) , "one",   "1", new Message(ID_KEY_1)     );
    m_pcTwoB    = new MyButton( Rect(  X0+D, Y0+4*D,   X0+D+L, Y0+4*D+L) , "two",   "2", new Message(ID_KEY_2)     );
    m_pcThreeB  = new MyButton( Rect(X0+2*D, Y0+4*D, X0+2*D+L, Y0+4*D+L) , "three", "3", new Message(ID_KEY_3)     );
    m_pcFourB   = new MyButton( Rect(    X0, Y0+3*D,     X0+L, Y0+3*D+L) , "four",  "4", new Message(ID_KEY_4)     );
    m_pcFiveB   = new MyButton( Rect(  X0+D, Y0+3*D,   X0+D+L, Y0+3*D+L) , "five",  "5", new Message(ID_KEY_5)     );
    m_pcSixB    = new MyButton( Rect(X0+2*D, Y0+3*D, X0+2*D+L, Y0+3*D+L) , "six",   "6", new Message(ID_KEY_6)     );
    m_pcSevenB  = new MyButton( Rect(    X0, Y0+2*D,     X0+L, Y0+2*D+L) , "seven", "7", new Message(ID_KEY_7)     );
    m_pcEightB  = new MyButton( Rect(  X0+D, Y0+2*D,   X0+D+L, Y0+2*D+L) , "eight", "8", new Message(ID_KEY_8)     );
    m_pcNineB   = new MyButton( Rect(X0+2*D, Y0+2*D, X0+2*D+L, Y0+2*D+L) , "nine",  "9", new Message(ID_KEY_9)     );
    m_pcPointB  = new MyButton( Rect(X0+2*D, Y0+5*D, X0+2*D+L, Y0+5*D+L) , "point", ".", new Message(ID_KEY_DOT)   );
    m_pcEqualB  = new MyButton( Rect(X0+3*D, Y0+4*D, X0+3*D+L, Y0+5*D+L) , "eq",    "=", new Message(ID_KEY_EQUAL) );
    m_pcAddB    = new MyButton( Rect(X0+3*D, Y0+2*D, X0+3*D+L, Y0+3*D+L) , "add",   "+", new Message(ID_KEY_ADD)   );
    m_pcSubB    = new MyButton( Rect(X0+3*D,   Y0+D, X0+3*D+L,   Y0+D+L) , "sub",   "-", new Message(ID_KEY_SUB)   );
    m_pcMultB   = new MyButton( Rect(X0+2*D,   Y0+D, X0+2*D+L,   Y0+D+L) , "mult",  "x", new Message(ID_KEY_MULT)  );
    m_pcDivB    = new MyButton( Rect(  X0+D,   Y0+D,   X0+D+L,   Y0+D+L) , "div",   "/", new Message(ID_KEY_DIV)   );
    m_pcClearB  = new MyButton( Rect(    X0,   Y0+D,     X0+L,   Y0+D+L) , "clear", "C", new Message(ID_KEY_CLR)   );
    m_pcDisplay = new MyTextView( Rect(X0,Y0-10,X0+3*D+L,Y0+L-10),
                                  "text_view",
                                  "0.",
                                  CF_FOLLOW_LEFT, WID_WILL_DRAW );

    m_pcZeroB->SetBgColor( 235,235,220 );   
    m_pcOneB->SetBgColor( 235,235,220 );   
    m_pcTwoB->SetBgColor( 235,235,220 );   
    m_pcThreeB->SetBgColor( 235,235,220 );
    m_pcFourB->SetBgColor( 235,235,220 );
    m_pcFiveB->SetBgColor( 235,235,220 );
    m_pcSixB->SetBgColor( 235,235,220 );
    m_pcSevenB->SetBgColor( 235,235,220 );
    m_pcEightB->SetBgColor( 235,235,220 );
    m_pcNineB->SetBgColor( 235,235,220 );
    m_pcPointB->SetBgColor( 235,235,220 );
    m_pcEqualB->SetBgColor( 165,165,155 );
    m_pcAddB->SetBgColor( 165,165,155 );
    m_pcSubB->SetBgColor( 165,165,155 );
    m_pcMultB->SetBgColor( 165,165,155 );
    m_pcDivB->SetBgColor( 165,165,155 );
    m_pcClearB->SetFgColor( 255,250,250 );
    m_pcClearB->SetBgColor( 200,100,50 );
/*
    m_pcZeroB->SetFgColor( 0,0,0 );
    m_pcOneB->SetFgColor( 0,0,0 );
    m_pcTwoB->SetFgColor( 0,0,0 );   
    m_pcThreeB->SetFgColor( 0,0,0 );
    m_pcFourB->SetFgColor( 0,0,0 );
    m_pcFiveB->SetFgColor( 0,0,0 );
    m_pcSixB->SetFgColor( 0,0,0 );
    m_pcSevenB->SetFgColor( 0,0,0 );
    m_pcEightB->SetFgColor( 0,0,0 );
    m_pcNineB->SetFgColor( 0,0,0 );
    m_pcPointB->SetFgColor( 0,0,0 );
    m_pcEqualB->SetFgColor( 0,0,0 );
    m_pcAddB->SetFgColor( 0,0,0 );
    m_pcSubB->SetFgColor( 0,0,0 );
    m_pcMultB->SetFgColor( 0,0,0 );
    m_pcDivB->SetFgColor( 0,0,0 );
*/
    // --- Add gadgets to window ---
    AddChild( m_pcZeroB,   true );
    AddChild( m_pcOneB,    true );
    AddChild( m_pcTwoB,    true );
    AddChild( m_pcThreeB,  true );
    AddChild( m_pcFourB,   true );
    AddChild( m_pcFiveB,   true );
    AddChild( m_pcSixB,    true );
    AddChild( m_pcSevenB,  true );
    AddChild( m_pcEightB,  true );
    AddChild( m_pcNineB,   true );
    AddChild( m_pcPointB,  true );
    AddChild( m_pcEqualB,  true );
    AddChild( m_pcAddB,    true );
    AddChild( m_pcSubB,    true );
    AddChild( m_pcMultB,   true );
    AddChild( m_pcDivB,    true );
    AddChild( m_pcClearB,  true );
    AddChild( m_pcDisplay, true );
   
    m_pcEqualB->setNeedsFocus( this );

    clearDisplay();
}

MyWindow::~MyWindow()
{
   delete m_pcZeroB;  delete m_pcOneB;   delete m_pcTwoB;   delete m_pcThreeB;
   delete m_pcFourB;  delete m_pcFiveB;  delete m_pcSixB;   delete m_pcSevenB;
   delete m_pcEightB; delete m_pcNineB;  delete m_pcPointB; delete m_pcAddB;
   delete m_pcSubB;   delete m_pcMultB;  delete m_pcDivB;   delete m_pcEqualB;
   delete m_pcClearB; delete m_pcDisplay;
}

bool MyWindow::OkToQuit( void )
{
    Application::GetInstance()->PostMessage( M_QUIT );
    return( false );
}

void MyWindow::HandleMessage( Message* pcMessage )
{
    switch( pcMessage->GetCode() )  // Get the custom message code.
    {
        case ID_EDIT_COPY:
            m_pcDisplay->SelectAll();
            m_pcDisplay->Copy();
            m_pcDisplay->ClearSelection();
            break;
        case ID_KEY_0:  m_pcDisplay->appendDisplay("0");  break;
        case ID_KEY_1:  m_pcDisplay->appendDisplay("1");  break;
        case ID_KEY_2:  m_pcDisplay->appendDisplay("2");  break;
        case ID_KEY_3:  m_pcDisplay->appendDisplay("3");  break;
        case ID_KEY_4:  m_pcDisplay->appendDisplay("4");  break;
        case ID_KEY_5:  m_pcDisplay->appendDisplay("5");  break;
        case ID_KEY_6:  m_pcDisplay->appendDisplay("6");  break;
        case ID_KEY_7:  m_pcDisplay->appendDisplay("7");  break;
        case ID_KEY_8:  m_pcDisplay->appendDisplay("8");  break;
        case ID_KEY_9:  m_pcDisplay->appendDisplay("9");  break;
        case ID_KEY_DOT: m_pcDisplay->appendDisplay("."); break;
        case ID_KEY_ADD: case ID_KEY_SUB: case ID_KEY_MULT: case ID_KEY_DIV : case ID_KEY_EQUAL:
            appendOperator( pcMessage->GetCode() );
            break;
        case ID_KEY_CLR:
            clearDisplay();
            break;
        default:
            Window::HandleMessage( pcMessage );
            break;
    }
}

/** Converts keypresses into GUI events. */
bool MyWindow::keyUp( const char * pzString,  const char * pzRawString,  uint32 nQualifiers )
{
   bool b = true;
   Message m;

   switch( pzString[0] )
   {
        case '0':  m.SetCode( ID_KEY_0 );  break;
        case '1':  m.SetCode( ID_KEY_1 );  break;
        case '2':  m.SetCode( ID_KEY_2 );  break;
        case '3':  m.SetCode( ID_KEY_3 );  break;
        case '4':  m.SetCode( ID_KEY_4 );  break;
        case '5':  m.SetCode( ID_KEY_5 );  break;
        case '6':  m.SetCode( ID_KEY_6 );  break;
        case '7':  m.SetCode( ID_KEY_7 );  break;
        case '8':  m.SetCode( ID_KEY_8 );  break;
        case '9':  m.SetCode( ID_KEY_9 );  break;
        case '.':  m.SetCode( ID_KEY_DOT );  break;
        case '+':  m.SetCode( ID_KEY_ADD );  break;
        case '-':  m.SetCode( ID_KEY_SUB );  break;
        case '*':  m.SetCode( ID_KEY_MULT );  break;
        case '/':  m.SetCode( ID_KEY_DIV );  break;
        case '=':  m.SetCode( ID_KEY_EQUAL );  break;
        case 'C':  case 'c': m.SetCode( ID_KEY_CLR );  break;
        default:
            // Not processed at this level (pass it to the button's base class).
            b = false;  break;
   }
   if( b )  HandleMessage( &m );    // PostMessage( &m );
   return b;
}

/** Clears both the internal buffer and the Text gadget. */
void MyWindow::clearDisplay()
{
    m_nOpcode = ID_KEY_EQUAL;  m_vOperand = 0.0;
    m_pcDisplay->clearDisplay();  m_pcDisplay->refreshDisplay();
}

void MyWindow::appendOperator( int k )
{
    if( m_nOpcode != ID_KEY_EQUAL )
    {
        // A previous operation still has to be done.
        switch( m_nOpcode )
        {
            case ID_KEY_ADD:
                m_vOperand += m_pcDisplay->getOperand();
                break;
            case ID_KEY_SUB:
                m_vOperand -= m_pcDisplay->getOperand();
                break;
            case ID_KEY_MULT:
                m_vOperand *= m_pcDisplay->getOperand();
                break;
            case ID_KEY_DIV:
                m_vOperand /= m_pcDisplay->getOperand();
                break;
            default:
                assert( false );    // Should never reach here !
        }
        // Show result.
        m_pcDisplay->setDisplay( m_vOperand );
        m_pcDisplay->refreshDisplay();
    }
    else
    {
        // Use the last result.
        if( ! m_pcDisplay->isEmpty() )  m_vOperand = m_pcDisplay->getOperand();
    }

    m_nOpcode = k;
    m_pcDisplay->setDisplay( "" );              // Clear internal buffer, but don't refresh display.
}

// ---------------------------------------------------------------------------

MyApp::MyApp() : Application( "application/x-VND.KHS-Calculator" )
{
    m_pcMainWindow = new MyWindow( Rect(120,120,243,305) );
    m_pcMainWindow->Show();
    m_pcMainWindow->MakeFocus();
}

// ---------------------------------------------------------------------------

int main( int argc, char** argv )
{
    MyApp* pcApp = new MyApp();
    
    pcApp->Run();                               // Enter main GUI loop.
    return( 0 );
}

// ---------------------------------------------------------------------------
