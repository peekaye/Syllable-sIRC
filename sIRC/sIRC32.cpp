
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
#include "postoffice.h"
#include "messages.h"

//User Variables
std::string HOST = "chat.freenode.net";
std::string CHAN = "#syllable";
std::string NICK = "sIRC_";
std::string USER = "sIRC_";
std::string PASS = "password";
std::string NAME = "Syllable User";

//Common Strings/Variables
std::string sendmessage = "";

//----------------------------------------------------------------------------------

CommThread::CommThread( const os::Messenger &cTarget ):os::Looper( "comm_worker" )
{
	m_cTarget = cTarget;
	m_eState = S_STOP;
	AddMailbox( "Commthread" );
}

bool CommThread::OkToQuit()
{
	return true;
}

void CommThread::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			if( m_eState == S_STOP )
			{
				m_eState = S_START;
				Connect();
			}
			break;
		}
		case MSG_TOLOOPER_STOP:
		{
			if( m_eState == S_START )
			{
				Disconnect();
				m_eState = S_STOP;
			}
			break;
		}
		case MSG_TOLOOPER_SEND:
		{
			SendMessage();
			break;
		}
		case MSG_TOLOOPER_RECEIVE:
		{
			ReceiveMessage();
			break;
		}
		default:
		{
			os::Looper::HandleMessage( pcMessage );
			break;
		}
	}
}

void CommThread::SendReceiveLoop( bool messagetosend )
{
	if( messagetosend == true )
	{
		os::Message *msg = new os::Message( MSG_TOLOOPER_SEND );
		Mail( "Commthread", msg );
	}
	else if( messagetosend == false )
	{
		os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
		Mail( "Commthread", msg );
	}
}

