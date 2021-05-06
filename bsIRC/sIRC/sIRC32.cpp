//	sIRC30.cpp
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
//	Thanks to:
//	Spikeb
//	Brent
//	Jonas
//	Vanders
//	Rick

//	Apparently its good practice to comment on your code - just a warning you are 
//	going to find a lot of code comments in here! I find its easier to work out
//	how things are going to fit together if I write down what each section is going
//	to do.

#include "sIRC.h"
#include "postoffice1.h"

//User Variables
string stdInputMessage = string("");
string stdOutgoingMessage = string("");
string stdName = string("BurningShadow1");
string stdPassword = string("");
string stdRealName = string("BurningShadow");
string stdServerName = string("chat.freenode.net");
string stdServerPort = string("6667");
string stdServerChannel = string("#syllableirc");

//Common Strings/Variables
string end = " \n";
string colon = " :";
string priv = "PRIVMSG ";	
string join = "JOIN ";			
string identify = priv + "nickserv :IDENTIFY ";


//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------

class CommThread : public os::Looper
{
public:	
	CommThread( const os::Messenger& cTarget );
	virtual void HandleMessage( os::Message* pcMessage );
//	virtual bool Idle();
	virtual bool OkToQuit();
private:

	void ReceiveMessage();
	void SendMessage();	
	void SendReceiveLoop();


	enum state_t { S_START, S_STOP };

	os::Messenger m_cTarget;
	state_t m_eState;
};

//----------------------------------------------------------------------------------

class MainWindow : public LayoutView
{
  public:
    MainWindow( const Rect & r ); /* constructor */
	void AttachedToWindow();
	void HandleMessage(Message* pcMessage);
	bool OkToQuit();
	void ShowAbout(void);
	void Connect(void);
	void SendMsg(void);
	void Join(void);	
	void Update(void);		
	void Login(void);
	void Disconnect(void);
	TextView* m_pcMessageText;
	TextView* m_pcTextView;	
	Menu *mainMenuBar, *tempMenu; 	


	private:
//	TextView* textview;
	CommThread* m_CommThread;	

	protected:
		Button*		m_pcStart;
//		string		line;

};

//----------------------------------------------------------------------------------

CommThread :: CommThread( const Messenger& cTarget ): Looper( "comm_worker" )
{
	m_cTarget = cTarget;
	m_eState = S_STOP;
	AddMailbox("Commthread");
}

bool CommThread :: OkToQuit()
{
	return true;
}

void CommThread :: HandleMessage( Message * pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case MSG_TOLOOPER_START:
		{
			SendReceiveLoop();			
		
		break;
		}
		case MSG_TOLOOPER_RECEIVE:
		{
			ReceiveMessage();			
		
		break;
		}
		case MSG_TOLOOPER_SEND:
		{
			SendMessage();			
		
		break;
		}				
		default:
			Looper::HandleMessage( pcMessage );
			break;
	}
}

