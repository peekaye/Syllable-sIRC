#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include <util/application.h>
#include <util/message.h>
#include <util/settings.h>

//James Coxon jac208@cam.ac.uk
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
#include <gui/view.h>
#include <gui/rect.h>
#include <gui/font.h>
#include <gui/checkmenu.h>
#include <util/looper.h>
#include <util/string.h>
#include <util/resources.h>
#include <util/messenger.h>
#include <util/datetime.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <atheos/time.h>
#include "postoffice1.h"


using namespace os;
using namespace std;

#define SLEEP 999999999

//James Coxon jac208@cam.ac.uk
class CommThread : public os::Looper
{
public:
	CommThread( const os::Messenger& cTarget );
	virtual void HandleMessage( os::Message* pcMessage );
	virtual bool OkToQuit();
private:
	void ReceiveMessage();
	void SendMessage();
	void SendReceiveLoop();

	enum state_t { S_START, S_STOP };

	os::Messenger m_cTarget;
	state_t m_eState;
};


class MainWindow : public os::Window
{
public:
	MainWindow();
	void LoadBookmarkList();
	void HandleMessage( os::Message* );

	//James Coxon jac208@cam.ac.uk
	void Connect(void);
	void Disconnect(void);
	void Login(void);
	void SendMsg(void);
	void Join(void);
	void Update(void);

private:
	bool OkToQuit();
	BitmapImage* LoadImageFromResource( os::String zResource );
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
	std::string stdName;
	std::string stdPassword;
	std::string stdRealName;
	std::string stdServerName;
	std::string stdServerPort;
	std::string stdServerChannel;
	std::string stdInputMessage;

	//James Coxon jac208@cam.ac.uk
	CommThread* m_CommThread;	
};

#endif

