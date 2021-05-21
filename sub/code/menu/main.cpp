
#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/menu.h>
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
    virtual void Init();
    virtual bool OkToQuit();
  private:
    TestView *view;
};

TestWindow :: TestWindow( const Rect & r )
    : Window( r, "TestWindow", "Window", 0, CURRENT_DESKTOP )
{
  Init();
}

void TestWindow :: Init()
{
  Rect r = GetBounds(); 
  r.bottom = 18; 
  Menu *mainMenuBar, *tempMenu; 
  mainMenuBar = new Menu( r, "mainMenuBar", ITEMS_IN_ROW ); 
  // App menu 
  tempMenu = new Menu( Rect(0,0,0,0), "App", ITEMS_IN_COLUMN ); 
  tempMenu->AddItem( "Settings", NULL ); 
  tempMenu->AddItem( "About", NULL ); 
  tempMenu->AddItem( "Quit", NULL ); 
  mainMenuBar->AddItem( tempMenu ); 
  // File menu 
  tempMenu = new Menu( Rect(0,0,0,0), "File", ITEMS_IN_COLUMN ); 
  tempMenu->AddItem( "New", NULL ); 
  tempMenu->AddItem( "Save", NULL ); 
  tempMenu->AddItem( "Open", NULL ); 
  tempMenu->AddItem( "Close", NULL ); 
  mainMenuBar->AddItem( tempMenu ); 
  // Help menu 
  tempMenu = new Menu( Rect(0,0,0,0), "Help", ITEMS_IN_COLUMN ); 
  tempMenu->AddItem( "Contents", NULL ); 
  tempMenu->AddItem( "Homepage", NULL ); 
  mainMenuBar->AddItem( tempMenu ); 
  AddChild( mainMenuBar ); 
  mainMenuBar->SetTargetForItems( this ); 
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

