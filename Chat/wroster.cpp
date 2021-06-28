#include <util/message.h>
#include <util/application.h>
#include <gui/window.h>
#include <gui/requesters.h>
#include <gui/stringview.h>
#include <gui/textview.h>
#include <gui/menu.h>
#include "wroster.h"
#include "woptions.h"
#include "wchat.h"
#include "wlog.h"
#include "app.h"
#include "def.h"


#define W      220
#define H      230
#define STLINEH 17


#define STATUSTOP  (BTNTOP+BTNH+2)
#define STATUSH    20

#define ID_CANCEL_NMWINDOW 0x43
#define ID_OK_NMWINDOW  0x44

using namespace os;




struct UserInfo {
	std::string m_cJid;
	int m_nPresence;
	float m_fPaintY,m_fPaintH;
};



class UserListView : public os::View {
public:
	UserListView(WRoster *pcWRoster, const Rect &cFrame,
				 const std::string &cTitle,
				 uint32 nResizeMask=CF_FOLLOW_LEFT|CF_FOLLOW_TOP,
				 uint32 nFlags=WID_WILL_DRAW|WID_CLEAR_BACKGROUND);	 
	

	virtual void Paint(const Rect &cUpdateRect);
	virtual void MouseMove(const Point &cNewPoint, int nCode, uint32 nButtons,
						   Message *pcData);
	virtual void MouseUp(const Point &cPosition, uint32 nButtons,
						 Message *pcData);
	
	void SetPresence(const std::string &cJid, int nPresence);
	void SetAllUnavailable();
	
private:
	WRoster *m_pcWRoster;
	std::vector<UserInfo *> m_cVect;
	UserInfo *m_pcCursor;
	
	UserInfo *PointToUser(const Point &p);
};




UserListView::UserListView(WRoster *pcWRoster, const Rect &cFrame,
						   const std::string &cTitle,
						   uint32 nResizeMask,
						   uint32 nFlags):
		View(cFrame,cTitle,nResizeMask,nFlags)
{
	m_pcWRoster = pcWRoster;
	m_pcCursor = NULL;
}

void UserListView::SetPresence(const std::string &cJid, int nPresence) {
	std::vector<UserInfo *>::iterator it;
	UserInfo *pcUInfo;

	for (it = m_cVect.begin(); it != m_cVect.end(); it++) {
		if ((*it)->m_cJid == cJid) {
			(*it)->m_nPresence = nPresence;
			break;
		}
	}
	if (it == m_cVect.end()) { // not found
		pcUInfo = new UserInfo();
		pcUInfo->m_cJid = cJid;
		pcUInfo->m_nPresence = nPresence;
		pcUInfo->m_fPaintY = 0;
		pcUInfo->m_fPaintH = 0;
		m_cVect.push_back(pcUInfo);
	}
	Invalidate();
	Flush();
}

void UserListView::SetAllUnavailable() {
	std::vector<UserInfo *>::iterator it;

	for (it = m_cVect.begin(); it != m_cVect.end(); it++)
		(*it)->m_nPresence = JabberSession::PRESENCE_UNAVAILABLE;
	Invalidate();
	Flush();
}