void CommThread :: SendReceiveLoop()
{
//	SendReceiveLoop is called when the program is started, it basically
//	receives a message from ReceiveMessage() and checks whether we need
//	to send a message. If we do then it calls SendMessage() which once
//	done sends a message back to start the loop again via MSG_TOLOOPER_START.
//	If there isn't a message ReceiveMessage() is called and checks if there
//	is data waiting on the socket (sockfd), if so it "Mails" MainWindow to
//	get the data and do its thing. Once done it then sends a message to 
//	SendReceiveLoop() to start all over again. If there isn't any data to
//	receive then it also messages back to start the loop.

	if (messagetosend == 1){
		SendMessage();
		}
	else if (messagetosend == 0) {
		ReceiveMessage();
		}
}
void CommThread :: SendMessage()
{
//	This is called if there is a message to send, it'll take the global 
//	string sendmessage and send it through the socket. Its important to
//	note that it'll send the string sendmessage exactly as it is (once it
//	has converted it to const char) so all editing such as the adding of 
//	PRIVMSG needs to be done before SendMessage is called.
			fd_set	writefds;
 			struct	timeval tv;
			int n, rv;
			gowrite = 0;
			FD_ZERO(&writefds);
			
			FD_SET(sockfd, &writefds);
			
			n = sockfd + 1;		
			cout << "write n = " << n << endl;
			tv.tv_sec = 2;
			tv.tv_usec = 500000;
			rv = select(n, NULL, &writefds, NULL, &tv);
			cout << "write rv = " << rv << endl;			
			
			if (rv == -1) {
				perror("select"); //error occured in select()
			} else if (rv == 0 ) {
				printf("(Write) Timeout occured! Blocked after 2.5 seconds. \n");
				gowrite = 1; 	

			} else {
				if (FD_ISSET(sockfd, &writefds)) {
					cout << "write rv > 1" << endl;	
					gowrite = -1;				
					cout << "Clear to send" << endl;
			}
			}	
			if (gowrite == -1) {
//	As gowrite == 1 then we convert the string sendmessage into const chars, measure its length
//	and then send it through sockfd.		
				const char* finalmsg;
				finalmsg = stdOutgoingMessage.c_str();
				int finalmessagelen, finalmessagebytes_sent;
				finalmessagelen = stdOutgoingMessage.length();
				finalmessagebytes_sent = send(sockfd, finalmsg, finalmessagelen, MSG_DONTWAIT); 							
				cout << "Sent string sendmessage: " << stdOutgoingMessage << endl;
				cout << "Final Message Length: " << finalmessagelen << endl;
				cout << "Bytes Sent: " << finalmessagebytes_sent << endl;	
				stdOutgoingMessage = "";
				messagetosend = 0;						
				Message *msg1 = new Message(MSG_TOLOOPER_START);
				Mail("Commthread", msg1);					
			}
			else if (gowrite == 1) {
//	If gowrite == -1 then there its not possible to send a message, the sendreceive loop starts again
//	and will attempt to send the message.
			messagetosend = 0;
			Message *msg1 = new Message(MSG_TOLOOPER_START);
			Mail("Commthread", msg1);
			}
}
void CommThread :: ReceiveMessage()
{
// 	This section of code checks sockfd (socket to irc server) for data,
// 	if there is data it sends a message to MainWindow to execute Update (),
//	which after collecting the data and putting it into the textOutputView
//	will restart the SendReceiveLoop().
//	If not it instead sends a message to start the SendReceiveLoop() again.
			fd_set	readfds;
 			struct	timeval tv;
			int n, rv;
			go = 0;
			FD_ZERO(&readfds);
			
			FD_SET(sockfd, &readfds);
			
			n = sockfd +1;
			cout << "n = " << n << endl;
			tv.tv_sec = 2;
			tv.tv_usec = 500000;
			rv = select(n, &readfds, NULL, NULL, &tv);
			cout << "rv = " << rv << endl;			
			
			if (rv == -1) {
				perror("select"); //error occured in select()
			} else if (rv == 0 ) {
				printf("(Read) Timeout occured! No data after 2.5 seconds. \n");
				go = -1;

			} else {
				if (FD_ISSET(sockfd, &readfds)) {
					cout << "rv > 1" << endl;	
					go = 1;				
			}
			}		
//	If "go" is set to -1 there is no data the SendReceiveLoop is started 
//	again, if it is 1 then a message is sent (via Mail) to MainWindow.
			cout << "go = " << go << endl;	
			if (go == 1) {	
				bzero(buf, 50000);		
				numbytes = recv (sockfd, buf,50000, 0);
				printf("buffer = %d\n", numbytes);
//				cout << "Raw buf = " << buf << endl;		
				Message *msg = new Message(MSG_FROMLOOPER_NEW_MESSAGE);
				Mail("MainWindow", msg);			
			} else if (go == -1) {

						Message *msg1 = new Message(MSG_TOLOOPER_START);
						Mail("Commthread", msg1);
			}		

}

//----------------------------------------------------------------------------------

