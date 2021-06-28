
#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/listview.h>
#include <util/application.h>

using namespace os;


class TestView : public View
{
  public:
    TestView( const Rect & r ); /* constructor */
    virtual void Init();
};

TestView :: TestView( const Rect & r )
    : View( r, "TestView", CF_FOLLOW_LEFT | CF_FOLLOW_TOP, WID_WILL_DRAW | WID_CLEAR_BACKGROUND )
{
  Init();
}

void TestView :: Init()
{
  ListView *myListView; 
  Rect r = GetBounds(); 
  myListView = new ListView( r, "myListView", 
                             ListView::F_RENDER_BORDER, 
                             CF_FOLLOW_ALL ); 
  
  myListView->InsertColumn( "First Name", static_cast<int>(GetBounds().Width() / 3) ); 
  myListView->InsertColumn( "Last name", static_cast<int>(GetBounds().Width() / 3) ); 
  myListView->InsertColumn( "Age", static_cast<int>(GetBounds().Width() / 3) ); 

  ListViewStringRow *row; 
  row = new ListViewStringRow(); 
  row->AppendString( "Heero" ); 
  row->AppendString( "Yui" ); 
  row->AppendString( "15" ); 
  myListView->InsertRow( row ); 
  
  row = new ListViewStringRow(); 
  row->AppendString( "Relena" ); 
  row->AppendString( "Peacecraft" ); 
  row->AppendString( "15" ); 
  myListView->InsertRow( row ); 
  
  row = new ListViewStringRow(); 
  row->AppendString( "Treize" ); 
  row->AppendString( "Kushrenada" ); 
  row->AppendString( "22" ); 
  myListView->InsertRow( row ); 

  AddChild( myListView );
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



