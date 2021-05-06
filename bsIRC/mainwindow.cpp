#include "mainwindow.h"
#include "messages.h"
#include <unistd.h>

std::string stdOutgoingMessage;
//James Coxon jac208@cam.ac.uk
std::string end = " \n";
std::string colon = " :";
std::string priv = "PRIVMSG ";
std::string join = "JOIN ";
std::string identify = priv + "nickserv :IDENTIFY ";
int argc;
char *argv[4096];
int sockfd, go, gowrite, messagetosend;
int numbytes;
char buf[50000];
char buf1[5000];
int a = 0;
int erase = 0;




MainWindow::MainWindow() : os::Window( os::Rect( 0, 0, 750, 500 ), "main_wnd", "bsIRC" )
{
	os::LayoutView* pcView = new os::LayoutView( GetBounds(), "layout_view" );
	#include "mainwindowLayout.cpp"
	pcView->SetRoot( m_pcRoot );
	AddChild( pcView );

	LoadBookmarkList();
	AddMailbox( "MainWindow" );
	m_CommThread = new CommThread( this );
	m_CommThread->Run();
}


//James Coxon jac208@cam.ac.uk
void CommThread :: HandleMessage( Message * pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_TOLOOPER_START:
		{
			SendReceiveLoop();
			break;
		}

		case M_TOLOOPER_RECEIVE:
		{
			ReceiveMessage();
			break;
		}

		case M_TOLOOPER_SEND:
		{
			SendMessage();
			break;
		}

		default:
			Looper::HandleMessage( pcMessage );
			break;
	}
}