MainWindow::MainWindow(const Rect& cRect) : LayoutView(cRect, "MainWindow")
//	Here is where we construct the window e.g views, buttons, textviews, etc...
{

//create the vertical root layout node
	LayoutNode* pcRootNode = new VLayoutNode("Root");

//create textview
//top spacer
	pcRootNode->AddChild(new VLayoutSpacer("Spacer1", 19, 19));
//create output box
  	Rect y;
	y.Resize(0,0,0,0);
	m_pcTextView = new TextView (y, "OutputView", "", CF_FOLLOW_ALL );
	m_pcTextView ->SetMultiLine(true);
	m_pcTextView ->SetReadOnly(true);
	pcRootNode->AddChild(m_pcTextView);
//spacer between input and output
	pcRootNode->AddChild(new VLayoutSpacer("Spacer2", 5, 5));
//create horizontal layer
	LayoutNode* pcButtonNode = new HLayoutNode("Buttons", 0);
	
	pcButtonNode->AddChild(new HLayoutSpacer("Spacer3", 5, 5));
//create input box
  	Rect x;
	x.Resize( 0,0,0,0);
//	TextView* textInputView = new TextView (x, "InputView", "Input", CF_FOLLOW_ALL );
	m_pcMessageText = new TextView (x, "InputView", "", CF_FOLLOW_ALL );
	pcButtonNode->AddChild(m_pcMessageText);
//bottom spacer
	pcButtonNode->AddChild(new HLayoutSpacer("Spacer4", 5, 5));
	m_pcStart = new Button(Rect(0, 0, 0, 0), "Send", "Send", new Message(MSG_SEND));
	m_pcStart->GetFont()->SetSize(10);
	pcButtonNode->AddChild(m_pcStart);
	pcButtonNode->AddChild(new HLayoutSpacer("Spacer5", 5, 5));
	m_pcStart->GetPreferredSize (false).y;
	
//add the row of buttons to the vertical layout node	
	pcRootNode->AddChild(new VLayoutSpacer("Spacer6", 10, 10));
	pcRootNode->AddChild(pcButtonNode);
	pcRootNode->AddChild(new VLayoutSpacer("Spacer7", 10, 10));

//add it all to this layout view
	SetRoot(pcRootNode);

//	Menu *mainMenuBar, *tempMenu; 
	mainMenuBar = new Menu( Rect(0,0,0,0), "mainMenuBar", ITEMS_IN_ROW ); 
	mainMenuBar ->SetFrame ( Rect (0, 0, GetBounds().Width() + 1, 18) );

// App menu 
	tempMenu = new Menu( Rect(0,0,0,0), "Application", ITEMS_IN_COLUMN ); 
	tempMenu->AddItem( "Settings", new Message(MSG_SETTINGS)); 
	tempMenu->AddItem("About", new Message(M_MENU_ABO)); 
	tempMenu->AddItem("Quit", new Message(M_MENU_QUIT)); 
	mainMenuBar->AddItem( tempMenu ); 
// File menu 
	tempMenu = new Menu( Rect(0,0,0,0), "Server", ITEMS_IN_COLUMN ); 
	tempMenu->AddItem( "Connect", new Message(M_MENU_CONNECT) ); 
	tempMenu->AddItem( "Login", new Message(M_MENU_LOGIN) ); 
	tempMenu->AddItem( "Disconnect", new Message(M_MENU_DISCONNECT) ); 
	mainMenuBar->AddItem( tempMenu ); 
// Help menu 
	tempMenu = new Menu( Rect(0,0,0,0), "Channel", ITEMS_IN_COLUMN ); 
	tempMenu->AddItem( "Join Channel", new Message(MSG_JOINCHANNEL) ); 	
	tempMenu->AddItem( "Send Message", new Message(MSG_SEND) ); 
	mainMenuBar->AddItem( tempMenu ); 
	AddChild( mainMenuBar ); 
//	mainMenuBar->SetTargetForItems( this ); 
	
	m_CommThread = new CommThread( this );
	m_CommThread->Run();
	
	AddMailbox("MainWindow");
  
}


class MainWindoww : public Window
{
  public:
    MainWindoww( const Rect & r ); /* constructor */
    bool OkToQuit();
    
  private:
    MainWindow *view;

	    
};

MainWindoww :: MainWindoww( const Rect & r )
    : Window( r, "MainWindoww", "sIRC", 0, CURRENT_DESKTOP )
{
  /* Set up "view" as the main view for the window, filling up the entire */
  /* window (thus the call to GetBounds()). */
  view = new MainWindow( GetBounds() );
  AddChild( view );
	AddMailbox("MainWindoww");

};