void UserListView::Paint(const Rect &cUpdateRect) {
	std::string str;
	font_height fh;
	float lineh;
	std::vector<UserInfo *>::iterator it;
	int i;
	float y,w;

	SetBgColor(255,255,255);
	View::Paint(cUpdateRect);
	GetFontHeight(&fh);
	lineh = fh.ascender+fh.descender+fh.line_gap;
	y = 0;
	w = Width();
	for (it = m_cVect.begin(); it != m_cVect.end(); it ++) {
		i = m_pcCursor == (*it) ? 255 : 0;
		SetFgColor(255 ^ i ,255 ^ i ,255 ^ i );
		SetBgColor(255 ^ i ,255 ^ i ,255 ^ i ); // needed for Anti-Aliasing
		FillRect(Rect(0,y,w,y+lineh));
		switch((*it)->m_nPresence) {
			case JabberSession::PRESENCE_CHAT:
			{
				str = "[CHT]";
				SetFgColor(0 ^ i ,0 ^ i ,255 ^ i );
				break;
			}
			case JabberSession::PRESENCE_ONLINE:
				str = "[ON]";
				SetFgColor(0 ^ i ,0 ^ i ,255 ^ i );
				break;
			case JabberSession::PRESENCE_DND:
				str = "<DND>";
				SetFgColor(0 ^ i ,0 ^ i ,0 ^ i );
				break;
			case JabberSession::PRESENCE_AWAY:
				str = "(aw)";
				SetFgColor(0 ^ i ,0 ^ i ,0 ^ i );
				break;
			case JabberSession::PRESENCE_XA:
				str = "(xa)";
				SetFgColor(0 ^ i ,0 ^ i ,0 ^ i );
				break;
			case JabberSession::PRESENCE_UNAVAILABLE:
				str = "(off)";
				SetFgColor(0 ^ i ,0 ^ i ,0 ^ i );
				break;
			default:
				str = "<?>";
				SetFgColor(255 ^ i ,0 ^ i ,0 ^ i );
		}
		str += " ";
		str += (*it)->m_cJid;
		(*it)->m_fPaintH = lineh;
		(*it)->m_fPaintY = y;
		DrawString(Point(0,y+fh.ascender),str);

		y += lineh;
	}
	
}

UserInfo *UserListView::PointToUser(const Point &p) {
	std::vector<UserInfo *>::iterator it;
	
	if (p.x < 0 || p.x >= Width())
		return NULL;
	for (it = m_cVect.begin(); it != m_cVect.end(); it ++)
		if ((*it)->m_fPaintY <= p.y &&
				p.y < (*it)->m_fPaintY+(*it)->m_fPaintH)
			return (*it);
	return NULL;
}

void UserListView::MouseMove(const Point &cNewPoint, int nCode, uint32 nButtons,
							 Message *pcData)
{
	UserInfo *pcNewCursor = PointToUser(cNewPoint);
	if (nButtons == 0) {
		if (m_pcCursor != pcNewCursor) {
			m_pcCursor = pcNewCursor;
			Invalidate();
			Flush();
		}
	}
	else {
		if (m_pcCursor != NULL) {
			m_pcCursor = NULL;
			Invalidate();
			Flush();
		}
	}
}

void UserListView::MouseUp(const Point &cPosition, uint32 nButtons,
						   Message *pcData)
{
	UserInfo *pcCursor;
	
	if (pcData == NULL && (nButtons&1)) {	// no Drag&Drop and left button
		pcCursor = PointToUser(cPosition);
		if (m_pcCursor == pcCursor && pcCursor != NULL) {
			m_pcWRoster->ShowChatForUser(pcCursor->m_cJid);
		}
		else {
			m_pcCursor = pcCursor;
			Invalidate();
			Flush();
		}
	}
}


class NMWindow : public Window
{
public:
    NMWindow(WRoster* pcParent);
    void CenterWindow(Window* pcWin);
private:
    TextView* pcTo;
   TextView* pcSub;
   TextView* pcMsg;
    Button* pcOk;
    Button* pcCancel;
    virtual void HandleMessage(Message* pcMessage);
    WRoster* pcParentWindow;
};

NMWindow::NMWindow(WRoster* pcParent) : Window(Rect(0, 0, 200, 200), "_nmsg_window", "Send New Message...", WND_NOT_MOVEABLE | WND_NOT_RESIZABLE)
{
    pcParentWindow = pcParent;

    pcTo = new TextView(Rect(10,10, 190,30),"_user","");
    AddChild(pcTo);

    pcSub = new TextView(Rect(10, 35, 190, 55), "_sub","");
    AddChild(pcSub);
    pcMsg = new TextView(Rect(10,60,190,160),"_msg","");
    pcMsg->SetMultiLine(true);
    AddChild(pcMsg);


    pcOk = new Button(Rect(40, 165, 85, 190), "_ok_but","Ok",new Message(ID_OK_NMWINDOW));
    AddChild(pcOk);

    pcCancel = new Button(Rect(90, 165, 135, 190), "_cancel_but", "Cancel", new Message(ID_CANCEL_NMWINDOW));
    AddChild(pcCancel);
	 
}

