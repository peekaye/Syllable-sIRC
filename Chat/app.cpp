#include <util/application.h>
#include <gui/requesters.h>
#include <string>
#include "expat/expat.h"
#include "app.h"
#include "wroster.h"
#include "wlog.h"
#include "socket.h"
#include "poller.h"
#include "error.h"
#include "def.h"

using namespace os;



int App::m_depth = 0;

App *g_pcApplication;



App::App() : Application("application/x-vnd.PCH-Chat")
{
}

void App::Init() {
	// It's stupid. This all can be in constructor, I know.
	// But I like the way of assigning g_pcApplication in main()
	// and g_pcApplication must point to App object already
	m_pcConfig = new ConfigFile("^/config/Maja.cfg",
								"~/config/Maja/Maja.cfg");
	m_pcConfig->SetListener(new VerboseConfigFileListener());
	m_pcConfig->AddStringOption(OPT_HOST,"");
	m_pcConfig->AddIntOption(OPT_PORT,5222);
	m_pcConfig->AddStringOption(OPT_USER,"");
	m_pcConfig->AddStringOption(OPT_PASSWD,"");
	m_pcConfig->AddStringOption(OPT_RESOURCE,"Chat");
	m_pcConfig->AddBoolOption(OPT_FOCUSCHAT,false);
	m_pcConfig->Read();
	
	m_pcSocket = new Socket();
	m_pcPoller = new Poller(1, this, ID_POLL);
	m_pcPoller->AddSocket(m_pcSocket);
	m_pcPoller->Run();
	
	m_cXMLParser = XML_ParserCreateNS(NULL,':');
	XML_SetElementHandler(m_cXMLParser, st_XMLStartElement, st_XMLEndElement);
	XML_SetCharacterDataHandler(m_cXMLParser, st_XMLCData);

	m_pcJabber = new JabberSession(this);

	m_pcWRoster = new WRoster();
	m_pcWLog = new WLog();

	m_pcWRoster->Show();
	m_pcWRoster->MakeFocus();
	m_pcWLog->Start();
}

void App::ReqQuit() {
	PostMessage(M_QUIT);
}

void App::ReqConnect() {
	
	PostMessage(ID_REQCONNECT,this);
}

void App::ReqSendPlain(const std::string &data) {
	Message msg(ID_REQSENDPLAIN);
	msg.AddString("data",data);
	PostMessage(&msg,this);
}

void App::ReqSendMessage(int type, const std::string &to, const std::string &subject,
						 const std::string &body) {
	Message msg(ID_REQSENDMESSAGE);
	msg.AddString("body",body);
	msg.AddString("subject",subject);
	msg.AddString("to",to);
	msg.AddInt32("type",type);
	PostMessage(&msg,this);
}

void App::ReqSendPresence(int type, const String &status) {
	Message msg(ID_REQSENDPRESENCE);
	msg.AddInt32("type",type);
	msg.AddString("status",status);
	PostMessage(&msg,this);
}

void App::SetState(int nNewState) {
	if (nNewState != m_nState) {
		m_nState = nNewState;
		m_pcWRoster->NotifyStateChanged(m_nState);
	}
}