void MainWindow::AttachedToWindow()
{
//ensure messages are sent to the view and not the window
	LayoutView::AttachedToWindow();
	m_pcMessageText->SetTarget(this);
	m_pcStart->SetTarget(this);
	mainMenuBar->SetTargetForItems( this ); 	
}

		
void MainWindow::HandleMessage(Message* pcMessage)
{
	switch(pcMessage->GetCode())
	{
		case M_MENU_QUIT:
			cout << "You pressed Quit Button" << endl;
			OkToQuit();
			break;	
		
		case MSG_SEND:
			{
			cout << "You pressed Send" << endl;					
			SendMsg();
			break;
			}		
		case MSG_FROMLOOPER_NEW_MESSAGE:
			cout << "MSG_FROMLOOPER_NEW_MESSAGE received" << endl;		
			cout << "Starting Update" << endl;					
			Update();				
			cout << "Update Done" << endl;							
			break;				
			
		case M_MENU_ABO:
		    cout << "You pressed About" << endl;
			ShowAbout();
			break;
		
		case M_MENU_CONNECT:
		    cout << "You pressed Connect" << endl;
			Connect();
sleep(5);
/*			break; */
			
		case M_MENU_LOGIN:
		    cout << "You pressed Login" << endl;
			Login();
sleep(5);
/*			break; */
			
		case MSG_JOINCHANNEL:
		    cout << "You pressed Join" << endl;
			Join();
			break;
			
		case MSG_SINGLEUPDATE:
		    cout << "You pressed Single Update" << endl;
			Update();
			break;			
			
		case MSG_UPDATE:
		    cout << "You pressed Update" << endl;
			if( m_CommThread )
				m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread ); 	
			cout << "Message send to MSG_TOLOOPER_START" << endl;	
			break;	
					
		case M_MENU_DISCONNECT:
		    cout << "You pressed Disconnect" << endl;
			Disconnect();
			break;
		case MSG_SETTINGS:
		{
		    cout << "You pressed Settings" << endl;
			break;
		}
					
		default:
			LayoutView::HandleMessage(pcMessage);
			break;
	}
}

bool MainWindow :: OkToQuit( void )
{
  Application::GetInstance()->PostMessage( M_QUIT );
  return( true );
}

bool MainWindoww :: OkToQuit( void )
{
  Application::GetInstance()->PostMessage( M_QUIT );
  return( true );
}


void MainWindow::ShowAbout(void)
{
	Alert* sAbout = new Alert("sIRC 0.03",
		"sIRC 0.01\n\n"
		"Syllable Internet Relay Chat Client\n"
		"By James Coxon 2006\n"
		"jac208@cam.ac.uk"
		,
		0x00, "Close", NULL);
	sAbout->Go(new Invoker);
}

//	Join really does what is says on the tin, messagesend is set to 1 and therefore on the next loop the message
//	will be send. erase is also sent to 1. This tells the program to start editing the buf before sending it to
//	the textOutputView - removes all the junk. Finally it puts together a string, places it in the sendmessage 
//	variable and starts the loop
void MainWindow::Join(void)
{
	messagetosend = 1;	
	erase = 1;		
	stdOutgoingMessage = join + stdServerChannel + end;	
	cout << "Sent JOIN" << endl;	
	Message *msg1 = new Message(MSG_TOLOOPER_START);
	Mail("Commthread", msg1);	
	

}

void MainWindow::Update(void)
{
 			cout << "Updating textOutputView" << endl;	 					
// First we get the infomation taken from the IRC server and put into "buf" is added to the textOutputView
//	We then check the incoming data to see if it contains the PING command from the server.			
			int i;
			string bufstring;
			bufstring = buf;					
			i = bufstring.find("PING");
			cout << "Ping?\n" << endl;
			cout << i << endl;
			cout << bufstring << endl;
			if (i == -1) {
				cout << "No Ping" << endl;
				}
//	If it does we return the message "Pong nick\n", The Ping is sent from the server to check that 
//	we are still receiving messages
//	However due Ping being mentioned in the MOTD from freenode here is a dirty hack to get round this,
//	it only checks if the PING command is not equal to -1 (-1 is returned when Ping isn't mentioned)
//	and less then 80. (80 not 10 as it allows you to test the function by sending a PRIVMSG with Ping
//	embedded.					
			else if (i !=-1) {
				if (i < 80) {
					cout << "Yes Ping" << endl;
					stdOutgoingMessage = "PONG " + stdName + end;		
					messagetosend = 1;
					}
				else {
					cout << "No Ping" << endl;
					}
				}
//	Once the Ping command is done we now need to cut down the message to contain just the nick
//	of the sender and also the message itself.	Now this gets a little stuck with the MOTD from
//	the server so we only turn it on once we have joined a channel. (when erase = 1)
			if (erase ==1){
				string bufstring1;
				string bufstring2;						
				string bufstring3;							
				bufstring1 = buf;
				string find1 = "!";
				string::size_type pos = bufstring1.find (find1,0);
				string find2 = "#";									
				string::size_type pos1 = bufstring1.find (find2,0);
				int pos2, totallength;
				pos2 = pos1 - pos;				
				bufstring2 = bufstring1.erase (pos, pos2);	
				const char* bufchar;					
				bufchar = bufstring2.c_str();				
				os::String cTempTextView = m_pcTextView->GetValue();
				cTempTextView = cTempTextView + bufchar;
				m_pcTextView->Clear();
				m_pcTextView->Set(cTempTextView.c_str(), true);
/*				m_pcTextView->Insert(bufchar, true);		*/
				}
			else {
				os::String cTempTextView = m_pcTextView->GetValue();
				cTempTextView = cTempTextView + buf;
				m_pcTextView->Clear();
				m_pcTextView->Set(cTempTextView.c_str(), true);
/*				m_pcTextView->Insert(buf, true);	*/
				}
			if( m_CommThread )
			m_CommThread->PostMessage( MSG_TOLOOPER_START, m_CommThread, m_CommThread ); 											
		
}

