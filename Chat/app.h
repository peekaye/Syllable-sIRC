#ifndef __APP_H
#define __APP_H


#include <util/application.h>
#include <util/message.h>
#include <gui/textview.h>
#include "expat/expat.h"
#include "configfile.h"
#include "jabber.h"
#include "wlog.h"


class WRoster;
class WLog;
class Socket;
class Poller;
class App;


extern App *g_pcApplication;


class App : public os::Application, public JabberListener {
public:
	enum {
		ST_DISCONNECTED,
		ST_HANDSHAKE,
		ST_AUTHENTICATING,
		ST_AUTHENTICATED,
	};



	App();
	
	void Init();

	
	void ReqConnect();
	
	void ReqSendPlain(const std::string &data);
	void ReqSendMessage(int type, const std::string &to, const std::string &subject,
						const std::string &body);
	void ReqSendPresence(int type, const os::String &status);
	
	void ReqQuit();
	
	virtual void HandleMessage(os::Message *pcMessage);


	WLog *GetLogWindow() { return m_pcWLog; }
	ConfigFile *GetConfig() { return m_pcConfig; }
	
	
	// JabberListener implementation
	virtual void JabOpen();
	virtual void JabAuthPassed();
	virtual void JabAuthFailed();
	virtual void JabMessage(int type, const std::string &id, const std::string &from,
							const std::string &subject, const std::string &body);
	virtual void JabPresence(const std::string &from, int presence);
	virtual void JabError(const char *);

	
	
	// called from JabberSession
	void Write(const char *cstr);
	void Write(const char *buf, int len);
	void Flush();
	
	
	
private:
	enum {
		ID_POLL,
		ID_REQCONNECT,
		ID_REQSENDPRESENCE,
		ID_REQSENDPLAIN,
		ID_REQSENDMESSAGE,
	};
	


	void SetState(int nNewState);

	

	static int m_depth;

	static void st_XMLStartElement(void *pApp, const XML_Char *name,
								   const XML_Char **atts) {
		WLog *log = g_pcApplication->m_pcWLog;
		
		for(int i = 0; i < m_depth; i ++)
			log->ReqInsert("    ");
		m_depth++;
	
		log->ReqInsert("<");
		log->ReqInsert(name);
		for (int i = 0; atts[i]; i += 2) {
			log->ReqInsert(" ");
			log->ReqInsert(atts[i]);
			log->ReqInsert("=\"");
			log->ReqInsert(atts[i+1]);
			log->ReqInsert("\"");
		}
		log->ReqInsert(">\n");
		
		
		
		g_pcApplication->m_pcJabber->RecvElemStart(name, atts);
	}
	
	static void st_XMLEndElement(void *pApp, const XML_Char *name) {
		WLog *log = g_pcApplication->m_pcWLog;

		m_depth--;
		for(int i = 0; i < m_depth; i ++)
			log->ReqInsert("    ");
	
		log->ReqInsert("</");
		log->ReqInsert(name);
		log->ReqInsert(">\n");
		
		
		
		g_pcApplication->m_pcJabber->RecvElemEnd(name);
	}
	
	static void st_XMLCData(void *pApp, const XML_Char *s, int len) {
		WLog *log = g_pcApplication->m_pcWLog;
		
		log->ReqInsert(s, len);
		log->ReqInsert("\n");
		
		
		
		g_pcApplication->m_pcJabber->RecvData(s,len);
	}
	

	XML_Parser m_cXMLParser;

	WRoster *m_pcWRoster;
	WLog *m_pcWLog;
	
	Socket *m_pcSocket;
	Poller *m_pcPoller;
	
	JabberSession *m_pcJabber;
	ConfigFile *m_pcConfig;
	
	int m_nState;
};







#endif

















