
/**
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "sIRC.h"
#include "commthread.h"
#include "messages.h"

//User Variables
const os::String HOST = "chat.freenode.net";
const os::String PORT = "6667";
const os::String CHAN = "#syllable";
const os::String NICK = "sIRC_";
const os::String USER = "sIRC_";
const os::String PASS = "password";
const os::String NAME = "Syllable User";

//----------------------------------------------------------------------------------

// Here is where we construct the window e.g views, buttons, textviews, etc...
MainView::MainView( const os::Rect &r ):os::View( r, "MainView", os::CF_FOLLOW_ALL, os::WID_FULL_UPDATE_ON_RESIZE )
{
	//create the vertical root structure
	os::LayoutNode *pcRootNode = new os::VLayoutNode( "Root" );
	os::LayoutView *pcView = new os::LayoutView( r, "Layout" );

	//create main menu
	mainMenuBar = new os::Menu( os::Rect( 0, 0, 0, 0 ), "mainMenuBar", os::ITEMS_IN_ROW );
	mainMenuBar->SetFrame( os::Rect( 0, 0, GetBounds().Width() + 1, mainMenuBar->GetPreferredSize( false ).y + 1 ) );

	// App menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Application", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Settings", new os::Message( M_MENU_SETTINGS ) );
	tempMenu->AddItem( new os::MenuSeparator() );
	tempMenu->AddItem( "About", new os::Message( os::M_ABOUT_REQUESTED ) );
	tempMenu->AddItem( "Quit", new os::Message( os::M_QUIT_REQUESTED ) );
	mainMenuBar->AddItem( tempMenu );
	// File menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Server", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Connect", new os::Message( M_MENU_START ) );
	tempMenu->AddItem( "Login", new os::Message( M_MENU_LOGIN ) );
	tempMenu->AddItem( "Disconnect", new os::Message( M_MENU_STOP ) );
	mainMenuBar->AddItem( tempMenu );
	// Help menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Channel", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Join Channel", new os::Message( M_MENU_JOINCHANNEL ) );
	tempMenu->AddItem( "Leave Channel", new os::Message( M_MENU_LEAVECHANNEL ) );
	mainMenuBar->AddItem( tempMenu );
	pcRootNode->AddChild( mainMenuBar );

	//create output box
	m_Text = new os::TextView( os::Rect( 0, 0, 0, 0 ), "output_view", "", os::CF_FOLLOW_ALL );
	m_Text->SetMultiLine( true );
	m_Text->SetReadOnly( true );
	pcRootNode->AddChild( m_Text );
	
	//create input box
	os::LayoutNode *pcButtonNode = new os::HLayoutNode( "Buttons", 0 );
	pcButtonNode->SetBorders( os::Rect( 5, 5, 5, 5 ) );
	m_Input = new os::TextView( os::Rect( 0, 0, 0, 0 ), "input_view", "Input", os::CF_FOLLOW_ALL );
	pcButtonNode->AddChild( m_Input );
	pcButtonNode->AddChild( new os::HLayoutSpacer( "button_spacer", 5, 5 ) );
	m_pcSend = new os::Button( os::Rect( 0, 0, 0, 0 ), "send_button", "Send", new os::Message( MSG_TOLOOPER_NEW_MESSAGE ) );
	m_pcSend->GetFont()->SetSize( 10 );
	pcButtonNode->AddChild( m_pcSend )->LimitMaxSize( m_pcSend->GetPreferredSize( false ) );
	pcRootNode->AddChild( pcButtonNode );

	//add it all to the vertical root structure
	pcView->SetRoot( pcRootNode );
	AddChild( pcView );
}

void MainView::AttachedToWindow()
{
	//ensure messages are sent to the view and not the window
	os::View::AttachedToWindow();

	mainMenuBar->SetTargetForItems( this );
	m_pcSend->SetTarget( this );
	bInChannel = false;

	/* create a protected commthread instance */
	CommThread *m_pCommThread;
	m_pCommThread = new CommThread( this );
	if( m_pCommThread == NULL )
	{
		fprintf( stderr, "Commthread: %s (%d)\n", strerror( errno ), errno );
		os::Application::GetInstance()->PostMessage( os::M_QUIT );
	}
	m_CommThread = m_pCommThread;
	m_CommThread->Run();
}

void MainView::FrameSized( const os::Point& cDelta )
{
	Invalidate( mainMenuBar->GetFrame(), true );

	os::View::FrameSized( cDelta );
}

void MainView::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case M_MENU_START:
		{	
			m_Text->Clear();
			os::Message cMsg( MSG_TOLOOPER_START );
			cMsg.AddString( "host", HOST );
			cMsg.AddString( "port", PORT );
			cMsg.AddString( "channel", CHAN );
			cMsg.AddString( "nick", NICK );
			cMsg.AddString( "user", USER );
			cMsg.AddString( "password", PASS );
			cMsg.AddString( "realname", NAME );
			m_CommThread->PostMessage( &cMsg );
			break;
		}
		case M_MENU_STOP:
		{
			m_CommThread->PostMessage( MSG_TOLOOPER_STOP, m_CommThread );
			break;
		}
		case MSG_TOLOOPER_NEW_MESSAGE:
		{
			SendMsg( m_Input->GetBuffer()[0].const_str() );
			m_Input->Clear();
			break;
		}
		case MSG_FROMLOOPER_NEW_MESSAGE:
		{
			const char *pzName;
			int nCount = 0;
			pcMessage->GetNameInfo( "name", NULL, &nCount );
			for( int i = 0; i < nCount ; ++i )
			{
				if( pcMessage->FindString( "name", &pzName, i ) == 0 )
				{
					Update( os::String( pzName ) );
					break;
				}
			}
			break;
		}
		case M_MENU_LOGIN:
		{
			Login();
			break;
		}
		case M_MENU_JOINCHANNEL:
		{
			if( bInChannel == false )
			{
				Join();
				bInChannel = true;
			}
			break;
		}
		case M_MENU_LEAVECHANNEL:
		{
			if( bInChannel == true )
			{
				bInChannel = false;
				Leave();
			}
			break;
		}
		case M_MENU_SETTINGS:
		case os::M_ABOUT_REQUESTED:
		case os::M_QUIT_REQUESTED:
		{
			os::Application::GetInstance()->PostMessage( pcMessage );
			break;
		}
		default:
		{
			os::View::HandleMessage( pcMessage );
			break;
		}
	}
}

