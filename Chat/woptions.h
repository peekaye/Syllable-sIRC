#ifndef __WOPTIONS_H
#define __WOPTIONS_H




#include <gui/window.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <gui/checkbox.h>
#include <util/message.h>


class WRoster;

class WOptions : public os::Window {
public:
	WOptions(WRoster *pcWRoster);

	virtual void HandleMessage(os::Message *pcMessage);
	
	virtual bool OkToQuit();

private:
	enum { ID_BOK, ID_BCANCEL };
	
	os::TextView *m_pcFieldHost;
	os::TextView *m_pcFieldPort;
	os::TextView *m_pcFieldUser;
	os::TextView *m_pcFieldPass;
	os::TextView *m_pcFieldResource;
	os::CheckBox *m_pcCBFocusChat;
	
	os::StringView *m_pcStatus;
	
	WRoster *m_pcWRoster;
};






#endif