void MainWindow::HandleMessage( os::Message* pcMessage )
{
	switch( pcMessage->GetCode() )
	{
		case M_BUTTON_ADDBOOKMARK:
		{
			cName = m_pcTextUserName->GetValue();
			cPassword = m_pcTextUserPassword->GetValue();
			cRealName = m_pcTextUserRealName->GetValue();
			cServerName = m_pcTextServerName->GetValue();
			cServerPort = m_pcTextServerPort->GetValue();
			cServerChannel = m_pcTextServerChannel->GetValue();
			cBookmarkName = m_pcTextBookmarkName->GetValue();

			if( cBookmarkName != "" )
			{
				os::String zPath = cBookmarkPath;

				try
				{
					mkdir( zPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
					zPath += os::String("/") + cBookmarkName;
					os::File cFile( zPath, O_RDWR | O_CREAT );	
					os::Settings* pcSettings = new os::Settings(new File(zPath));

					pcSettings->SetString( "Name", cName,0);
					pcSettings->SetString( "Password", cPassword,0);
					pcSettings->SetString( "RealName", cRealName,0);
					pcSettings->SetString( "ServerName", cServerName,0);
					pcSettings->SetString( "ServerPort", cServerPort,0);
					pcSettings->SetString( "ServerChannel", cServerChannel,0);

					/* Make sure there's only one bookmark with the same name, on the list */
					int nCount = m_pcBookmarkList->GetRowCount();
					bool bSameName = false;
					int counter;
					for( counter = 0 ; counter <= nCount - 1 ; counter = counter + 1)
					{
						ListViewStringRow* pcRow = static_cast<os::ListViewStringRow*>( m_pcBookmarkList->GetRow( counter ) );
						os::String cTempBookmarkName = pcRow->GetString(0);
						if( cBookmarkName == cTempBookmarkName )
							bSameName = true;
					}
					if( bSameName == false )
					{
						ListViewStringRow *row = new ListViewStringRow();
						row->AppendString( cBookmarkName );
						m_pcBookmarkList->InsertRow( row );
					}

					pcSettings->Save();
					delete( pcSettings );
				}
				catch(...)
				{
				}
			}
			break;
		}


		case M_BUTTON_REMOVEBOOKMARK:
		{
			int nSelected = -1;
			nSelected = m_pcBookmarkList->GetLastSelected();
			ListViewStringRow* pcRow = static_cast<os::ListViewStringRow*>( m_pcBookmarkList->GetRow( nSelected ) );
			os::String cBookmarkName = pcRow->GetString(0);
			if( m_pcBookmarkList->GetRowCount() > 0 && nSelected != -1 )
			{
				m_pcBookmarkList->RemoveRow( nSelected );
			}
			os::String zExec = os::String( "rm -f " ) + cBookmarkPath + os::String( "/" ) + cBookmarkName;
			system( zExec.c_str() );

			break;
		}


		case M_BUTTON_BCONNECT:
		{
			int nSelected = m_pcBookmarkList->GetLastSelected();
			ListViewStringRow* pcRow = static_cast<os::ListViewStringRow*>( m_pcBookmarkList->GetRow( nSelected ) );
			os::String cBookmarkName = pcRow->GetString(0);

			if( cBookmarkName != ""  )
			{
				os::String zPath = cBookmarkPath + os::String( "/" ) + cBookmarkName;

				try
				{
					os::Settings* pcSettings = new os::Settings(new File(zPath));
					pcSettings->Load();

					os::String cName = pcSettings->GetString("Name","",0);
					os::String cPassword = pcSettings->GetString("Password","",0);
					os::String cRealName = pcSettings->GetString("RealName","",0);
					os::String cServerName = pcSettings->GetString("ServerName","",0);
					os::String cServerPort = pcSettings->GetString("ServerPort","",0);
					os::String cServerChannel = pcSettings->GetString("ServerChannel","",0);

					m_pcTextUserName->Set( cName.c_str() );
					m_pcTextUserPassword->Set( cPassword.c_str() );
					m_pcTextUserRealName->Set( cRealName.c_str() );
					m_pcTextServerName->Set( cServerName.c_str() );
					m_pcTextServerPort->Set( cServerPort.c_str() );
					m_pcTextServerChannel->Set( cServerChannel.c_str() );
					m_pcTextBookmarkName->Set( cBookmarkName.c_str() );

					delete( pcSettings );
				}
				catch(...)
				{
				}
			}
			/* and fall to CCONNECT */
		}


		case M_BUTTON_CCONNECT:
		{
			stdName = m_pcTextUserName->GetBuffer()[0].const_str();
			stdPassword = m_pcTextUserPassword->GetBuffer()[0].const_str();
			stdRealName = m_pcTextUserRealName->GetBuffer()[0].const_str();
			stdServerName = m_pcTextServerName->GetBuffer()[0].const_str();
			stdServerPort = m_pcTextServerPort->GetBuffer()[0].const_str();
			stdServerChannel = m_pcTextServerChannel->GetBuffer()[0].const_str();
			if( stdRealName == "" )
				stdRealName = stdName;

			/* Connect - Login - Join */
			//James Coxon jac208@cam.ac.uk
			cout << "You pressed Connect" << endl;
			Connect();
/*
			cout << "You pressed Login" << endl;
			Login();

			cout << "You pressed Join" << endl;
			Join();
*/
			break;
		}


		case M_BUTTON_DISCONNECT:
		{
			//James Coxon jac208@cam.ac.uk
			cout << "You pressed Disconnect" << endl;
			Disconnect();
			break;
		}


		case M_BUTTON_SEND:
		{
			cMessage = m_pcMessageText->GetValue();
			if( cMessage != "" )
			{
				//James Coxon jac208@cam.ac.uk
				cout << "You pressed Send" << endl;
				SendMsg();
			}
			break;
		}


		case M_BUTTON_CLEAR:
		{
			m_pcMessageText->Clear();
			break;
		}


		case M_MENU_HELP:
		{
			if( fork() == 0 )
			{
				set_thread_priority( -1, 0 );
				execlp( "/Applications/ABrowse/ABrowse","/Applications/ABrowse/ABrowse","http://www.syllable-software.info/index.php?id=bsIRC",NULL );
			}
			break;
		}


		case M_MENU_WEB:
		{
			if( fork() == 0 )
			{
				set_thread_priority( -1, 0 );
				execlp( "/Applications/ABrowse/ABrowse","/Applications/ABrowse/ABrowse","http://www.syllable-software.info/index.php?id=bsIRC",NULL );
			}
			break;
		}


		case M_MENU_ABOUT:
		{
			Alert* sAbout = new Alert("bsIRC 0.02",
				"bsIRC is based on sIRC 0.03\n\n"
				"bsIRC By BurningShadow - flemming@syllable.org\n"
				"sIRC By James - jac208@cam.ac.uk\n"
				,0x00, "Close", NULL);
			sAbout->Go(new Invoker);
			break;
		}


		//James Coxon jac208@cam.ac.uk
		case M_FROMLOOPER_NEW_MESSAGE:
		{
			cout << "M_FROMLOOPER_NEW_MESSAGE received" << endl;
			cout << "Starting Update" << endl;
			Update();
			cout << "Update Done" << endl;
			break;
		}


		//James Coxon jac208@cam.ac.uk
		case M_SINGLEUPDATE:
		{
			cout << "You pressed Single Update" << endl;
			Update();
			break;
		}


		//James Coxon jac208@cam.ac.uk
		case M_UPDATE:
		{
			cout << "You pressed Update" << endl;
			if( m_CommThread )
				m_CommThread->PostMessage( M_TOLOOPER_START, m_CommThread, m_CommThread );
			cout << "Message send to M_TOLOOPER_START" << endl;
			break;
		}		

/*
		//James Coxon jac208@cam.ac.uk
		default:
		{
			LayoutView::HandleMessage(pcMessage);
			break;
		}
*/

		case M_MENU_QUIT:
		case M_APP_QUIT:
		{
			OkToQuit();
		}

		break;
	}
}


//James Coxon jac208@cam.ac.uk
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

	if (messagetosend == 1)
	{
		SendMessage();
	}
	else if (messagetosend == 0)
	{
		ReceiveMessage();
	}
}