void CommThread::SendMessage()
{
	fd_set writefds;
	struct timeval tv;
	int rv;

	FD_ZERO( &writefds );
	FD_SET( sockfd, &writefds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	rv = select( sockfd + 1, NULL, &writefds, NULL, &tv );

	if( rv == -1 )
	{
		perror( "(Write) select" );
	}
	else if( rv == 0 )
	{
		printf( "(Write) Timeout occured! Blocked after 2.5 seconds. \n" );
		os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
		Mail( "Commthread", msg );
	}
	else
	{
		if( FD_ISSET( sockfd, &writefds ) )
		{
			send( sockfd, sendmessage.c_str(), sendmessage.length(), MSG_DONTWAIT );
			sendmessage = "";
			os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
			Mail( "Commthread", msg );
		}
	}
}

void CommThread::ReceiveMessage()
{
	fd_set readfds;
	struct timeval tv;
	int numbytes, rv;
	char buf[MAXDATASIZE];

	FD_ZERO( &readfds );
	FD_SET( sockfd, &readfds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	rv = select( sockfd + 1, &readfds, NULL, NULL, &tv );

	if( rv == -1 )
	{
		perror( "(Read) select" );
	}
	else if( rv == 0 )
	{
		printf( "(Read) Timeout occured! No data after 2.5 seconds. \n" );
		os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
		Mail( "Commthread", msg );
	}
	else
	{
		if( FD_ISSET( sockfd, &readfds ) )
		{
			bzero( buf, MAXDATASIZE );
			if( ( numbytes = recv( sockfd, buf, MAXDATASIZE, 0 ) ) < 1 )
			{
				if( numbytes == -1 )
					perror( "recv" );
				return;
			}
			buf[numbytes] = '\0';
			SendMessage( buf );
		}
	}
}

void CommThread::Connect( void )
{
	struct hostent *he;
	struct sockaddr_in their_addr;	// connector's address information 

	if( ( he = gethostbyname( HOST.c_str() ) ) == NULL )	// get the host info 
	{
		herror( "gethostbyname" );
		return;
	}

	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		perror( "socket" );
		return;
	}

	their_addr.sin_family = AF_INET;	// host byte order 
	their_addr.sin_port = htons( PORT );	// short, network byte order 
	their_addr.sin_addr = *( ( struct in_addr * )he->h_addr );
	memset( &( their_addr.sin_zero ), '\0', 8 );	// zero the rest of the struct 

	if( connect( sockfd, ( struct sockaddr * )&their_addr, sizeof( struct sockaddr ) ) == -1 )
	{
		perror( "connect" );
		return;
	}

	sendmessage = "NICK " + NICK + "\r\n";
	send( sockfd, sendmessage.c_str(), sendmessage.length(), MSG_DONTWAIT );

	sendmessage = "USER " + USER + " 0 * :" + NAME + "\r\n";
	send( sockfd, sendmessage.c_str(), sendmessage.length(), MSG_DONTWAIT );

	os::Message *msg = new os::Message( MSG_TOLOOPER_RECEIVE );
	Mail( "Commthread", msg );
}

void CommThread::Disconnect( void )
{
	close( sockfd );
}

void CommThread::SendMessage( const os::String& cName )
{
	try
	{
		os::Message cMsg( MSG_FROMLOOPER_NEW_MESSAGE );
		cMsg.AddString( "name", cName );

		m_cTarget.SendMessage( &cMsg );
	}
	catch( ... ) { }
}

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
	tempMenu->AddItem( "About", new os::Message( M_MENU_ABOUT ) );
	tempMenu->AddItem( "Quit", new os::Message( M_MENU_QUIT ) );
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
	pcRootNode->AddChild( new os::VLayoutSpacer( "Spacer2", 5, 5 ) );
	os::LayoutNode *pcButtonNode = new os::HLayoutNode( "Buttons", 0 );
	pcButtonNode->AddChild( new os::HLayoutSpacer( "Spacer3", 5, 5 ) );
	m_Input = new os::TextView( os::Rect( 0, 0, 0, 0 ), "input_view", "Input", os::CF_FOLLOW_ALL );
	pcButtonNode->AddChild( m_Input );
	pcButtonNode->AddChild( new os::HLayoutSpacer( "Spacer4", 5, 5 ) );
	m_pcSend = new os::Button( os::Rect( 0, 0, 0, 0 ), "send_button", "Send", new os::Message( MSG_TOLOOPER_NEW_MESSAGE ) );
	m_pcSend->GetFont()->SetSize( 10 );
	pcButtonNode->AddChild( m_pcSend )->LimitMaxSize( m_pcSend->GetPreferredSize( false ) );

	//add the row of buttons to the vertical layout node    
	pcButtonNode->AddChild( new os::HLayoutSpacer( "Spacer5", 5, 5 ) );
	pcRootNode->AddChild( pcButtonNode );
	pcRootNode->AddChild( new os::VLayoutSpacer( "Spacer7", 5, 5 ) );

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

	m_CommThread = new CommThread( this );
	m_CommThread->Run();
}

void MainView::HandleMessage( os::Message *pcMessage )
{
	switch ( pcMessage->GetCode() )
	{
		case M_MENU_START:
		{	
			m_Text->Clear();
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread );
			break;
		}
		case M_MENU_STOP:
		{
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_STOP, m_CommThread, m_CommThread );
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
			Join();
			break;
		}
		case M_MENU_LEAVECHANNEL:
		{
			Leave();
			break;
		}
		case M_MENU_SETTINGS:
		{
			// not yet implemented
			break;
		}
		case M_MENU_ABOUT:
		{
			ShowAbout();
			break;
		}
		case M_MENU_QUIT:
		{
			OkToQuit();
			break;
		}
		default:
		{
			os::View::HandleMessage( pcMessage );
			break;
		}
	}
}