void MainView::Update( const os::String cBufString )
{
	os::String cTempString( cBufString );

	if( bInChannel == true )
	{
		const os::String find1 = "!";
		const std::string::size_type pos1 = cBufString.find( find1, 0 );
		const os::String find2 = "#";
		const std::string::size_type pos2 = cBufString.find( find2, 0 );
		if( pos1 != std::string::npos && pos2 != std::string::npos )
			cTempString = cTempString.erase( pos1, pos2 - pos1 - 1 );
	}

	AddStringToTextView( cTempString );
	m_CommThread->SendReceiveLoop();
}

void MainView::Login( void ) const
{
	os::String cTempString = "PRIVMSG NickServ :IDENTIFY ";
	cTempString += PASS;
	cTempString += "\r\n";
	m_CommThread->Send( cTempString );
}

void MainView::Join( void ) const
{
	os::String cTempString = "JOIN ";
	cTempString += CHAN;
	cTempString += "\r\n";
	m_CommThread->Send( cTempString );
}

void MainView::Leave( void ) const
{
	os::String cTempString = "PART ";
	cTempString += CHAN;
	cTempString += "\r\n";
	m_CommThread->Send( cTempString );
}

void MainView::SendMsg( const os::String cTextFromInput )
{
	os::String cTempString;

	if( bInChannel == true )
	{
		cTempString = "PRIVMSG ";
		cTempString += CHAN;
		cTempString += " :";
		cTempString += cTextFromInput;
		cTempString += "\r\n";
	}
	else
	{
		cTempString = cTextFromInput;
		cTempString += "\r\n";
	}

	m_CommThread->Send( cTempString );
	AddStringToTextView( cTempString );
}

void MainView::AddStringToTextView( const os::String &cName ) const
{
	// add new text at the end of the textview buffer
	const os::TextView::buffer_type cBuffer = m_Text->GetBuffer();
	const int posX = cBuffer[cBuffer.size()-1].size();
	const int posY = cBuffer.size()-1;
	const os::IPoint cPoint = os::IPoint( posX, posY );
	m_Text->Insert( cPoint, cName.c_str() );
}

//----------------------------------------------------------------------------------

MainWindow::MainWindow( const os::Rect &r ):os::Window( r, "MainWindow", "sIRC:- Syllable Internet Relay Chat Client", 0, os::CURRENT_DESKTOP )
{
	/* Set up "view" as the main view for the window, filling up the entire */
	/* window (thus the call to GetBounds()). */
	view = new MainView( GetBounds() );
	AddChild( view );
}

void MainWindow::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case M_MENU_SETTINGS:
		{
			// not yet implemented
			break;
		}
		case os::M_ABOUT_REQUESTED:
		{
			ShowAbout();
			break;
		}
		case os::M_QUIT_REQUESTED:
		{
			OkToQuit();
			break;
		}
		default:
		{
			os::Window::HandleMessage( pcMessage );
			break;
		}
	}
}

void MainWindow::ShowAbout( void )
{
	os::Alert *sAbout = new os::Alert( "About sIRC...",
		"sIRC 0.13\n"
		"Syllable Internet Relay Chat Client\n\n"
		"sIRC 0.03 by James Coxon 2006 jac208@cam.ac.uk\n"
		"sIRC 0.13 by David Kent 2021\n\n"
		"Basically sIRC can connect to a single channel \n"
		"(which has to be changed in the source) \n"
		"and can send and receive messages.  \n\n"
		"sIRC is released under the GNU General Public License.\n"
		"Please see the file COPYING, distributed with sIRC,\n"
		"or http://www.gnu.org for more information.",
		os::Alert::ALERT_INFO, 0x00, "Close", NULL );
	sAbout->CenterInWindow( this );
	sAbout->Go( new os::Invoker() );
}

bool MainWindow::OkToQuit( void )
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return ( true );
}

//----------------------------------------------------------------------------------

MyApplication::MyApplication( void ):os::Application( "application/x-vnd.Syllable-sIRC" )
{
	// Remember it goes "x" then "y"
	myMainWindow = new MainWindow( os::Rect( 20, 20, 600, 400 ) );
	myMainWindow->Show();
	myMainWindow->MakeFocus();
}

void MyApplication::HandleMessage( os::Message * pcMessage )
{
	// bounce message to the window
	myMainWindow->PostMessage( pcMessage, myMainWindow );
	return ( os::Application::HandleMessage( pcMessage ) );
}

//----------------------------------------------------------------------------------

int main( void )
{
	MyApplication *thisApp;
	thisApp = new MyApplication();
	thisApp->Run();

	return ( 0 );
}

