
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
#include "chathelp.h"

//User Variables
const os::String HOST = "irc.libera.chat";
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
	os::LayoutView *pcView = new os::LayoutView( r, "Layout", pcRootNode );

	//create main menu
	mainMenuBar = new os::Menu( os::Rect( 0, 0, 0, 0 ), "mainMenuBar", os::ITEMS_IN_ROW );
	mainMenuBar->SetFrame( os::Rect( 0, 0, GetBounds().Width() + 1, mainMenuBar->GetPreferredSize( false ).y + 1 ) );

	// Application menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Application", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Settings", new os::Message( M_MENU_SETTINGS ) );
	tempMenu->AddItem( new os::MenuSeparator() );
	tempMenu->AddItem( "About", new os::Message( os::M_ABOUT_REQUESTED ) );
	tempMenu->AddItem( "Quit", new os::Message( os::M_QUIT_REQUESTED ) );
	mainMenuBar->AddItem( tempMenu );
	// Server menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Server", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Connect", new os::Message( M_MENU_START ) );
	tempMenu->AddItem( "Login", new os::Message( M_MENU_LOGIN ) );
	tempMenu->AddItem( "Disconnect", new os::Message( M_MENU_STOP ) );
	mainMenuBar->AddItem( tempMenu );
	// Channel menu
	tempMenu = new os::Menu( os::Rect( 0, 0, 0, 0 ), "Channel", os::ITEMS_IN_COLUMN );
	tempMenu->AddItem( m_pcJoinMenuItem = new os::MenuItem( "Join Channel", new os::Message( M_MENU_JOINCHANNEL ) ) );
	tempMenu->AddItem( m_pcLeaveMenuItem = new os::MenuItem( "Leave Channel", new os::Message( M_MENU_LEAVECHANNEL ) ) );
	mainMenuBar->AddItem( tempMenu );
	pcRootNode->AddChild( mainMenuBar );

	//create output box
	m_Text = new os::TextView( os::Rect( 0, 0, 0, 0 ), "output_view", "", os::CF_FOLLOW_ALL );
	m_Text->SetMultiLine( true );
	m_Text->SetReadOnly( true );
	pcRootNode->AddChild( m_Text );
	
	//create input box
	os::LayoutNode *pcButtonNode = new os::HLayoutNode( "button_node", 0 );
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

void MainView::AllAttached()
{
	mainMenuBar->SetTargetForItems( this );
	m_pcJoinMenuItem->SetEnable( true );
	m_pcLeaveMenuItem->SetEnable( false );
	m_pcSend->SetTarget( this );
	bInChannel = false;

	os::View::AllAttached();
}

void MainView::AttachedToWindow()
{
	//ensure messages are sent to the view and not the window
	os::View::AttachedToWindow();

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
			if( !m_CommThread->IsConnected() )
			{
				os::Message cMsg( MSG_TOLOOPER_START );
				cMsg.AddString( "host", HOST );
				cMsg.AddString( "port", PORT );
				cMsg.AddString( "channel", CHAN );
				cMsg.AddString( "nick", NICK );
				cMsg.AddString( "user", USER );
				cMsg.AddString( "password", PASS );
				cMsg.AddString( "realname", NAME );
				m_CommThread->PostMessage( &cMsg );
				m_Text->Clear();
			}
			break;
		}
		case M_MENU_STOP:
		{
			if( m_CommThread->IsConnected() )
			{
				m_CommThread->Send( os::String().Format( "QUIT %s\r\n", ":-)" ) );
				sleep( 3 ); // wait for server exit message
				m_CommThread->PostMessage( MSG_TOLOOPER_STOP, m_CommThread );
			}
			break;
		}
		case MSG_TOLOOPER_NEW_MESSAGE:
		{
			if( bInChannel )
				m_CommThread->Send( os::String().Format( "PRIVMSG %s :%s\r\n", CHAN.c_str(), m_Input->GetBuffer()[0].c_str() ) );
			else
				m_CommThread->Send( os::String().Format( "%s\r\n", m_Input->GetBuffer()[0].c_str() ) );
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
			m_CommThread->Send( os::String().Format( "PRIVMSG NickServ :IDENTIFY %s\r\n", PASS.c_str() ) );
			break;
		}
		case M_MENU_JOINCHANNEL:
		{
			if( m_CommThread->IsConnected() )
			{
				if( !bInChannel )
				{
					m_CommThread->Send( os::String().Format( "JOIN %s\r\n", CHAN.c_str() ) );
					m_pcJoinMenuItem->SetEnable( false );
					m_pcLeaveMenuItem->SetEnable( true );
					bInChannel = true;
				}
			}
			break;
		}
		case M_MENU_LEAVECHANNEL:
		{
			if( m_CommThread->IsConnected() )
			{
				if( bInChannel )
				{
					bInChannel = false;
					m_pcJoinMenuItem->SetEnable( true );
					m_pcLeaveMenuItem->SetEnable( false );
					m_CommThread->Send( os::String().Format( "PART %s\r\n", CHAN.c_str() ) );
				}
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

	if( bInChannel )
	{
		const os::String find1 = "!";
		const std::string::size_type pos1 = cBufString.find( find1, 0 );
		const os::String find2 = "#";
		const std::string::size_type pos2 = cBufString.find( find2, 0 );

		// cut between pos1 '!' and pos2 '#'
		if( pos1 != std::string::npos && pos2 != std::string::npos )
			cTempString = cTempString.erase( pos1, pos2 - pos1 - 1 );
	}

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
		"sIRC 0.23\n"
		"Syllable Internet Relay Chat Client\n\n"
		"sIRC 0.03 Copyright © James Coxon 2006 jac208@cam.ac.uk\n"
		"sIRC 0.23 Copyright © David Kent 2021\n\n"
		"sIRC is released under the GNU General Public License.\n"
		"Please see the file COPYING, distributed with sIRC,\n"
		"or http://www.gnu.org for more information.",
		os::Alert::ALERT_INFO, 0x00, "Help", "Close", NULL );
	sAbout->CenterInWindow( this );

	int32 reply = sAbout->Go();
	switch ( reply )
	{
		// the first button 'Help'
		case 0x00: ( new os::Alert( "Help", ChatHelp(), os::Alert::ALERT_TIP, 0x00, "Right", NULL ) )->Go(); break;

		// the second button 'Close'
		case 0x01: break;
	}
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
