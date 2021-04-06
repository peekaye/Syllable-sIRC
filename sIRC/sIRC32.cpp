
/*****************************************************************************/
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA  */
/*                                                                           */
/*****************************************************************************/
// 	sIRC 0.03 - Syllable Internet Relay Chat Client 
//	Copyright (C) 2006 James Coxon jac208@cam.ac.uk
//	Yeah its GPL :-D
//	Based on various tutorials from Syllable User Bible
//	Thanks to: Spikeb, Brent, Jonas, Vanders, Rick

#include "sIRC.h"
#include "postoffice1.h"

using namespace os;
using namespace std;

//----------------------------------------------------------------------------------

//User Variables
string host = "chat.freenode.net";
string channel = "#syllable";	
string nick = "sIRC_";	
string user = "sIRC_";
string password = "";

//Common Strings/Variables
int sockfd, erase, messagetosend;
char buf[MAXDATASIZE];
string sendmessage;

//----------------------------------------------------------------------------------

CommThread::CommThread( const Messenger& cTarget ):Looper( "comm_worker" )
{
	m_cTarget = cTarget;
	m_eState = S_STOP;

	AddMailbox( "Commthread" );
}

bool CommThread::OkToQuit()
{
	return true;
}

void CommThread::HandleMessage( Message * pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			SendReceiveLoop();
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
			Looper::HandleMessage( pcMessage );
			break;
		}
	}
}

void CommThread::SendReceiveLoop()
{
	if ( messagetosend == 1 )
	{
		SendMessage();
	}
	else if( messagetosend == 0 )
	{
		ReceiveMessage();
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
		perror( "select" );	//error occured in select()
	}
	else if( rv == 0 )
	{
		printf( "(Write) Timeout occured! Blocked after 2.5 seconds. \n" );
		messagetosend = 0;
		Message *msg = new Message( MSG_TOLOOPER_START );
		Mail( "Commthread", msg );
	}
	else
	{
		if( FD_ISSET( sockfd, &writefds ) )
		{
			send( sockfd, sendmessage.c_str(), sendmessage.length(), MSG_DONTWAIT );
			sendmessage = "";
			messagetosend = 0;
			Message *msg = new Message( MSG_TOLOOPER_START );
			Mail( "Commthread", msg );
		}
	}
}

void CommThread::ReceiveMessage()
{
	fd_set readfds;
	struct timeval tv;
	int rv;

	FD_ZERO( &readfds );
	FD_SET( sockfd, &readfds );
	tv.tv_sec = 2;
	tv.tv_usec = 500000;

	rv = select( sockfd + 1, &readfds, NULL, NULL, &tv );

	if( rv == -1 )
	{
		perror( "select" );	//error occured in select()
	}
	else if( rv == 0 )
	{
		printf( "(Read) Timeout occured! No data after 2.5 seconds. \n" );
		Message *msg = new Message( MSG_TOLOOPER_START );
		Mail( "Commthread", msg );
	}
	else
	{
		if( FD_ISSET( sockfd, &readfds ) )
		{
			bzero( buf, MAXDATASIZE );
			recv( sockfd, buf, MAXDATASIZE, 0 );
			Message *msg = new Message( MSG_FROMLOOPER_NEW_MESSAGE );
			Mail( "MainView", msg );
		}
	}
}

//----------------------------------------------------------------------------------