void NMWindow::HandleMessage(Message* pcMessage)
{
switch (pcMessage->GetCode())
    {
    case ID_CANCEL_NMWINDOW:
        Close();
        break;

    case ID_OK_NMWINDOW:
		{
 	std::string user=pcTo->GetBuffer()[0];
	std::string sub=pcSub->GetBuffer()[0];
	std::string msg=pcMsg->GetBuffer()[0];
	pcParentWindow->SendNewMessage(user,sub,msg);
	Close();
        break;
		}
    default:
        Window::PostMessage(pcMessage);
        break;
    }
}

void NMWindow::CenterWindow( Window* pcWin )
{
    Rect cBounds = GetBounds();
    Rect cR = pcWin->GetFrame();

    MoveTo( cR.left + ( cR.Width() - cBounds.Width() ) / 2,
            cR.top  + ( cR.Height() - cBounds.Height() ) / 2 );
}

WRoster::WRoster()
		:Window(Rect(80,20,80+W-1,20+H-1),"roster_window","Chat")
{

	Menu *m/*,*m1*/;

/*	
	m1 = new Menu(Rect(0,0,0,0),"Account >",ITEMS_IN_COLUMN);
	m1->AddItem("Change Password",NULL);
	m1->AddItem("Create",NULL);
	m1->AddItem("Remove",NULL);
	m1->SetTargetForItems(this);	// FIXME: Is this necessary?
*/

	m = new Menu(Rect(0,0,0,0),"menu",ITEMS_IN_COLUMN);
//	m->AddItem(m1);
//	m->AddItem("Disconnect",NULL);
//	m->AddItem(new os::MenuSeparator());
	m->AddItem("Show received xml",new Message(ID_MSHOWLOG));
	m->AddItem("Options...",new Message(ID_MOPTIONS));
	m->AddItem(new os::MenuSeparator());
	m->AddItem("Quit",new Message(ID_MQUIT));
	m->SetTargetForItems(this);

	m_pcMenu = new Menu(Rect(0,H-STLINEH,40,H-1),
						"Jabber",ITEMS_IN_ROW,CF_FOLLOW_LEFT|CF_FOLLOW_BOTTOM);
	m_pcMenu->AddItem(m);
	AddChild(m_pcMenu);

	m_pcDropdownStatus = new DropdownMenu(Rect(40,H-STLINEH,119,H-1),
						   "menu_status",
						   CF_FOLLOW_LEFT|CF_FOLLOW_BOTTOM);
	
	m_pcDropdownStatus->AppendItem("Free for chat");
	m_pcDropdownStatus->AppendItem("Online");
	m_pcDropdownStatus->AppendItem("Do not disturb");
	m_pcDropdownStatus->AppendItem("Away");
	m_pcDropdownStatus->AppendItem("XAway");
	m_pcDropdownStatus->AppendItem("Not Available");
	m_pcDropdownStatus->SetReadOnly(true);
	m_pcDropdownStatus->Show(false); // na tomhle miste ukazeme m_pcBtnConnect
	m_pcDropdownStatus->SetSelectionMessage(new Message(ID_CHANGESTATUS));
	m_pcDropdownStatus->SetTarget(this,this);
	AddChild(m_pcDropdownStatus);
	m_pcDropdownAction = new DropdownMenu(Rect(120,H-STLINEH,219,H-1), "menu action", CF_FOLLOW_BOTTOM);
	m_pcDropdownAction->AppendItem("Send New Msg");
	m_pcDropdownAction->AppendItem("Add Contact");
	m_pcDropdownAction->Show(false);
	m_pcDropdownAction->SetSelectionMessage(new Message(ID_DOACTION));
	m_pcDropdownAction->SetTarget(this,this);
	AddChild(m_pcDropdownAction);
	m_pcBtnConnect = new Button(Rect(40,H-STLINEH,119,H-1),
								"btn_connect","<connect>",
								new Message(ID_BCONNECT),
								CF_FOLLOW_LEFT|CF_FOLLOW_RIGHT|CF_FOLLOW_BOTTOM);
	AddChild(m_pcBtnConnect);
	m_pcViewUserList = new UserListView(this,Rect(0,0,119,H-STLINEH-1),
				"roster-userlist",CF_FOLLOW_ALL);
	AddChild(m_pcViewUserList);
	
	m_pcWOptions = NULL;
}

