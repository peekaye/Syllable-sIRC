#include <util/message.h>
#include <util/application.h>
#include <gui/window.h>
#include <gui/requesters.h>
#include <gui/textview.h>
#include "wlog.h"


#define W 500
#define H 300

using namespace os;


WLog::WLog()
		:Window(Rect(80,20,80+W-1,20+H-1),"log_window", "Log (Maja)")
{
	m_pcLog = new TextView(Rect(10,10,W-11,H-11),"logview",NULL,
						   CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|
						   CF_FOLLOW_TOP|CF_FOLLOW_BOTTOM);
	m_pcLog->SetMultiLine(true);
	m_pcLog->SetReadOnly(true);
	m_bShown = false;
	AddChild(m_pcLog);
}

void WLog::ReqInsert(const char *s, int len) {
	char buf[1024];
	int n;

	if (s == NULL)
		return;
	
	while (len > 0) {
		n = len<1024?len:1024;
		memcpy(buf,s,n);
		s += n;
		len -= n;
		buf[n] = 0;
		ReqInsert(buf);
	}
}

void WLog::ReqInsert(const char *s) {
	if (s == NULL)
		return;
	
	Message msg(ID_INSERT);
	msg.AddString("str",s);
	PostMessage(&msg,this);
}

void WLog::ReqShow() {
	PostMessage(new Message(ID_REQSHOW),this);
}

void WLog::HandleMessage( Message *pcMessage ) {
	switch(pcMessage->GetCode()) {
	case ID_INSERT:
		const char *s;
		pcMessage->FindString("str",&s);
		m_pcLog->Insert(s);
		break;
		
	case ID_REQSHOW:
		if (!m_bShown) {
			m_bShown = true;
			Show();
		}
		
	default:
		Window::HandleMessage( pcMessage );
	}
}

bool WLog::OkToQuit() {
	if (m_bShown) {
		m_bShown = false;
		Show(false);
	}
	return false;
}


