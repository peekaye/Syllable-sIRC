#ifndef __WCHAT_H
#define __WCHAT_H




#include <gui/window.h>
#include <gui/textview.h>
#include <util/message.h>
#include <string>


class Socket;
class Poller;
class MyTextView;
class WRoster;

class WChat : public os::Window {
friend class MyTextView;
public:
	WChat(WRoster *pcWroster, const std::string &cUser);
	virtual void HandleMessage(os::Message *pcMessage);
	virtual bool OkToQuit(void);

	void ReqShowMessage(int type, const std::string &from,
						const std::string &subject, const std::string &body);
	
	std::string GetUser() { return m_cUser; }


private:
	enum { ID_BPLAIN, ID_BMESSAGE, ID_SENDMESSAGE, ID_SHOWMESSAGE };
	Socket *m_pcSocket;
	Poller *m_pcPoller;
	
	MyTextView *m_pcInput;
	os::TextView *m_pcLogView;
	
	std::string m_cUser;
	WRoster *m_pcWRoster;
};




#endif





