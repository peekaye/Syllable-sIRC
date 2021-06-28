#ifndef __WLOGIN_H
#define __WLOGIN_H



#include <gui/window.h>
#include <gui/listview.h>
#include <gui/textview.h>
#include <gui/dropdownmenu.h>
#include <gui/menu.h>
#include <gui/listview.h>
#include <util/message.h>
#include <string>
#include <vector>
#include <iostream>
#include <iso646.h>

class WOptions;
class WChat;

class UserListView;

class WRoster : public os::Window {
public:
	enum {
		// these must match to indexes in DropdownStatus
		STATUS_CHAT,
		STATUS_ONLINE,
		STATUS_DND,
		STATUS_AWAY,
		STATUS_XA,
		STATUS_UNAVAILABLE,
	};
	
	enum {
		//match indexes DropdownAction
		SEND_MSG,
		ADD_CONTACT,
	};
	WRoster();
	
	bool OkToQuit();
	virtual void HandleMessage(os::Message *pcMessage);
	void NewMessage();
	void SendNewMessage(std::string user, std::string sub, std::string msg);
	void NotifyStateChanged(int nState);
	void NotifyReceivedPresence(std::string cFrom, int nPresence);
	void NotifyReceivedMessage(int type, const std::string &id,
				const std::string &from, const std::string &subject,
				const std::string &body);
	void ReqCloseOptions();
	void ReqCloseChat(WChat *ptr);
	
	WChat *ShowChatForUser(const std::string &cJid);
	
private:
	enum {
		ID_NOTIFYSTATECHANGED,
		ID_NOTIFYRECEIVEDPRESENCE,
		ID_NOTIFYRECEIVEDMESSAGE,
		ID_REQCLOSEOPTIONS,
		ID_REQCLOSECHAT,
		
		ID_BCONNECT,
		ID_MQUIT,
		ID_MOPTIONS,
		ID_MSHOWLOG,
		ID_CHANGESTATUS,
		ID_DOACTION,
	};
	
	std::string JidStripResource(const std::string &str);
	
	os::Menu *m_pcMenu;
	os::Button *m_pcBtnConnect;
	os::DropdownMenu *m_pcDropdownAction;
	os::DropdownMenu *m_pcDropdownStatus;
	UserListView *m_pcViewUserList;
	
	WOptions *m_pcWOptions;
	std::vector<WChat *> m_cVectWChat;
};





#endif




























