#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <gui/window.h>
#include <gui/layoutview.h>
#include <gui/button.h>
#include <gui/menu.h>
#include <gui/listview.h>
#include <gui/tabview.h>
#include <gui/imagebutton.h>
#include <gui/frameview.h>
#include <gui/stringview.h>
#include <gui/separator.h>
#include <gui/textview.h>
#include <gui/requesters.h>
#include <gui/image.h>
#include <gui/point.h>
#include <util/application.h>
#include <util/message.h>
#include <util/settings.h>
#include <util/string.h>
#include <util/invoker.h>
#include <storage/file.h>

class CommThread;

class MainWindow:public os::Window
{
public:
	MainWindow();
	virtual void HandleMessage( os::Message *pcMessage );
	virtual bool OkToQuit();

private:
	void LoadBookmarkList();
	os::BitmapImage *LoadImageFromResource( os::String zResource );
	bool PingPong( const os::String &cName ) const;
	void AddStringToTextView( const os::String &cName ) const;

#include "mainwindowLayout.h"

	os::String cMessage;
	os::String cBookmarkPath;
	os::String cBookmarkName;
	os::String cName;
	os::String cPassword;
	os::String cRealName;
	os::String cServerName;
	os::String cServerPort;
	os::String cServerChannel;
	os::String stdName;
	os::String stdPassword;
	os::String stdRealName;
	os::String stdServerName;
	os::String stdServerPort;
	os::String stdServerChannel;

	CommThread *m_CommThread;
};

#endif