MainView::MainView( const Rect& cRect ):LayoutView( cRect, "MainView" )
//	Here is where we construct the window e.g views, buttons, textviews, etc...
{
	//create the vertical root layout node
	LayoutNode *pcRootNode = new VLayoutNode( "Root" );

	//create output box
	pcRootNode->AddChild( new VLayoutSpacer( "Spacer1", 19, 19 ) );
	textOutputView = new TextView( Rect( 0, 0, 0, 0 ), "OutputView", "", CF_FOLLOW_ALL );
	textOutputView->SetMultiLine( true );
	textOutputView->SetReadOnly( true );
	pcRootNode->AddChild( textOutputView );

	//create input box
	pcRootNode->AddChild( new VLayoutSpacer( "Spacer2", 5, 5 ) );
	LayoutNode *pcButtonNode = new HLayoutNode( "Buttons", 0 );
	pcButtonNode->AddChild( new HLayoutSpacer( "Spacer3", 5, 5 ) );
	textInputView = new TextView( Rect( 0, 0, 0, 0 ), "InputView", "Input", CF_FOLLOW_ALL );
	pcButtonNode->AddChild( textInputView );
	pcButtonNode->AddChild( new HLayoutSpacer( "Spacer4", 5, 5 ) );
	m_pcStart = new Button( Rect( 0, 0, 0, 0 ), "Send", "Send", new Message( MSG_SEND ) );
	m_pcStart->GetFont()->SetSize( 10 );
	pcButtonNode->AddChild( m_pcStart );
	pcButtonNode->AddChild( new HLayoutSpacer( "Spacer5", 5, 5 ) );

	//add the row of buttons to the vertical layout node    
	pcRootNode->AddChild( new VLayoutSpacer( "Spacer6", 5, 5 ) );
	pcRootNode->AddChild( pcButtonNode );
	pcRootNode->AddChild( new VLayoutSpacer( "Spacer7", 5, 5 ) );

	//add it all to this layout view
	SetRoot( pcRootNode );

	//create main menu
	mainMenuBar = new Menu( Rect( 0, 0, 0, 0 ), "mainMenuBar", ITEMS_IN_ROW );
	mainMenuBar->SetFrame( Rect( 0, 0, GetBounds().Width(  ) + 1, 18 ) );

	// App menu 
	tempMenu = new Menu( Rect( 0, 0, 0, 0 ), "Application", ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Settings", new Message( MSG_SETTINGS ) );
	tempMenu->AddItem( "About", new Message( M_MENU_ABO ) );
	tempMenu->AddItem( "Quit", new Message( M_MENU_QUIT ) );
	mainMenuBar->AddItem( tempMenu );
	// File menu 
	tempMenu = new Menu( Rect( 0, 0, 0, 0 ), "Server", ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Connect", new Message( M_MENU_CONNECT ) );
	tempMenu->AddItem( "Login", new Message( M_MENU_LOGIN ) );
	tempMenu->AddItem( "Disconnect", new Message( M_MENU_DISCONNECT ) );
	mainMenuBar->AddItem( tempMenu );
	// Help menu 
	tempMenu = new Menu( Rect( 0, 0, 0, 0 ), "Channel", ITEMS_IN_COLUMN );
	tempMenu->AddItem( "Join Channel", new Message( MSG_JOINCHANNEL ) );
	tempMenu->AddItem( "Send Message", new Message( MSG_SEND ) );
	mainMenuBar->AddItem( tempMenu );
	AddChild( mainMenuBar );

	m_CommThread = new CommThread( this );
	m_CommThread->Run();

	AddMailbox( "MainView" );
}

void MainView::AttachedToWindow()
{
	//ensure messages are sent to the view and not the window
	LayoutView::AttachedToWindow();

	mainMenuBar->SetTargetForItems( this );
	textInputView->SetTarget( this );
	m_pcStart->SetTarget( this );
	erase = 0;
}

void MainView::HandleMessage( Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_MENU_QUIT:
		{
			OkToQuit();
			break;
		}
		case MSG_SEND:
		{
			SendMsg();
			break;
		}
		case MSG_FROMLOOPER_NEW_MESSAGE:
		{
			Update();
			break;
		}
		case M_MENU_ABO:
		{
			ShowAbout();
			break;
		}
		case M_MENU_CONNECT:
		{
			Connect();
			break;
		}
		case M_MENU_LOGIN:
		{
			Login();
			break;
		}
		case MSG_JOINCHANNEL:
		{
			Join();
			break;
		}
		case MSG_SINGLEUPDATE:
		{
			Update();
			break;
		}
		case MSG_UPDATE:
		{
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread );
			break;
		}
		case M_MENU_DISCONNECT:
		{
			Disconnect();
			break;
		}
		case MSG_SETTINGS:
		{
			break;
		}
		default:
		{
			LayoutView::HandleMessage( pcMessage );
			break;
		}
	}
}

