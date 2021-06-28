#include <gui/requesters.h>
#include "jabber.h"
#include "app.h"
#include "def.h"


std::string Tag::empty_string;

JabberSession::JabberSession(JabberListener *l) {
	m_nState = ST_CLOSED;
	m_nNextQueryID = 1;
	m_pcListener = l;
	m_pcTagRoot = NULL;
	m_nMsgID = 1;
	
	m_cActionMap["jabber:client:iq"] = &JabberSession::JTagIQ;
	m_cActionMap["jabber:client:message"] = &JabberSession::JTagMessage;
	m_cActionMap["jabber:client:presence"] = &JabberSession::JTagPresence;
}

JabberSession::~JabberSession() {
}



void JabberSession::JTagIQ(TagTree *t) {
	if (t->FindAttribute("id") == "auth") {
		if (t->FindAttribute("type") == "result") {
			m_pcListener->JabAuthPassed();
		}
		else {
			m_pcListener->JabAuthFailed();
		}
	}
}

void JabberSession::JTagMessage(TagTree *t) {
	const std::string &stype = t->FindAttribute("type");
	std::string subject;
	std::string body;
	const TagTree *tt;
	int type;
	
	if (stype == "normal")
		type = MSGTYPE_NORMAL;
	else if (stype == "chat")
		type = MSGTYPE_CHAT;
	else
		type = MSGTYPE_UNKNOWN;
		
	
	tt = t->FindChild("jabber:client:subject");
	if (tt != NULL)
		subject = tt->GetData();
	else
		subject = "";
	
	tt = t->FindChild("jabber:client:body");
	if (tt != NULL)
		body = tt->GetData();
	else
		body = "";

	m_pcListener->JabMessage(type, t->FindAttribute("id"),
							 t->FindAttribute("from"),
							 subject, body);
}

void JabberSession::JTagPresence(TagTree *t) {
	const TagTree *tt;
	std::string type = t->FindAttribute("type");
	std::string from = t->FindAttribute("from");
	std::string show,status;
	int presence;

	if (type == "available" || type == "") {
		tt = t->FindChild("jabber:client:show");
		show = tt ? tt->GetData() : "";
			
		if (show == "chat")
			presence = PRESENCE_CHAT;
		else if (show == "dnd")
			presence = PRESENCE_DND;
		else if (show == "away")
			presence = PRESENCE_AWAY;
		else if (show == "xa")
			presence = PRESENCE_XA;
		else
			presence = PRESENCE_ONLINE;
		
		m_pcListener->JabPresence(from,presence);
	}
	else if (type == "unavailable") {
		m_pcListener->JabPresence(from,PRESENCE_UNAVAILABLE);
	}
	else if (type == "error") {
		m_pcListener->JabPresence(from,PRESENCE_ERROR);
	}
}

void JabberSession::RecvElemStart(const char *name, const char **atts)
{
	Tag t(name,atts);
	m_cTagStack.push(t);
	
	switch (m_nState) {
		
	case ST_CLOSED:
		ReportError("Received XML tag '%s' before anything sent. Huh :(",
					name);
		break;
		
	case ST_OPENING:
		if (t.GetName() == "http://etherx.jabber.org/streams:stream") {
			m_nState = ST_OPEN;
			m_pcListener->JabOpen();
		}
		else {
			ReportError("Received XML tag '%s' when waiting for "
						"initial response.", name);
		}
		break;
		
	case ST_OPEN:
		if (m_pcTagRoot == NULL) {
			std::map<std::string,TagAction>::iterator it=m_cActionMap.find(t.GetName());
			if (it != m_cActionMap.end()) {
				m_pcTagRoot = new TagTree(t);
				m_mfAction = (*it).second;
			}
		}
		else {
			TagTree *tt = new TagTree(t);
			m_pcTagRoot->Append(tt);
			m_pcTagRoot = tt;
		}
		
		break;
	}
}

void JabberSession::RecvElemEnd(const char *name) {
	const Tag &t = m_cTagStack.top();
	
	if (t.GetName() != name) {
		// error
	}
	
	if (m_pcTagRoot) {
		if (m_pcTagRoot->GetParent() == NULL) {
			(this->*m_mfAction)(m_pcTagRoot);
			delete m_pcTagRoot;
			m_pcTagRoot = NULL;
		}
		else {
			m_pcTagRoot = m_pcTagRoot->GetParent();
		}
	}
	
	m_cTagStack.pop();
}

void JabberSession::RecvData(const char *data, int len) {
	if (m_pcTagRoot != NULL)
		m_pcTagRoot->AppendData(std::string(data,len));
}