//James Coxon jac208@cam.ac.uk
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

	if (rv == -1)
	{
		perror("select"); //error occured in select()
	}
	else if (rv == 0 )
	{
		printf("(Write) Timeout occured! Blocked after 2.5 seconds. \n");
		gowrite = 1;
	}
	else
	{
		if (FD_ISSET(sockfd, &writefds))
		{
			cout << "write rv > 1" << endl;
			gowrite = -1;
			cout << "Clear to send" << endl;
		}
	}

	if (gowrite == -1)
	{
		//	As gowrite == 1 then we convert the string sendmessage into const chars,
		//	measure its length and then send it through sockfd.
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
		Message *msg1 = new Message(M_TOLOOPER_START);
		Mail("Commthread", msg1);
	}
	else if (gowrite == 1)
	{
	//	If gowrite == -1 then there its not possible to send a message, the
	//	sendreceive loop starts again and will attempt to send the message.
		messagetosend = 0;
		Message *msg1 = new Message(M_TOLOOPER_START);
		Mail("Commthread", msg1);
	}
}


//James Coxon jac208@cam.ac.uk
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
				Message *msg = new Message(M_FROMLOOPER_NEW_MESSAGE);
				Mail("MainWindow", msg);			
			} else if (go == -1) {

						Message *msg1 = new Message(M_TOLOOPER_START);
						Mail("Commthread", msg1);
			}		

}


//James Coxon jac208@cam.ac.uk
void MainWindow::Connect(void)
{
	erase = 0;
	struct hostent *he;
	struct sockaddr_in their_addr;	// connector's address information

	char cSrvName[99];
	sprintf( cSrvName, "%s", stdServerName.c_str() );
	argv[1] = cSrvName;

	if ((he=gethostbyname(argv[1])) == NULL)	// get the host info
	{
		perror("gethostbyname");
		printf("get the host info");
		exit(1);
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket");
		exit(1);
	}

	int nPort;
	nPort = atoi( stdServerPort.c_str() );
	their_addr.sin_family = AF_INET;	// host byte order
	their_addr.sin_port = htons(nPort);	// short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8);	// zero the rest of the struct

	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("connect");
		printf("connecting");
		exit(1);
	}
	printf(" read: %s\n", buf1);
	string nickmessage;
	nickmessage = "Nick " + stdName + end;
	const char* nickmsg;
	nickmsg = nickmessage.c_str();
	int nickmessagelen, nickmessagebytes_sent;
	nickmessagelen = nickmessage.length();
	nickmessagebytes_sent = send(sockfd, nickmsg, nickmessagelen, MSG_DONTWAIT);
//	send(sockfd, "NICK jcoxon_\n", 13, 0);
	cout << "Sent NICK" << endl;
	send(sockfd, "USER sIRC sIRC_ sIRC__ :sIRC\n", 29, 0);
	cout << "Sent USER" << endl;
	go = 1;
	Message *msg1 = new Message(M_TOLOOPER_RECEIVE);
	Mail("Commthread", msg1);
}


//James Coxon jac208@cam.ac.uk
void MainWindow::Disconnect(void)
{
	cout << "Closing Socket" << endl;
	close(sockfd);
	cout << "Socket Closed" << endl;
}


//James Coxon jac208@cam.ac.uk
void MainWindow::Login(void)
{
	string identify = "PRIVMSG NickServ :IDENTIFY ";
	stdOutgoingMessage = identify + stdPassword + end;
	cout << stdOutgoingMessage << endl;
	cout << "Sent Privmsg to nickserv" << endl;
}


//James Coxon jac208@cam.ac.uk
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
	Message *msg1 = new Message( M_TOLOOPER_START );
	Mail("Commthread", msg1);
}


