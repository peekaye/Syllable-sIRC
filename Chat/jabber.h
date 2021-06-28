#ifndef __JABBER_H
#define __JABBER_H


#include <vector>
#include <map>
#include <stack>
#include <string>



struct Query {
	int type;
	int id;
};

class Tag {
public:
	/* tag name and attribute names are converted to lowercase */
	Tag(const char *_name, const char **_atts) {
		name = _name;
		for (int i = 0; _atts[i]; i += 2) {
			atts.push_back(std::string(_atts[i]));
			atts.push_back(std::string(_atts[i+1]));
		}
	}
	
	const std::string &GetName() const {
		return name;
	}
	
	// returns value of found attribute or "" if not found
	const std::string &FindAttribute(const std::string &name) const {
		int i;
		int len = atts.size();
		for (i = 0; i < len; i += 2) {
			if (atts[i] == name)
				return atts[i+1];
		}
		return empty_string;
	}
	
private:
	std::string name;
	std::vector<std::string> atts;
	static std::string empty_string;
};


class TagTree : public Tag {
public:
	TagTree(Tag &t) : Tag(t) {
		parent = NULL;
	}
	
	~TagTree() {
		int i,len;
		len = children.size();
		for (i = 0; i < len; i ++)
			delete children[i];
	}
	
	void Append(TagTree *t) {
		children.push_back(t);
		t->parent = this;
	}
	
	void AppendData(const std::string &s) {
		data.append(s);
	}
	
	const TagTree *FindChild(const std::string &name) const {
		int i,len;
		len = children.size();
		for (i = 0; i < len; i ++)
			if (children[i]->GetName() == name)
				return children[i];
		return NULL;
	}
	
	const std::string &GetData() const {
		return data;
	}
	
	TagTree *GetParent() const {
		return parent;
	}
	
private:
	std::vector<TagTree *> children;
	std::string data;
	TagTree *parent;
};



/* interface */
class JabberListener {
public:
	virtual ~JabberListener() {}
	virtual void JabOpen() {}
	virtual void JabAuthPassed() {}
	virtual void JabAuthFailed() {}
	
	virtual void JabPresence(const std::string &from, int presence) {}
	virtual void JabMessage(int type, const std::string &id, const std::string &from,
							const std::string &subject, const std::string &body) {}
	
	virtual void JabError(const char *msg) {}
};




/*
 * Class providing jabber logic, uses App object for sending/receiving,
 * intended to be run in App thread
 */


/* All strings are treated as UTF-8 Encoded (except tag names and
tag attribute names, which must be in pure ascii) */
class JabberSession {
public:
	enum {
		ST_CLOSED,
		ST_OPENING,
		ST_OPEN,
	};
	
	enum {
		PRESENCE_CHAT,
		PRESENCE_ONLINE,
		PRESENCE_DND,
		PRESENCE_AWAY,
		PRESENCE_XA,
		PRESENCE_UNAVAILABLE,
		PRESENCE_ERROR,
	};
	
	/* All public functions are for called from App */
	JabberSession(JabberListener *l);
	~JabberSession();
	
	void RecvElemStart(const char *name, const char **atts);
	void RecvElemEnd(const char *name);
	void RecvData(const char *data, int len);
	void RecvXMLError();

	void ConnectionDown();
	
	
	enum {
		MSGTYPE_NORMAL,
		MSGTYPE_CHAT,
		MSGTYPE_UNKNOWN,
	};

	void OpenStream(const char *host);
	void CloseStream();
	void Authenticate(const std::string &login, const std::string &passwd,
					  const std::string &resource);
	void SendPresence(int type, const std::string &status);
	void SendMessage(int type, const std::string &to,
					 const std::string &subject, const std::string &body);

private:
	typedef void (JabberSession::* TagAction)(TagTree *);
	void WriteXML(const char *s);
	void Write(const char *s);
	void Flush();
	
	void ReportError(const char *fmt, ...);
	
	void JTagIQ(TagTree *t);
	void JTagMessage(TagTree *t);
	void JTagPresence(TagTree *t);
	
	std::stack<Tag> m_cTagStack;
	std::map<std::string,TagAction> m_cActionMap;
	
	int m_nState;
	int m_nNextQueryID;
	
	TagTree *m_pcTagRoot;
	TagAction m_mfAction;
	
	JabberListener *m_pcListener;
	
	int m_nMsgID;
};



#endif


















