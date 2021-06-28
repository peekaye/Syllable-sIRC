
#include "mainwindow.h"
#include "commthread.h"
#include "messages.h"

MainWindow::MainWindow():os::Window( os::Rect( 0, 0, 750, 500 ), "main_wnd", "bsIRC" )
{
	os::LayoutView * pcView = new os::LayoutView( GetBounds(), "layout_view" );

#include "mainwindowLayout.cpp"

	// set tab order
	uint iTabOrder = 0;
	m_pcTextUserName->SetTabOrder( iTabOrder++ );
	m_pcTextUserPassword->SetTabOrder( iTabOrder++ );
	m_pcTextUserRealName->SetTabOrder( iTabOrder++ );
	m_pcTextServerName->SetTabOrder( iTabOrder++ );
	m_pcTextServerPort->SetTabOrder( iTabOrder++ );
	m_pcTextServerChannel->SetTabOrder( iTabOrder++ );

	pcView->SetRoot( m_pcRoot );
	AddChild( pcView );

	SetBookmarkPath();

	/* create a protected commthread instance */
	CommThread *m_pCommThread;
	m_pCommThread = new CommThread( this );
	if( m_pCommThread == NULL )
	{
		os::Alert *sAlert = new os::Alert( "Error", os::String().Format( "Failed to create a commthread instance\nReason: %s", strerror( errno ) ), 0, "OK", NULL );
		sAlert->Go();
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
			AddBookmarkToList();
			break;
		}

		case M_BUTTON_REMOVEBOOKMARK:
		{
			RemoveBookmarkFromList();
			break;
		}

		case M_BUTTON_BCONNECT:
		{
			SelectBookmarkFromList();
			break;
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

			const os::String cMessage = m_pcMessageText->GetValue().AsString();
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

		case os::M_ABOUT_REQUESTED:
		{
			AboutRequested();
			break;
		}

		case MSG_FROMLOOPER_NEW_MESSAGE:
		{
			const char *pzName;
			int nCount = 0;

			pcMessage->GetNameInfo( "name", NULL, &nCount );

			for( int i = 0; i < nCount; ++i )
			{
				if( pcMessage->FindString( "name", &pzName, i ) == EOK )
					AddStringToTextView( pzName );
			}
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

void MainWindow::SetBookmarkPath()
{
	os::Path cBookmarkPath;
	os::FSNode cNode;
	os::FileReference cRef;

	cBookmarkPath.SetTo( getenv( "HOME" ) );
	cBookmarkPath.Append( "/Settings" );

	if( cNode.SetTo( cBookmarkPath.GetPath() ) != EOK )
		mkdir( cBookmarkPath.GetPath().c_str(), S_IRWXU | S_IRWXG | S_IRWXO );

	cNode.Unset();
	cBookmarkPath.Append( "/bsIRC" );
	if( cNode.SetTo( cBookmarkPath.GetPath() ) != EOK )
		mkdir( cBookmarkPath.GetPath().c_str(), S_IRWXU | S_IRWXG | S_IRWXO );

	if( cRef.SetTo( cBookmarkPath ) == EOK )
	{
		m_pcDirectory = new os::Directory( cBookmarkPath );
		LoadBookmarkList( m_pcDirectory );
	}
}

void MainWindow::LoadBookmarkList( os::Directory *pcDirectory )
{
	/* Open up directory and check it actually contains something */
	if( pcDirectory )
	{
		/* Clear listview */
		m_pcBookmarkList->Clear();

		/* Loop through directory and add each entry to the list */
		os::String cEntry;
		while( pcDirectory->GetNextEntry( &cEntry ) )
		{
			/* If special directory (i.e. dot(.) and dotdot(..) then ignore */
			if( cEntry == "." || cEntry == ".." )
			{
				continue;
			}

			/* Its valid, add to the listview */
			os::ListViewStringRow * pcRow = new os::ListViewStringRow();
			pcRow->AppendString( cEntry );
			m_pcBookmarkList->InsertRow( pcRow );
		}
	}
}

void MainWindow::AddBookmarkToList()
{
	os::Variant cVar( m_pcTextBookmarkName->GetValue().AsString() );
	os::String cBookmarkName = cVar.AsString();

	if( !cBookmarkName.empty() )
	{
		os::String zPath;
		if( m_pcDirectory->GetPath( &zPath ) == -1 )
			return;

		try
		{
			zPath += os::String ( "/" ) + cBookmarkName;
			os::File cFile( zPath, O_RDWR | O_CREAT );

			os::Settings * pcSettings = new os::Settings( new os::File( cFile ) );
			pcSettings->SetString( "Name", m_pcTextUserName->GetValue().AsString() );
			pcSettings->SetString( "Password", m_pcTextUserPassword->GetValue().AsString() );
			pcSettings->SetString( "RealName", m_pcTextUserRealName->GetValue().AsString() );
			pcSettings->SetString( "ServerName", m_pcTextServerName->GetValue().AsString() );
			pcSettings->SetString( "ServerPort", m_pcTextServerPort->GetValue().AsString() );
			pcSettings->SetString( "ServerChannel", m_pcTextBookmarkName->GetValue().AsString() );
			pcSettings->Save();
			delete( pcSettings );

			/* Make sure there's only one bookmark with the same name, on the list */
			bool bSameName = false;

			for( uint i = 0; i < m_pcBookmarkList->GetRowCount(); ++i )
			{
				os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( i ) );
				if( cBookmarkName == pcRow->GetString( 0 ) )
					bSameName = true;
			}

			if( bSameName == false )
			{
				os::ListViewStringRow *row = new os::ListViewStringRow();
				row->AppendString( cBookmarkName );
				m_pcBookmarkList->InsertRow( row );
			}
		}
		catch( std::exception &e )
		{
			std::cerr << zPath.str() << ": " << e.what() << std::endl;
		}
	}
}

void MainWindow::RemoveBookmarkFromList()
{
	int nSelected = m_pcBookmarkList->GetLastSelected();
	os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( nSelected ) );
	if( pcRow )
	{
		os::String cBookmarkPath;
		if( m_pcDirectory->GetPath( &cBookmarkPath ) == -1 )
			return;
		os::String cBookmarkName = pcRow->GetString( 0 );

		if( m_pcBookmarkList->GetRowCount() > 0 && nSelected != -1 )
		{
			m_pcBookmarkList->RemoveRow( nSelected );

			os::FileReference cRef( cBookmarkPath + "/" + cBookmarkName );
			cRef.Delete();

			m_pcTextUserName->Clear();
			m_pcTextUserPassword->Clear();
			m_pcTextUserRealName->Clear();
			m_pcTextServerName->Clear();
			m_pcTextServerPort->Clear();
			m_pcTextServerChannel->Clear();
			m_pcTextBookmarkName->Clear();
		}
	}
}

void MainWindow::SelectBookmarkFromList()
{
	int nSelected = m_pcBookmarkList->GetLastSelected();
	if( nSelected == -1 )
		return;

	os::ListViewStringRow *pcRow = static_cast < os::ListViewStringRow * >( m_pcBookmarkList->GetRow( nSelected ) );
	if( pcRow )
	{
		os::String cBookmarkName = pcRow->GetString( 0 );

		if( !cBookmarkName.empty() )
		{
			os::String cBookmarkPath;
			if( m_pcDirectory->GetPath( &cBookmarkPath ) == -1 )
				return;
			os::String zPath( cBookmarkPath + "/" + cBookmarkName );

			try
			{
				/* Load settings */
				os::Settings* pcSettings = new os::Settings( new os::File( zPath ) );
				if( pcSettings->Load() == EOK )
				{
					m_pcTextUserName->Set( pcSettings->GetString( "Name" ).c_str() );
					m_pcTextUserPassword->Set( pcSettings->GetString( "Password" ).c_str() );
					m_pcTextUserRealName->Set( pcSettings->GetString( "RealName" ).c_str() );
					m_pcTextServerName->Set( pcSettings->GetString( "ServerName" ).c_str() );
					m_pcTextServerPort->Set( pcSettings->GetString( "ServerPort" ).c_str() );
					m_pcTextServerChannel->Set( pcSettings->GetString( "ServerChannel" ).c_str() );
					m_pcTextBookmarkName->Set( cBookmarkName.c_str() );
				}
				delete( pcSettings );
			}
			catch( ... )
			{
			}
		}
	}
}

void MainWindow::AddStringToTextView( const char* pzName ) const
{
	os::String cName;
	os::DateTime pcTime = os::DateTime::Now();

	if( pcTime.GetHour() < 10 && pcTime.GetMin() > 9 )
		cName.Format( "[0%d:%d] %s", pcTime.GetHour(), pcTime.GetMin(), pzName );
	else if( pcTime.GetHour() > 9 && pcTime.GetMin() < 10 )
		cName.Format( "[%d:0%d] %s", pcTime.GetHour(), pcTime.GetMin(), pzName );
	else if( pcTime.GetHour() < 10 && pcTime.GetMin() < 10 )
		cName.Format( "[0%d:0%d] %s", pcTime.GetHour(), pcTime.GetMin(), pzName );
	else
		cName.Format( "[%d:%d] %s", pcTime.GetHour(), pcTime.GetMin(), pzName );

	/* add new text to the end of the text view buffer */
	const os::TextView::buffer_type cBuffer = m_pcTextView->GetBuffer();
	const int posX = cBuffer[cBuffer.size()-1].size();
	const int posY = cBuffer.size()-1;
	const os::IPoint cPoint = os::IPoint( posX, posY );
	m_pcTextView->Insert( cPoint, cName.c_str() );
}

void MainWindow::AboutRequested()
{
	os::String cText = "bsIRC 0.12\n";
	cText += "Syllable Internet Relay Chat Client\n\n";
	cText += "bsIRC: Copyright © 2006 Flemming H. Sørensen (BurningShadow)\n";
	cText += "libnet: Copyright © 2004 Kristian van der Vliet (vanders)\n";
	cText += "commits: Copyright © 2021 David Kent\n\n";
	cText += "bsIRC is released under the GNU General Public License.\n";
	cText += "Please see the file COPYING, distributed with bsIRC,\n";
	cText += "or http://www.gnu.org for more information.";

	os::Alert *sAbout = new os::Alert( "About bsIRC...", cText, os::Alert::ALERT_INFO, 0x00, "Close", NULL );
	sAbout->CenterInWindow( this );
	sAbout->Go( new os::Invoker() );
}

bool MainWindow::OkToQuit()
{
	os::Application::GetInstance()->PostMessage( os::M_QUIT );
	return ( true );
}