//James Coxon jac208@cam.ac.uk
void MainWindow::SendMsg (void)
{
	std::string stdNameTemp = stdName;
	stdInputMessage = m_pcMessageText->GetBuffer()[0].c_str();
	cout << "Text from input: " << stdInputMessage << endl;
	messagetosend = 1;
	stdOutgoingMessage = priv + stdServerChannel + " :" + stdInputMessage + end;
	Message *msg1 = new Message(M_TOLOOPER_START);
	Mail("Commthread", msg1);
	string printinputmessage = stdNameTemp + stdServerChannel + " : " + stdInputMessage + end;
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


void MainWindow::Update(void)
{
	cout << "Updating m_pcTextView" << endl;
//	First we get the infomation taken from the IRC server and put into "buf" is added to the textOutputView
//	We then check the incoming data to see if it contains the PING command from the server.
	int i;
	string bufstring;
	bufstring = buf;
	i = bufstring.find("PING");
	cout << "Ping?\n" << endl;
	cout << i << endl;
	cout << bufstring << endl;
	if (i == -1)
	{
		cout << "No Ping" << endl;
	}

//	If it does we return the message "Pong nick\n", The Ping is sent from the server to check that
//	we are still receiving messages
//	However due Ping being mentioned in the MOTD from freenode here is a dirty hack to get round this,
//	it only checks if the PING command is not equal to -1 (-1 is returned when Ping isn't mentioned)
//	and less then 80. (80 not 10 as it allows you to test the function by sending a PRIVMSG with Ping
//	embedded.
	else if (i !=-1)
	{
		if (i < 80)
		{
			std::string stdNameTemp = stdName;
			cout << "Yes Ping" << endl;
			stdOutgoingMessage = "PONG " + stdNameTemp + end;
			messagetosend = 1;
		}
		else
		{
			cout << "No Ping" << endl;
		}
	}

//	Once the Ping command is done we now need to cut down the message to contain just the nick
//	of the sender and also the message itself.	Now this gets a little stuck with the MOTD from
//	the server so we only turn it on once we have joined a channel. (when erase = 1)
	if (erase ==1)
	{
		string bufstring1;
		string bufstring2;
		string bufstring3;
		bufstring1 = buf;
		string find1 = "!";
		string::size_type pos = bufstring1.find (find1,0);
		string find2 = "#";
		string::size_type pos1 = bufstring1.find (find2,0);
		int pos2;
		pos2 = pos1 - pos;
		bufstring2 = bufstring1.erase (pos, pos2);
		const char* bufchar;
		bufchar = bufstring2.c_str();
		os::String cTempTextView = m_pcTextView->GetValue();
		cTempTextView = cTempTextView + bufchar;
		m_pcTextView->Clear();
		m_pcTextView->Set(cTempTextView.c_str(), true);
/*		m_pcTextView->Insert(bufchar, true); */
	}
	else
	{
		os::String cTempTextView = m_pcTextView->GetValue();
		cTempTextView = cTempTextView + buf;
		m_pcTextView->Clear();
		m_pcTextView->Set(cTempTextView.c_str(), true);
/*		m_pcTextView->Insert(buf, true); */
	}

	if( m_CommThread )
{
		m_CommThread->PostMessage( M_TOLOOPER_START, m_CommThread, m_CommThread );
}
}


void MainWindow::LoadBookmarkList()
{
	/* Set Bookmark-dir. */
	cBookmarkPath = getenv( "HOME" );
	cBookmarkPath += os::String( "/IRC-Bookmarks" );

	/* Open up keymaps directory and check it actually contains something */
	DIR *pDir = opendir( cBookmarkPath.c_str() );
	if (pDir == NULL)
	{
		return;
	}

	/* Clear listview */
	m_pcBookmarkList->Clear();

	/* Create a directory entry object */
	dirent *psEntry;

	/* Loop through directory and add each keymap to list */
	int i = 0;
	while ( (psEntry = readdir( pDir )) != NULL)
	{
		/* If special directory (i.e. dot(.) and dotdot(..) then ignore */
		if (strcmp( psEntry->d_name, ".") == 0 || strcmp( psEntry->d_name, ".." ) == 0)
		{
			continue;
		}

		/* Its valid, add to the listview */
		os::ListViewStringRow* pcRow = new os::ListViewStringRow();
		pcRow->AppendString( psEntry->d_name );
		m_pcBookmarkList->InsertRow( pcRow );

		++i;
	}
	closedir( pDir );
}


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


bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return( true );
}

