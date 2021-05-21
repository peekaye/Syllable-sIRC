
#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <util/application.h>

using namespace os;


class TestView : public View
{
  public:
    TestView( const Rect & r ); /* constructor */
};

TestView :: TestView( const Rect & r )
    : View( r, "TestView", CF_FOLLOW_LEFT | CF_FOLLOW_TOP, WID_WILL_DRAW | WID_CLEAR_BACKGROUND )
{
  /* Just let the View construtor do its thing */
}


class TestWindow : public Window
{
  public:
    TestWindow( const Rect & r ); /* constructor */
    bool OkToQuit();
  private:
    TestView *view;
};

TestWindow :: TestWindow( const Rect & r )
    : Window( r, "TestWindow", "Hello World", 0, CURRENT_DESKTOP )
{
  /* Set up "view" as the main view for the window, filling up the entire */
  /* window (thus the call to GetBounds()). */
  view = new TestView( GetBounds() );
  AddChild( view );
}

bool TestWindow :: OkToQuit( void )
{
  Application::GetInstance()->PostMessage( M_QUIT );
  return( true );
}


class MyApplication : public Application
{
  public:
    MyApplication( void ); /* constructor */
  private:
    TestWindow *myTestWindow;
};

MyApplication :: MyApplication( void )
    : Application( "application/x-vnd.Tutorial-HelloWorld" )
{
  myTestWindow = new TestWindow( Rect(20,20,300,200) );
  myTestWindow->Show();
}


int main( void )
{
  MyApplication *thisApp;
  thisApp = new MyApplication();
  thisApp->Run();
  return( 0 );
}
