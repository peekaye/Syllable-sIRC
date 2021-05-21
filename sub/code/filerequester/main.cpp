
#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/filerequester.h>
#include <util/application.h>
#include <util/messenger.h>
#include <util/message.h>

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
    virtual void HandleMessage( Message *pcMessage );
    virtual bool OkToQuit();
  private:
    TestView *view;
};

TestWindow :: TestWindow( const Rect & r )
    : Window( r, "TestWindow", "Window", 0, CURRENT_DESKTOP )
{
  FileRequester *fileRequester; 
  fileRequester = new FileRequester( FileRequester::LOAD_REQ, 
                                                      new Messenger( this ), 
                                                      "/", /* Starting directory */ 
                                                      FileRequester::NODE_FILE, 
                                                      false, NULL, NULL, false, true, 
                                                      "OK", "Cancel" ); 
  fileRequester->Show(); 
}

void TestWindow :: HandleMessage( Message *pcMessage ) 
{ 
  switch( pcMessage->GetCode() ) 
  { 
    case M_LOAD_REQUESTED: 
    { 
      const char *fileNameString; 
      pcMessage->FindString( "file/path", &fileNameString ); 
      /* Do what you want to with fileNameString */ 
    } 
  } 
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