void App::HandleMessage(Message *pcMessage)
{
	switch (pcMessage->GetCode()) {
	case ID_REQCONNECT:
	if (m_nState == ST_DISCONNECTED) {
			const char *host;
			int port;

			m_pcConfig->Lock();
			host = m_pcConfig->GetStringValue(OPT_HOST).c_str();
			port = m_pcConfig->GetIntValue(OPT_PORT);
			m_pcConfig->Unlock();

			if (m_pcSocket->Connect(host,port) == OK) {
				m_pcPoller->Poll();

				m_pcJabber->OpenStream(host);
				SetState(ST_HANDSHAKE);
			}
			else {
				char buf[1024];
				snprintf(buf,1024,"Cannot connect\nReason: %s",
						 strerror(errno));
				(new Alert("Error",buf, 0, "OK", NULL))->Go();
			}
		}
		break;
		
	case ID_POLL:
		{
			int32 actions;
			pcMessage->FindInt32("actions",&actions);

			if (actions&Poller::READ) {
				char buf[380];
				int n;
				n = m_pcSocket->ReadNB(buf,380);
				if (n > 0) {
					m_pcPoller->Poll();
					XML_Parse(m_cXMLParser,buf,n,0);
				}
				else if (n < 0) {
					(new Alert("Error", "Cannot read data from server",
								0,"OK",NULL))->Go();
					if (m_nState > ST_HANDSHAKE) {
						m_pcJabber->CloseStream();
					}
					m_pcSocket->Close();
					SetState(ST_DISCONNECTED);
				}
			}
			if (actions&(Poller::CLOSED|Poller::ERROR)) {
				(new Alert("Error", "Connection to server lost",
							0,"OK",NULL))->Go();
				if (m_nState > ST_HANDSHAKE) {
					m_pcJabber->CloseStream();
				}
				m_pcSocket->Close();
				SetState(ST_DISCONNECTED);
			}
		}
		break;
	
	case ID_REQSENDPRESENCE:
		{
			int32 type;
			std::string status;
			pcMessage->FindInt32("type",&type);
			pcMessage->FindString("status",&status);
			m_pcJabber->SendPresence(type,status);
		}
		break;
		
	case ID_REQSENDPLAIN:
		{
			std::string data;
		
			pcMessage->FindString("data",&data);

			Write(data.c_str());
			Flush();
		}
		break;
		
	case ID_REQSENDMESSAGE:
		{
			int32 type;
			std::string to,subject,body;
			TextView *v;
		
			pcMessage->FindPointer("textview",(void **)&v);
			pcMessage->FindString("to",&to);
			pcMessage->FindString("body",&body);
			pcMessage->FindString("subject",&subject);
			pcMessage->FindInt32("type",&type);
		
			m_pcJabber->SendMessage(type,to,subject,body);
		}
		break;
	}
}


void App::Write(const char *cstr) {
	// FIXME: errors...
	m_pcSocket->Write(cstr);
}

void App::Write(const char *buf, int len) {
	// FIXME: errors...
	m_pcSocket->Write(buf, len);
}

void App::Flush() {
	// FIXME: errors...
	m_pcSocket->Flush();
}



void App::JabOpen() {
	m_pcJabber->Authenticate(m_pcConfig->GetStringValue(OPT_USER),
							 m_pcConfig->GetStringValue(OPT_PASSWD),
							 m_pcConfig->GetStringValue(OPT_RESOURCE));
	SetState(ST_AUTHENTICATING);
}

void App::JabAuthPassed() {
	// we're about to call SetState(ST_AUTHENTICATED)
	// - this calls NotifyStateChanged in WRoster
	// - this shows the Dropdown menu showing status and selects
	//   first item, ie ONLINE
	// - this finaly causes the request to send jabber "online" presence
	//   (through App::ReqSendPresence())
	SetState(ST_AUTHENTICATED);
}

void App::JabAuthFailed() {
	(new Alert("Error","Authentication failed!",0,"OK",NULL))->Go();
	m_pcJabber->CloseStream();
	m_pcSocket->Close();
	SetState(ST_DISCONNECTED);
}

void App::JabMessage(int type, const std::string &id, const std::string &from,
					 const std::string &subject, const std::string &body) {
	m_pcWRoster->NotifyReceivedMessage(type,id,from,subject,body);
}

void App::JabPresence(const std::string &from, int presence) {
	m_pcWRoster->NotifyReceivedPresence(from,presence);
}


void App::JabError(const char *) {
}














int main() {
	g_pcApplication = new App();
	g_pcApplication->Init();
	g_pcApplication->Run();
	return 0;
}

