void JabberSession::RecvXMLError() {
}

void JabberSession::ConnectionDown() {
}

void JabberSession::OpenStream(const char *host) {
	if (m_nState != ST_CLOSED) {
		ReportError("Trying to OpenStream(), when state not ST_CLOSED");
		return;
	}
	Write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		  "<stream:stream to=\"");
	WriteXML(host);
	Write("\" "
		  "xmlns=\"jabber:client\" "
		  "xmlns:stream=\"http://etherx.jabber.org/streams\">");
	Flush();
	m_nState = ST_OPENING;
}

void JabberSession::CloseStream() {
	if (m_nState != ST_OPEN) {
		ReportError("Trying to CloseStream(), when state not ST_OPEN");
		return;
	}
	
	Write("</stream:stream>");
	Flush();
	m_nState = ST_CLOSED;
}

void JabberSession::Authenticate(const std::string &user, const std::string &passwd,
								 const std::string &resource) {
	if (m_nState != ST_OPEN) {
		ReportError("Trying to authenticate when no connection open");
		return;
	}
	
	Write("<iq type=\"set\" id=\"auth\">"
		  "<query xmlns=\"jabber:iq:auth\">"
		  "<username>");
	WriteXML(user.c_str());
	Write("</username><password>");
	WriteXML(passwd.c_str());
	Write("</password><resource>");
	WriteXML(resource.c_str());
	Write("</resource></query></iq>");
	Flush();
}

void JabberSession::SendPresence(int type, const std::string &status) {
	if (type == PRESENCE_UNAVAILABLE)
		Write("<presence type=\"unavailable\">");
	else {
		Write("<presence type=\"available\">");
		switch(type) {
			case PRESENCE_CHAT: Write("<show>chat</show>"); break;
			case PRESENCE_ONLINE: break;
			case PRESENCE_DND: Write("<show>dnd</show>"); break;
			case PRESENCE_AWAY: Write("<show>away</show>"); break;
			case PRESENCE_XA: Write("<show>xa</show>"); break;
			default: breakpoint("BAD PRESENCE"); break;
		}
	}
	
	Write("<status>");
	WriteXML(status.c_str());
	Write("</status>");
	Write("</presence>");
	
	Flush();
}

void JabberSession::SendMessage(int type, const std::string &to,
								const std::string &subject, const std::string &body)
{
	char buf[80];
	
	Write("<message type=\"");
	switch(type) {
	case MSGTYPE_CHAT:
		Write("chat");
		break;
		
	case MSGTYPE_NORMAL:
	default:
		Write("normal");
		break;
	}
	Write("\" id=\"");
	snprintf(buf,sizeof(buf),"MAJA_%d",m_nMsgID++);
	WriteXML(buf);
	Write("\" to=\"");
	WriteXML(to.c_str());
	Write("\">");
	if (subject != "") {
		Write("<subject>");
		WriteXML(subject.c_str());
		Write("</subject>");
	}
	Write("<body>");
	WriteXML(body.c_str());
	Write("</body></message>");
	Flush();
}




void JabberSession::WriteXML(const char *s) {
	int i,j;
	int c;
	
	i = j = 0;
	while (s[i]) {
		c = (unsigned char)s[i];
		// UTF-8 stuff
		if (c < 0x80) {
			if (c == '&' || c == '<' || c == '>' || c == '"' || c == '\'') {
				if (j < i) {
					g_pcApplication->Write(s+j,i-j);
				}
				switch (c) {
				case '&':
					Write("&amp;");
					break;
				case '<':
					Write("&lt;");
					break;
				case '>':
					Write("&gt;");
					break;
				case '"':
					Write("&quot;");
					break;
				case '\'':
					Write("&apos;");
					break;
				}
			    j = i+1;
			}
			i++;
		}
		else if (c < 0xE0) {
			i += 2;
		}
		else if (c < 0xF0) {
			i += 3;
		}
		else {
			i += 4;
		}
	}
	
	if (j < i) {
		g_pcApplication->Write(s+j,i-j);
	}
}

void JabberSession::Write(const char *s) {
	g_pcApplication->Write(s,strlen(s));
}

void JabberSession::Flush() {
	g_pcApplication->Flush();
}

void JabberSession::ReportError(const char *fmt, ...) {
	char buf[1024];
	va_list l;
	
	va_start(l,fmt);
	vsnprintf(buf,1024,fmt,l);
	va_end(l);

	m_pcListener->JabError(buf);
}











