 
#include <gui/window.h> 
#include <gui/button.h> 
#include <gui/listview.h> 
#include <gui/view.h> 
#include <util/application.h> 
#include <util/message.h> 
#include <atheos/types.h> 
#include <posix/errno.h> 
 
using namespace os; 
 
/* Define an identifier for the Reset Button message. */ 
const unsigned int RESET_BUTTON_PRESSED_MESSAGE = 1; 
 
 
class MyButton : public Button 
{ 
  public: 
    MyButton( Rect cFrame, const String& cName, const String& cLabel, Message* pcMessage ); 
    status_t Invoke(Message *a_pcMessage); 
}; 
 
 
MyButton :: MyButton( Rect cFrame, const String& cName, const String& cLabel, Message* pcMessage ) 
    : Button( cFrame, cName, cLabel, pcMessage ) 
{ 
 
} 
 
status_t MyButton :: Invoke(Message *a_pcMessage) 
{ 
  /* Do something interesting here. */ 
  Application::GetInstance()->PostMessage( M_QUIT ); 
  return EOK; 
} 
 
 
class MyView : public View 
{ 
  public: 
    MyView( const Rect & r ); /* constructor */ 
}; 
 
MyView :: MyView( const Rect & r ) 
    : View( r, "MyView", CF_FOLLOW_LEFT | CF_FOLLOW_TOP, WID_WILL_DRAW | WID_CLEAR_BACKGROUND ) 
{ 
  MyButton *resetButton; 
  Message *message; 
//  Rect r; 
  /* Create a new Message, for the Button */ 
  message = new Message( RESET_BUTTON_PRESSED_MESSAGE ); 
  /* Create the Reset Button, telling it to send _message_ when it is clicked. */ 
  resetButton = new MyButton( Rect( 100, 100, 160, 125 ), "resetButton", "Reset", message ); 
  AddChild( resetButton ); 
} 
 
 
class MyWindow : public Window 
{ 
  public: 
    MyWindow( const Rect & r ); /* constructor */ 
    void HandleMessage( Message &pcMessage ); 
    bool OkToQuit(); 
  private: 
    MyView *view; 
}; 
 
 
MyWindow :: MyWindow( const Rect & r ) 
    : Window( r, "MyWindow", "MyWindow", 0, CURRENT_DESKTOP ) 
{ 
  /* Set up "view" as the main view for the window, filling up the entire */ 
  /* window (thus the call to GetBounds()). */ 
  view = new MyView( GetBounds() ); 
  AddChild( view ); 
} 
 
void MyWindow :: HandleMessage( Message &pcMessage ) 
{ 
  switch( pcMessage.GetCode() ) 
  { 
    case RESET_BUTTON_PRESSED_MESSAGE: 
      /* Do whatever when the Reset Button is clicked */ 
      break; 
    default: 
      Window :: HandleMessage( &pcMessage ); 
  } 
} 
 
bool MyWindow :: OkToQuit( void ) 
{ 
  Application::GetInstance()->PostMessage( M_QUIT ); 
  return( true ); 
} 
 
 
class MyApplication : public Application 
{ 
  public: 
    MyApplication( void ); /* constructor */ 
  private: 
    MyWindow *myWindow; 
}; 
 
MyApplication :: MyApplication( void ) 
    : Application( "application/x-vnd.Tutorial-Intro" ) 
{ 
  myWindow = new MyWindow( Rect(20,20,300,200) ); 
  myWindow->Show(); 
} 
 
 
int main( void ) 
{ 
  MyApplication *thisApp; 
  thisApp = new MyApplication(); 
  thisApp->Run(); 
  return( 0 ); 
} 
 
