
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

#ifndef SIRC_SIRC_H
#define SIRC_SIRC_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include <iostream>
#include <string>

#include <gui/window.h>
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/menu.h>
#include <gui/requesters.h>
#include <gui/button.h>
#include <gui/point.h>
#include <gui/layoutview.h>
#include <gui/textview.h>
#include <gui/guidefines.h>

#include <util/application.h>
#include <util/message.h>
#include <util/string.h>
#include <util/invoker.h>
#include <util/messenger.h>

class CommThread;

//----------------------------------------------------------------------------------

class MainView:public os::View
{
public:
	MainView( const os::Rect &r );
	virtual void AllAttached();
	virtual void AttachedToWindow();
	virtual void FrameSized( const os::Point& cDelta );
	virtual void HandleMessage( os::Message *pcMessage );

private:
	void Update( const os::String cBufString );
	void AddStringToTextView( const os::String &cName ) const;

	os::Menu *mainMenuBar, *tempMenu;//os::Menu *m_pcMenuBar;
	os::MenuItem *m_pcJoinMenuItem, *m_pcLeaveMenuItem;
	os::TextView *m_Text, *m_Input;
	os::Button *m_pcSend;

	CommThread *m_CommThread;
	bool bInChannel;
};

//----------------------------------------------------------------------------------

class MainWindow:public os::Window
{
public:
	MainWindow( const os::Rect &r );
	virtual void HandleMessage( os::Message *pcMessage );
	virtual bool OkToQuit();

private:
	void ShowAbout( void );

	MainView *view;
};

//----------------------------------------------------------------------------------

class MyApplication:public os::Application
{
public:
	MyApplication( void );
	virtual void HandleMessage( os::Message *pcMessage );

private:
	MainWindow *myMainWindow;
};

#endif