void MainWindow::Login(void)
{
						
			string identify = "PRIVMSG NickServ :IDENTIFY ";
			stdOutgoingMessage = identify + stdPassword + end;
			cout << stdOutgoingMessage << endl;				
			cout << "Sent Privmsg to nickserv" << endl;									
}

void MainWindow::Connect(void)
{
			erase = 0;
			struct hostent *he;
			struct sockaddr_in their_addr; // connector's address information 
/*			argv[1] = "chat.freenode.net"; */
			char cSrvName[99];
			sprintf( cSrvName, "%s", stdServerName.c_str() );
			argv[1] = cSrvName;

			if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info 
	    		perror("gethostbyname");
				printf("get the host info");
			    exit(1);
			}

			if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			    perror("socket");
			    exit(1);
			}

			int nPort;
			nPort = atoi(stdServerPort.c_str());
			their_addr.sin_family = AF_INET;    // host byte order 
			their_addr.sin_port = htons(nPort);  // short, network byte order 
			their_addr.sin_addr = *((struct in_addr *)he->h_addr);
			memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 
	
			if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
			    perror("connect");
				printf("connecting");
			    exit(1);
			}
			int nrv, counter;
			nrv = read(sockfd,buf1,5000);			
			printf(" read: %s\n", buf1);
			string nickmessage;
			nickmessage = "Nick " + stdName + end;
			const char* nickmsg;
			nickmsg = nickmessage.c_str();
			int nickmessagelen, nickmessagebytes_sent;
			nickmessagelen = nickmessage.length();
			nickmessagebytes_sent = send(sockfd, nickmsg, nickmessagelen, MSG_DONTWAIT); 				
//			send(sockfd, "NICK jcoxon_\n", 13, 0);
			cout << "Sent NICK" << endl;	
			send(sockfd, "USER sIRC sIRC_ sIRC__ :sIRC\n", 29, 0);
			cout << "Sent USER" << endl;					
			go = 1;
			Message *msg1 = new Message(MSG_TOLOOPER_RECEIVE);
			Mail("Commthread", msg1);				
					
		
}

void MainWindow::SendMsg (void)
{
	stdInputMessage = m_pcMessageText->GetBuffer()[0].const_str();
	cout << "Text from input: " << stdInputMessage << endl;
	messagetosend = 1;		
	stdOutgoingMessage = priv + stdServerChannel + " :" + stdInputMessage + end;	
	Message *msg1 = new Message(MSG_TOLOOPER_START);
	Mail("Commthread", msg1);	
	string printinputmessage = stdName + stdServerChannel + " : " + stdInputMessage + end;	
	cout << printinputmessage << endl;	
	const char* msgupdate;
	msgupdate = printinputmessage.c_str();	
	os::String cTempTextView = m_pcTextView->GetValue();
	cTempTextView = cTempTextView + msgupdate;
	m_pcTextView->Clear();
	m_pcTextView->Set(cTempTextView.c_str(), true);
/*	m_pcTextView->Insert(msgupdate, true); */
	m_pcMessageText->Clear(true);	
}

void MainWindow::Disconnect(void)
{
	cout << "Closing Socket" << endl;
	close(sockfd);
	cout << "Socket Closed" << endl;
}

class MyApplication : public Application
{
  public:
    MyApplication( void ); /* constructor */
  private:
    MainWindoww *myMainWindoww;
};

MyApplication :: MyApplication( void )
    : Application( "application/x-vnd.jamescoxon-sIRC" )
{

// Remember it goes "x" then "y"
  myMainWindoww = new MainWindoww( Rect(20,20,600,400) );
  myMainWindoww->Show();
  myMainWindoww->MakeFocus();
}


int main( void )
{
  cout << "sIRC 0.03" << endl;
  cout << "Syllable Internet Relay Chat Client" << endl;
  cout << "By James Coxon 2006" << endl;
  cout << "jac208@cam.ac.uk" << endl;
  MyApplication *thisApp;
  thisApp = new MyApplication();
  thisApp->Run();
  cout << "Quitting" << endl;
  return( 0 );
}