void MainView::Connect( void )
{
	struct hostent *he;
	struct sockaddr_in their_addr;	// connector's address information 

	if( ( he = gethostbyname( host.c_str() ) ) == NULL )	// get the host info 
	{
		perror( "gethostbyname" );
		exit( 1 );
	}

	if( ( sockfd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
	{
		perror( "socket" );
		exit( 1 );
	}

	their_addr.sin_family = AF_INET;	// host byte order 
	their_addr.sin_port = htons( PORT );	// short, network byte order 
	their_addr.sin_addr = *( ( struct in_addr * )he->h_addr );
	memset( &( their_addr.sin_zero ), '\0', 8 );	// zero the rest of the struct 

	if( connect( sockfd, ( struct sockaddr * )&their_addr, sizeof( struct sockaddr ) ) == -1 )
	{
		perror( "connect" );
		exit( 1 );
	}

	sendmessage = "NICK " + nick + " \n";
	send( sockfd, sendmessage.c_str(), sendmessage.length(  ), MSG_DONTWAIT );

	sendmessage = "USER sIRC sIRC_ sIRC__ :sIRC\r\n";
	send( sockfd, sendmessage.c_str(), sendmessage.length(  ), MSG_DONTWAIT );

	Message *msg = new Message( MSG_TOLOOPER_RECEIVE );
	Mail( "Commthread", msg );
}


void MainView::Disconnect( void )
{
	close( sockfd );
}

void MainView::Update( void )
{
	string bufstring;
	int i;

	bufstring = buf;
	i = bufstring.find( "PING" );

	// test for server PING
	if( ( i != -1 ) && ( i < 80 ) )
	{
		cout << "Yes Ping" << endl;
		sendmessage = "PONG " + nick + " \n";
		messagetosend = 1;
	}

	// display server data
	if( erase == true )
	{
		string find1 = "!";
		string::size_type pos1 = bufstring.find( find1, 0 );
		string find2 = "#";
		string::size_type pos2 = bufstring.find( find2, 0 );

		bufstring = bufstring.erase( pos1, pos2 - pos1 );
	}

	textOutputView->Insert( bufstring.c_str(), true );

	if( m_CommThread )
		m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread );
}

void MainView::Login( void )
{
	string identify = "PRIVMSG NickServ :IDENTIFY ";
	sendmessage = identify + password + " \n";
}

void MainView::Join( void )
{
	erase = 1;		
	messagetosend = 1;
	sendmessage = "JOIN " + channel + " \n";

	Message *msg = new Message( MSG_TOLOOPER_START );
	Mail( "Commthread", msg );
}

void MainView::SendMsg( void )
{
	string textfrominput, printinputmessage;

	messagetosend = 1;
	textfrominput = textInputView->GetBuffer()[0].const_str();
	sendmessage = "PRIVMSG " + channel + " :" + textfrominput + " \n";
	printinputmessage = nick + channel + " : " + textfrominput + " \n";

	Message *msg = new Message( MSG_TOLOOPER_START );
	Mail( "Commthread", msg );

	textOutputView->Insert( printinputmessage.c_str(), true );
	textInputView->Clear( true );
}

void MainView::ShowAbout(void)
{
	Alert* sAbout = new Alert("About sIRC...",
		"sIRC 0.04\n"
		"Syllable Internet Relay Chat Client\n\n"
		"By David Kent 2021 'https://github.com/peekaye/'\n"
		"By James Coxon 2006 'jac208@cam.ac.uk'",
		0x00, "Close", NULL);
	sAbout->Go( new Invoker() );
}

bool MainView::OkToQuit( void )
{
  Application::GetInstance()->PostMessage( M_QUIT );
  return( true );
}

//----------------------------------------------------------------------------------

MainWindow::MainWindow( const os::Rect & r ):Window( r, "MainWindow", "sIRC - Internet Relay Chat Client", 0, CURRENT_DESKTOP )
{
	/* Set up "view" as the main view for the window, filling up the entire */
	/* window (thus the call to GetBounds()). */
	view = new MainView( GetBounds() );
	AddChild( view );

	AddMailbox( "MainWindow" );
}

bool MainWindow :: OkToQuit( void )
{
	Application::GetInstance()->PostMessage( M_QUIT );
	return( true );
}

//----------------------------------------------------------------------------------

MyApplication::MyApplication( void ):Application( "application/x-vnd.jamescoxon-sIRC" )
{
	// Remember it goes "x" then "y"
	myMainWindow = new MainWindow( Rect( 20, 20, 600, 400 ) );
	myMainWindow->Show();
	myMainWindow->MakeFocus();
}

//----------------------------------------------------------------------------------

int main( void )
{
	MyApplication *thisApp;
	thisApp = new MyApplication();
	thisApp->Run();

	return( 0 );
}