WChat *WRoster::ShowChatForUser(const std::string &cJid) {
	std::vector<WChat *>::iterator it;
	WChat *pcNewChat;
	for (it = m_cVectWChat.begin(); it != m_cVectWChat.end(); it ++) {
		if ((*it)->GetUser() == cJid) {
			return (*it);
		}
	}
	
	pcNewChat = new WChat(this,cJid);
	pcNewChat->Show();
	m_cVectWChat.push_back(pcNewChat);
	return pcNewChat;
}
void WRoster::SendNewMessage(std::string user, std::string sub, std::string msg)
{
g_pcApplication->ReqSendMessage(JabberSession::MSGTYPE_CHAT,user,sub,msg);
}

void WRoster::NewMessage()
{
	NMWindow* win = new NMWindow(this);
	win->CenterWindow(this);
	win->Show();
	win->MakeFocus();
	
}

void WRoster::NotifyStateChanged(int nState) {
	Message msg(ID_NOTIFYSTATECHANGED);
	msg.AddInt32("state",nState);
	PostMessage(&msg,this);
}

void WRoster::NotifyReceivedPresence(std::string cFrom, int nPresence) {
	Message msg(ID_NOTIFYRECEIVEDPRESENCE);
	msg.AddString("from",cFrom);
	msg.AddInt32("presence",nPresence);
	PostMessage(&msg,this);
}

void WRoster::ReqCloseOptions() {
	PostMessage(ID_REQCLOSEOPTIONS,this);
}

void WRoster::ReqCloseChat(WChat *ptr) {
	Message msg(ID_REQCLOSECHAT);
	msg.AddPointer("ptr",*(void **)&ptr);
	PostMessage(&msg,this);
}

void WRoster::NotifyReceivedMessage(int type, const std::string &id,
				const std::string &from, const std::string &subject,
				const std::string &body)
{
	Message msg(ID_NOTIFYRECEIVEDMESSAGE);
	msg.AddInt32("type",type);
	msg.AddString("id",id);
	msg.AddString("from",from);
	msg.AddString("subject",subject);
	msg.AddString("body",body);
	PostMessage(&msg,this);
}

