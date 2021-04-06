
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/menu.h>
#include <gui/requesters.h>
#include <gui/font.h>
#include <gui/checkmenu.h>
#include <gui/layoutview.h>
#include <gui/textview.h>

#include <util/application.h>
#include <util/message.h>
#include <util/looper.h>
#include <util/string.h>
#include <util/resources.h>
#include <util/settings.h>
#include <util/messenger.h>
#include <util/datetime.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <atheos/time.h>

enum
{
	M_MENU_QUIT,
	M_MENU_ABO,
	M_MENU_CONNECT,
	M_MENU_DISCONNECT,
	M_MENU_LOGIN,

	MSG_SEND,
	MSG_JOINCHANNEL,
	MSG_UPDATE,
	MSG_SETTINGS,
	MSG_SINGLEUPDATE,
	MSG_TOLOOPER_START,
	MSG_TOLOOPER_SEND,
	MSG_TOLOOPER_RECEIVE,
	MSG_FROMLOOPER_NEW_MESSAGE
};

#define PORT 6667 // the port client will be connecting to 
#define MAXDATASIZE 50000 // max number of bytes we can get at once

//----------------------------------------------------------------------------------

class CommThread : public os::Looper
{
public:	
	CommThread( const os::Messenger& cTarget );
	virtual void HandleMessage( os::Message* pcMessage );
	virtual bool OkToQuit();

private:
	void SendReceiveLoop();
	void SendMessage();
	void ReceiveMessage();

	enum state_t { S_START, S_STOP };

	os::Messenger m_cTarget;
	state_t m_eState;
};

//----------------------------------------------------------------------------------

class MainView : public os::LayoutView
{
public:
	MainView( const os::Rect & r );
	void AttachedToWindow();
	void HandleMessage( os::Message* pcMessage );
	bool OkToQuit();

protected:
	os::Button* m_pcStart;

private:
	void Connect( void );
	void Disconnect( void );
	void Update( void );
	void Login( void );
	void Join( void );
	void SendMsg( void );
	void ShowAbout( void );

	os::Menu *mainMenuBar, *tempMenu;
	os::TextView* textOutputView;
	os::TextView* textInputView;

	CommThread* m_CommThread;
};

//----------------------------------------------------------------------------------

class MainWindow : public os::Window
{
public:
	MainWindow( const os::Rect & r );
	bool OkToQuit();

private:
	MainView *view;
};

//----------------------------------------------------------------------------------

class MyApplication : public os::Application
{
public:
	MyApplication( void );

private:
	MainWindow *myMainWindow;
};