void MainView::Update( os::String name )
{
	bool messagetosend = false;
	std::string bufstring( name );

	// test for ping command
	if( bufstring[0] != ':' )
	{
		std::string::size_type pos = bufstring.find( "PING", 0 );
		if( pos != std::string::npos )
		{
			std::cout << "Ping? Pong!" << std::endl;
			sendmessage = "PONG :" + NICK + "\r\n";
			messagetosend = true;
		}
	}

	// display server data
	if( bInChannel == true )
	{
		std::string find1 = "!";
		std::string::size_type pos1 = bufstring.find( find1, 0 );
		std::string find2 = "#";
		std::string::size_type pos2 = bufstring.find( find2, 0 );
		if( pos1 != std::string::npos && pos2 != std::string::npos )
			bufstring = bufstring.erase( pos1, pos2 - pos1 - 1 );
	}

	AddStringToTextView( bufstring );

	if( m_CommThread )
		m_CommThread->SendReceiveLoop( messagetosend );
}

void MainView::Login( void )
{
	sendmessage = "PRIVMSG NickServ :IDENTIFY " + PASS + "\r\n";
	os::Message *msg = new os::Message( MSG_TOLOOPER_SEND );
	Mail( "Commthread", msg );
}

void MainView::Join( void )
{
	bInChannel = true;
	sendmessage = "JOIN " + CHAN + "\r\n";
	os::Message *msg = new os::Message( MSG_TOLOOPER_SEND );
	Mail( "Commthread", msg );
}

void MainView::Leave( void )
{
	bInChannel = false;
	sendmessage = "PART " + CHAN + "\r\n";
	os::Message *msg = new os::Message( MSG_TOLOOPER_SEND );
	Mail( "Commthread", msg );
}

void MainView::SendMsg( std::string textfrominput )
{
	sendmessage = "PRIVMSG " + CHAN + " :" + textfrominput + "\r\n";
	textfrominput = NICK + " " + CHAN + " :" + textfrominput + "\r\n";

	os::Message *msg = new os::Message( MSG_TOLOOPER_SEND );
	Mail( "Commthread", msg );

	AddStringToTextView( textfrominput );
}

/* add new text at the end of the os::TextView buffer */
void MainView::AddStringToTextView( const std::string &cName ) const
{
	const os::TextView::buffer_type cBuffer = m_Text->GetBuffer();
	const int posX = cBuffer[cBuffer.size()-1].size();
	const int posY = cBuffer.size()-1;
	const os::IPoint cPoint = os::IPoint( posX, posY );
	m_Text->Insert( cPoint, cName.c_str() );
}

void MainView::ShowAbout( void )
{
	os::Alert *sAbout = new os::Alert( "About sIRC...",
		"sIRC 0.09\n"
		"Syllable Internet Relay Chat Client\n\n"
		"sIRC 0.03 by James Coxon 2006 jac208@cam.ac.uk\n"
		"sIRC 0.09 by David Kent 2021\n\n"
		"Basically sIRC can connect to a single channel \n"
		"(which has to be changed in the source) \n"
		"and can send and receive messages.  \n\n"
		"sIRC is released under the GNU General Public License.\n"
		"Please see the file COPYING, distributed with sIRC,\n"
		"or http://www.gnu.org for more information.",
		os::Alert::ALERT_INFO, 0x00, "Close", NULL );
	sAbout->CenterInWindow( GetWindow() );
	sAbout->Go( new os::Invoker() );
}

bool MainView::OkToQuit( void )
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return ( true );
}

//----------------------------------------------------------------------------------

MainWindow::MainWindow( const os::Rect &r ):os::Window( r, "MainWindow", "sIRC:- Syllable Internet Relay Chat Client", 0, os::CURRENT_DESKTOP )
{
	/* Set up "view" as the main view for the window, filling up the entire */
	/* window (thus the call to GetBounds()). */
	view = new MainView( GetBounds() );
	AddChild( view );
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

//----------------------------------------------------------------------------------

int main( void )
{
	MyApplication *thisApp;
	thisApp = new MyApplication();
	thisApp->Run();

	return 0;
}