void WRoster::HandleMessage( Message *pcMessage ) {
	int32 st;
	int status;
	int32 type;
	std::string from,subject,body;
	int32 presence;
	WChat *wchat;
	std::vector<WChat *>::iterator it;
	ConfigFile *cf;
	
	switch(pcMessage->GetCode()) {
	case ID_NOTIFYSTATECHANGED:
		pcMessage->FindInt32("state",&st);
		
		if (st == App::ST_DISCONNECTED) {
			if (!m_pcBtnConnect->IsVisible()) m_pcBtnConnect->Show(true);
			if (m_pcDropdownStatus->IsVisible()) m_pcDropdownStatus->Show(false);
			if(m_pcDropdownAction->IsVisible()) m_pcDropdownAction->Show(false);
		}
		else if (st < App::ST_AUTHENTICATED) {
			if (m_pcBtnConnect->IsVisible()) m_pcBtnConnect->Show(false);
			if (m_pcDropdownStatus->IsVisible()) m_pcDropdownStatus->Show(false);
			if(m_pcDropdownAction->IsVisible()) m_pcDropdownAction->Show(false);
		}
		else {
			if (m_pcBtnConnect->IsVisible()) m_pcBtnConnect->Show(false);
			if (!m_pcDropdownStatus->IsVisible()) m_pcDropdownStatus->Show(true);
			if(!m_pcDropdownAction->IsVisible()) m_pcDropdownAction->Show(true);
			m_pcDropdownStatus->SetSelection(STATUS_ONLINE,false);
			PostMessage(ID_CHANGESTATUS,this);
		}
	
		break;
		
	case ID_NOTIFYRECEIVEDPRESENCE:
		pcMessage->FindInt32("presence",&presence);
		pcMessage->FindString("from",&from);
		m_pcViewUserList->SetPresence(JidStripResource(from),presence);
		break;
		
	case ID_NOTIFYRECEIVEDMESSAGE:
		pcMessage->FindInt32("type",&type);
//		pcMessage->FindString("id",&id);
		pcMessage->FindString("from",&from);
		pcMessage->FindString("subject",&subject);
		pcMessage->FindString("body",&body);
		wchat = ShowChatForUser(JidStripResource(from));
		cf = g_pcApplication->GetConfig();
		cf->Lock();
		if (cf->GetBoolValue(OPT_FOCUSCHAT))
			wchat->MakeFocus();
		cf->Unlock();
		wchat->ReqShowMessage(type,from,subject,body);
		break;
	case ID_DOACTION:
		status = m_pcDropdownAction->GetSelection();
		if(status == SEND_MSG)	 NewMessage();
		else if(status == ADD_CONTACT) std::cout<<"caught add_contact"<<std::endl;
		else {std::cout<<"something else"<<std::endl;}		

		break;
	case ID_CHANGESTATUS:
		status = m_pcDropdownStatus->GetSelection();
		if (status == STATUS_CHAT) type = JabberSession::PRESENCE_CHAT;
		else if (status == STATUS_ONLINE) type = JabberSession::PRESENCE_ONLINE;
		else if (status == STATUS_DND) type = JabberSession::PRESENCE_DND;
		else if (status == STATUS_AWAY) type = JabberSession::PRESENCE_AWAY;
		else if (status == STATUS_XA) type = JabberSession::PRESENCE_XA;
		else if (status == STATUS_UNAVAILABLE) {
			type = JabberSession::PRESENCE_UNAVAILABLE;
			m_pcViewUserList->SetAllUnavailable();
		}
		else {
			breakpoint("UNKNOWN PRESENCE!");
			type = JabberSession::PRESENCE_UNAVAILABLE;
		}

		g_pcApplication->ReqSendPresence(type,"");
		break;

	case ID_REQCLOSECHAT:
		pcMessage->FindPointer("ptr",(void **)&wchat);
		for (it = m_cVectWChat.begin(); it != m_cVectWChat.end(); it ++) {
			if (*it == wchat) {
				m_cVectWChat.erase(it);
				wchat->Close();
				break;
			}
		}
		break;
		
	case ID_MOPTIONS:
		if (m_pcWOptions) {	// typical situation
			m_pcWOptions->MakeFocus();
		}
		else {
			m_pcWOptions = new WOptions(this);
			m_pcWOptions->Show();
		}
		break;
		
	case ID_REQCLOSEOPTIONS:
		if (m_pcWOptions) {
			m_pcWOptions->Close();
			m_pcWOptions = NULL;
		}
		break;
		
	case ID_MSHOWLOG:
		g_pcApplication->GetLogWindow()->ReqShow();
		break;
		
	case ID_MQUIT:
		g_pcApplication->ReqQuit();
		break;
		
	case ID_BCONNECT:
		g_pcApplication->ReqConnect();
		break;
		
	default:
		Window::HandleMessage( pcMessage );
	}
}

bool WRoster::OkToQuit() {
	g_pcApplication->ReqQuit();
	return true;
}




std::string WRoster::JidStripResource(const std::string &str) {
	int pos = str.find('/');
	return pos >= 0 ? str.substr(0,pos) : str;
}







