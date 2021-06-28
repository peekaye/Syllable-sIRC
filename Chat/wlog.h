#ifndef __WLOG_H
#define __WLOG_H




#include <gui/window.h>
#include <gui/textview.h>
#include <util/message.h>


class WLog : public os::Window {
public:
	WLog();

	void ReqInsert(const char *s, int len);
	void ReqInsert(const char *s);
	virtual void HandleMessage(os::Message *pcMessage);
	virtual void ReqShow();
	virtual bool OkToQuit();

private:
	bool m_bShown;
	
	enum { ID_INSERT, ID_REQSHOW };
	os::TextView *m_pcLog;
};






#endif




