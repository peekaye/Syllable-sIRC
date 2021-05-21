
#include "mainwindow.h"
#include "commthread.h"
#include "messages.h"

MainWindow::MainWindow():os::Window( os::Rect( 0, 0, 750, 500 ), "main_wnd", "bsIRC" )
{
	os::LayoutView * pcView = new os::LayoutView( GetBounds(), "layout_view" );

#include "mainwindowLayout.cpp"

	pcView->SetRoot( m_pcRoot );
	AddChild( pcView );

	LoadBookmarkList();

	/* create a protected commthread instance */
	CommThread *m_pCommThread;
	m_pCommThread = new CommThread( this );
	if( m_pCommThread == NULL )
	{
		char buf[1024];
		snprintf( buf, 1024, "Failed to create a commthread instance\nReason: %s", strerror( errno ) );
		(new os::Alert( "Error", buf, 0, "OK", NULL ))->Go();
		os::Application::GetInstance()->PostMessage( os::M_QUIT );
	}
	m_CommThread = m_pCommThread;
}

void MainWindow::HandleMessage( os::Message * pcMessage )
{
	switch ( pcMessage->GetCode() )
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
					zPath += os::String ( "/" ) + cBookmarkName;
					os::File cFile( zPath, O_RDWR | O_CREAT );
					os::Settings * pcSettings = new os::Settings( new os::File( zPath ) );

					pcSettings->SetString( "Name", cName, 0 );
					pcSettings->SetString( "Password", cPassword, 0 );
					pcSettings->SetString( "RealName", cRealName, 0 );
					pcSettings->SetString( "ServerName", cServerName, 0 );
					pcSettings->SetString( "ServerPort", cServerPort, 0 );
					pcSettings->SetString( "ServerChannel", cServerChannel, 0 );

					/* Make sure there's only one bookmark with the same name, on the list */
					int nCount = m_pcBookmarkList->GetRowCount();
					bool bSameName = false;
					int counter;

					for( counter = 0; counter <= nCount - 1; counter = counter + 1 )
					{
						os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( counter ) );
						os::String cTempBookmarkName = pcRow->GetString( 0 );
						if( cBookmarkName == cTempBookmarkName )
							bSameName = true;
					}

					if( bSameName == false )
					{
						os::ListViewStringRow *row = new os::ListViewStringRow();
						row->AppendString( cBookmarkName );
						m_pcBookmarkList->InsertRow( row );
					}

					pcSettings->Save();
					delete( pcSettings );
				}
				catch( ... )
				{
				}
			}
			break;
		}

		case M_BUTTON_REMOVEBOOKMARK:
		{
			int nSelected = m_pcBookmarkList->GetLastSelected();
			if( nSelected == -1 )
				return;

			os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( nSelected ) );
			os::String cBookmarkName = pcRow->GetString( 0 );

			if( m_pcBookmarkList->GetRowCount() > 0 && nSelected != -1 )
			{
				m_pcBookmarkList->RemoveRow( nSelected );
			}

			os::String zExec = os::String ( "rm -f " ) + cBookmarkPath + os::String ( "/" ) + cBookmarkName;
			system( zExec.c_str() );

			break;
		}

		case M_BUTTON_BCONNECT:
		{
			int nSelected = m_pcBookmarkList->GetLastSelected();
			if( nSelected == -1 )
				return;

			os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( nSelected ) );
			os::String cBookmarkName = pcRow->GetString( 0 );

			if( cBookmarkName != "" )
			{
				os::String zPath = cBookmarkPath + os::String ( "/" ) + cBookmarkName;

				try
				{
					os::Settings * pcSettings = new os::Settings( new os::File( zPath ) );
					pcSettings->Load();

					os::String cName = pcSettings->GetString( "Name", "", 0 );
					os::String cPassword = pcSettings->GetString( "Password", "", 0 );
					os::String cRealName = pcSettings->GetString( "RealName", "", 0 );
					os::String cServerName = pcSettings->GetString( "ServerName", "", 0 );
					os::String cServerPort = pcSettings->GetString( "ServerPort", "", 0 );
					os::String cServerChannel = pcSettings->GetString( "ServerChannel", "", 0 );

					m_pcTextUserName->Set( cName.c_str() );
					m_pcTextUserPassword->Set( cPassword.c_str() );
					m_pcTextUserRealName->Set( cRealName.c_str() );
					m_pcTextServerName->Set( cServerName.c_str() );
					m_pcTextServerPort->Set( cServerPort.c_str() );
					m_pcTextServerChannel->Set( cServerChannel.c_str() );
					m_pcTextBookmarkName->Set( cBookmarkName.c_str() );

					delete( pcSettings );
				}
				catch( ... )
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
			if( m_CommThread->GetState() == CommThread::S_START )
				break;

			if ( !stdServerName.empty() && !stdServerPort.empty() )
			{
				if( m_CommThread->Connect( stdServerName, stdServerPort ) )
				{
					// authenticate to host
					m_CommThread->Send( os::String().Format( "USER %s localhost %s :%s", stdName.c_str(), stdServerName.c_str(), stdRealName.c_str() ) );
					m_CommThread->Send( os::String().Format( "PASS %s", stdPassword.c_str() ) );
					m_CommThread->Send( os::String().Format( "NICK %s", stdName.c_str() ) );

					m_pcTextView->Clear();
					m_CommThread->Start();
					m_CommThread->SetState( CommThread::S_START );
				}
			}
			break;
		}

		case M_BUTTON_DISCONNECT:
		{
			if( m_CommThread->GetState() == CommThread::S_START )
			{
				m_CommThread->SetState( CommThread::S_STOP );
				m_CommThread->Send( "QUIT" );
				m_CommThread->Disconnect();
				m_CommThread->Reinitialize();
			}
			break;
		}

		case M_BUTTON_SEND:
		{
			if( m_CommThread->GetState() == CommThread::S_STOP )
				break;

			cMessage = m_pcMessageText->GetValue();
			if( !cMessage.empty() )
			{
				m_CommThread->Send( m_pcMessageText->GetBuffer()[0].const_str() );
				m_pcMessageText->Clear();
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
			if( fork() == EOK )
			{
				set_thread_priority( -1, 0 );
				execlp( "/Applications/ABrowse/ABrowse", "/Applications/ABrowse/ABrowse", "http://www.syllable-software.info/index.php?id=bsIRC", NULL );
			}
			break;
		}

		case M_MENU_WEB:
		{
			if( fork() == EOK )
			{
				set_thread_priority( -1, 0 );
				execlp( "/Applications/ABrowse/ABrowse", "/Applications/ABrowse/ABrowse", "http://www.syllable-software.info/index.php?id=bsIRC", NULL );
			}
			break;
		}

		case M_MENU_ABOUT:
		{
			os::Alert *sAbout = new os::Alert( "About bsIRC...",
				"bsIRC 0.02\n"
				"Syllable Internet Relay Chat Client\n\n"
				"bsIRC By BurningShadow - flemming@syllable.org\n"
				"libnet by Kristian Van Der Vliet - vanders@liqwyd.com\n\n"
				"bsIRC is released under the GNU General Public License.\n"
				"Please see the file COPYING, distributed with bsIRC,\n"
				"or http://www.gnu.org for more information.",
				os::Alert::ALERT_INFO, 0x00, "Close", NULL );
			sAbout->CenterInWindow( this );
			sAbout->Go( new os::Invoker() );
			break;
		}

		case MSG_FROMLOOPER_NEW_MESSAGE:
		{
			const char *pzName;
			int nCount = 0;

			pcMessage->GetNameInfo( "name", NULL, &nCount );

			for( int i = 0; i < nCount ; ++i )
			{
				if( pcMessage->FindString( "name", &pzName, i ) == EOK )
				{
					if( PingPong( os::String( pzName ) ) )
						break;
					AddStringToTextView( os::String( pzName ) );
				}
			}
			break;
		}

		case M_MENU_QUIT:
		case M_APP_QUIT:
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

void MainWindow::LoadBookmarkList()
{
	/* Set Bookmark-dir. */
	cBookmarkPath = getenv( "HOME" );
	cBookmarkPath += os::String ( "/IRC-Bookmarks" );

	/* Open up keymaps directory and check it actually contains something */
	DIR *pDir = opendir( cBookmarkPath.c_str() );
	if( pDir == NULL )
	{
		return;
	}

	/* Clear listview */
	m_pcBookmarkList->Clear();

	/* Create a directory entry object */
	dirent *psEntry;

	/* Loop through directory and add each keymap to list */
	int i = 0;
	while( ( psEntry = readdir( pDir ) ) != NULL )
	{
		/* If special directory (i.e. dot(.) and dotdot(..) then ignore */
		if( strcmp( psEntry->d_name, "." ) == 0 || strcmp( psEntry->d_name, ".." ) == 0 )
		{
			continue;
		}

		/* Its valid, add to the listview */
		os::ListViewStringRow * pcRow = new os::ListViewStringRow();
		pcRow->AppendString( psEntry->d_name );
		m_pcBookmarkList->InsertRow( pcRow );

		++i;
	}
	closedir( pDir );
}

bool MainWindow::PingPong( const os::String &cName ) const
{
	if ( cName.substr( 0, 1 ) != ":" )
	{
		if( cName.substr( 0, 4 ).Compare( "PING" ) == EOK );
		{
			m_CommThread->Send( os::String().Format( "PONG%s", cName.substr( 4 ).c_str() ) );
			return ( true );
		}
	}

	return ( false );
}

/* add new text to the end of the text view buffer */
void MainWindow::AddStringToTextView( const os::String &cName ) const
{
	const os::TextView::buffer_type cBuffer = m_pcTextView->GetBuffer();
	const int posX = cBuffer[cBuffer.size()-1].size();
	const int posY = cBuffer.size()-1;
	const os::IPoint cPoint = os::IPoint( posX, posY );
	m_pcTextView->Insert( cPoint, cName.c_str() );
}

bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return ( true );
}

