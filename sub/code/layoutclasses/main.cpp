
#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/rect.h>
#include <gui/stringview.h>
#include <gui/button.h>
#include <util/application.h>
#include <util/message.h>

using namespace os;


const uint32 MSG_START = 101; 
const uint32 MSG_STOP = 102; 
const uint32 MSG_RESET = 103; 

class StopClockView : public LayoutView
{
  public:
    StopClockView(); /* constructor */
  private:
    bool m_bStart; 
    int m_nDeciSeconds; 
    int m_nSeconds; 
    int m_nMinutes; 
    int m_nHours;

    StringView *m_pcTime; 
    Button *m_pcStart, *m_pcStop, *m_pcReset; 
};

StopClockView :: StopClockView()
    : LayoutView(Rect(0, 0, 220, 80), "StopClockView") 
{
  //initialise clock 
  m_bStart = false; 
  m_nDeciSeconds = 0; 
  m_nSeconds = 0; 
  m_nMinutes = 0; 
  m_nHours = 0; 

  //create the vertical root layout node 
  LayoutNode* pcRootNode = new VLayoutNode("Root"); 

  //create and add the time 
  m_pcTime = new StringView(Rect(0, 0, 0, 0), "Time", "00:00:00.0", ALIGN_CENTER); 
  m_pcTime->GetFont()->SetSize(20); 
  pcRootNode->AddChild(m_pcTime); 

  //create the horizontal layout node containing the buttons 
  LayoutNode* pcButtonNode = new HLayoutNode("Buttons");

  m_pcStart = new Button(Rect(0, 0, 0, 0), "Start", "Start", new Message(MSG_START)); 
  m_pcStart->GetFont()->SetSize(10); 
  pcButtonNode->AddChild(m_pcStart); 
  
  pcButtonNode->AddChild(new HLayoutSpacer("Spacer1", 20, 20)); 
  
  m_pcStop = new Button(Rect(0, 0, 0, 0), "Stop", "Stop", new Message(MSG_STOP)); 
  m_pcStop->GetFont()->SetSize(10); 
  pcButtonNode->AddChild(m_pcStop); 
  
  pcButtonNode->AddChild(new HLayoutSpacer("Spacer2", 20, 20)); 
  
  m_pcReset = new Button(Rect(0, 0, 0, 0), "Reset", "Reset", new Message(MSG_RESET)); 
  m_pcReset->GetFont()->SetSize(10); 
  pcButtonNode->AddChild(m_pcReset); 

  //make the buttons the same size 
  pcButtonNode->SameWidth("Start", "Stop", "Reset", NULL); 

  //add the row of buttons to the vertical layout node 
  pcRootNode->AddChild(pcButtonNode); 
  
  //add it all to this layout view 
  SetRoot(pcRootNode); 
}


class TestWindow : public Window
{
  public:
    TestWindow( const Rect & r ); /* constructor */
    bool OkToQuit();
  private:
    StopClockView *view;
};

TestWindow :: TestWindow( const Rect & r )
    : Window( r, "TestWindow", "Hello World", 0, CURRENT_DESKTOP )
{
  /* Set up "view" as the main view for the window, filling up the entire */
  /* window (thus the call to GetBounds()). */
  view = new StopClockView();
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
