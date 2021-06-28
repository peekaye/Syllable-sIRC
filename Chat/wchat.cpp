#include <util/message.h>
#include <util/application.h>
#include <gui/window.h>
#include <gui/requesters.h>
#include <gui/textview.h>
#include "wchat.h"
#include "wroster.h"
#include "app.h"


#define W 500
#define H 220

#define BTNW 80
#define BTNH 25

using namespace os;





class MyTextView : public TextView {
public:
	MyTextView(const Rect &r, const char *pzTitle, const char *pzBuffer,
			   uint32 nResizeMask)
			: TextView(r,pzTitle,pzBuffer,nResizeMask) {}
	
	virtual void KeyDown(const char *pzString, const char *pzRawString,
						 uint32 nQualifiers);
	
	void SendMe();
};



WChat::WChat(WRoster *pcWRoster, const std::string &cUser)
		:Window(Rect(80,20,80+W-1,20+H-1),"chat_window",
				std::string("Chat with ")+cUser+" (Chat)")
{
	int y = H*2/3;
	m_pcLogView = new TextView(Rect(10,10,W-11,y-6),"logview",NULL,
							   CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|
							   CF_FOLLOW_TOP|CF_FOLLOW_BOTTOM);
	m_pcLogView->SetMultiLine(true);
	m_pcLogView->SetReadOnly(true);
	AddChild(m_pcLogView);

	m_pcInput = new MyTextView(Rect(10,y+5,W-BTNW-21,H-11),"textview",NULL,
							   CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|
							   CF_FOLLOW_BOTTOM);
	m_pcInput->SetMultiLine(true);
	AddChild(m_pcInput);

/*	
	AddChild(new Button(Rect(W-BTNW-10,y+5,W-11,y+5+BTNH-1),"btn_plain",
						"Send Plain", new Message(ID_BPLAIN),
						CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM));
*/
	AddChild(new Button(Rect(W-BTNW-10,y+5+BTNH+5,W-11,y+5+BTNH+5+BTNH-1),
						"btn_msg", "Message", new Message(ID_BMESSAGE),
						CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM));
	
	m_pcWRoster = pcWRoster;
	m_cUser = cUser;
}

void WChat::HandleMessage( Message *pcMessage ) {
	int i,n;
	std::string str;
	
	switch(pcMessage->GetCode()) {

	case ID_BPLAIN:
		n = m_pcInput->GetBuffer().size();
		for (i = 0; i < n; i ++) {
			if (i > 0) str += "\n";
			str += m_pcInput->GetBuffer()[i];
		}
		
		m_pcInput->Clear();

		g_pcApplication->ReqSendPlain(str);
		break;

	case ID_BMESSAGE:
	case ID_SENDMESSAGE:
		m_pcLogView->Insert("S:");
		n = m_pcInput->GetBuffer().size();
		for (i = 0; i < n; i ++) {
			m_pcLogView->Insert("  ");
			m_pcLogView->Insert(m_pcInput->GetBuffer()[i].c_str());
			m_pcLogView->Insert("\n");
			str += m_pcInput->GetBuffer()[i];
		}
		
		m_pcInput->Clear();

		g_pcApplication->ReqSendMessage(JabberSession::MSGTYPE_CHAT,
										m_cUser,"",str);
		break;
		
	case ID_SHOWMESSAGE:
	{
		int32 type;
		std::string from,subject,body;
		int i,j;
		
		pcMessage->FindInt32("type",&type);
		pcMessage->FindString("from",&from);
		pcMessage->FindString("subject",&subject);
		pcMessage->FindString("body",&body);
		m_pcLogView->Insert("R:");
		if (subject != "") {
			m_pcLogView->Insert("(");
			m_pcLogView->Insert(subject.c_str());
			m_pcLogView->Insert(")");
		}
		i = 0;
		while (i < (int)body.size()) {
			m_pcLogView->Insert("  ");
			j = body.find('\n',i);
			if (j == -1) j = body.size()-1;
			m_pcLogView->Insert(body.substr(i,j-i+1).c_str());
			i = j+1;
		}
		m_pcLogView->Insert("\n");
		break;
	}

	default:
		Window::HandleMessage( pcMessage );
	}
}

bool WChat::OkToQuit( void ) {
	m_pcWRoster->ReqCloseChat(this);
	return false;
}

void WChat::ReqShowMessage(int type, const std::string &from,
						   const std::string &subject, const std::string &body) {
	Message msg(ID_SHOWMESSAGE);
	msg.AddInt32("type",type);
	msg.AddString("from",from);
	msg.AddString("subject",subject);
	msg.AddString("body",body);
	PostMessage(&msg,this);
}


void MyTextView::KeyDown(const char *pzString, const char *pzRawString,
						 uint32 nQualifiers) {
	if (pzRawString[0] == '\n' && pzRawString[1] == 0) {
		if (nQualifiers) {
			TextView::KeyDown("\n", "\n", 0);
			return;
		}
		else {
			Window *w = GetWindow();
			w->PostMessage(WChat::ID_SENDMESSAGE,w);
			return;
		}
	}
	
	if (nQualifiers & (QUAL_LCTRL|QUAL_RCTRL)) {
		if (pzRawString[0] == 'c' && pzRawString[1] == 0) {
			Copy();
		}
		if (pzRawString[0] == 'x' && pzRawString[1] == 0) {
			Cut();
		}
		if (pzRawString[0] == 'v' && pzRawString[1] == 0) {
			Paste();
		}
		return;
	}
	
	TextView::KeyDown(pzString, pzRawString, nQualifiers);
}






