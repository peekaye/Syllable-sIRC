#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <iostream>

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
#include <gui/guidefines.h>
#include <util/application.h>
#include <util/message.h>
#include <util/settings.h>
#include <util/string.h>
#include <util/invoker.h>
#include <util/datetime.h>
#include <util/exceptions.h>
#include <storage/file.h>
#include <storage/path.h>
#include <storage/fsnode.h>
#include <storage/directory.h>
#include <storage/filereference.h>

class CommThread;

class MainWindow:public os::Window
{
public:
	MainWindow();
	virtual void HandleMessage( os::Message *pcMessage );
	virtual bool OkToQuit();

private:
	void SetBookmarkPath();
	void LoadBookmarkList( os::Directory *pcDirectory );
	void AddBookmarkToList();
	void RemoveBookmarkFromList();
	void SelectBookmarkFromList();
	os::BitmapImage *LoadImageFromResource( os::String zResource );
	void AddStringToTextView( const char* pzName ) const;
	void AboutRequested();

#include "mainwindowLayout.h"

	os::Directory *m_pcDirectory;
	os::String stdName;
	os::String stdPassword;
	os::String stdRealName;
	os::String stdServerName;
	os::String stdServerPort;
	os::String stdServerChannel;

	CommThread *m_CommThread;
};

#endif

