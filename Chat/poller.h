#ifndef __POLLER_H
#define __POLLER_H


#include <util/looper.h>
#include <util/locker.h>
#include <poll.h>

class Socket;


/*
 * Class on top of poll(). Creates new thread and periodically calls poll()
 * and posts messages to Looper informing what's new with sockets.
 * 
 * The message sent looks like:
 *     nMessageCode socket=(Socket *)
 *                  actions=(uint32)ored_vals_of_enum_below
 */
class Poller : public os::Looper {
public:
	enum { READ=1, CLOSED=2, ERROR=4 };
	
	Poller(int nMaxSockets, os::Looper *pcLooper, int nMessageCode);
	virtual ~Poller();
	
	/* when any of following three functions is called while polling,
	it will have no efect till the next poll */
	void ClearSockets();
	void AddSocket(Socket *pcSocket);
	void RemoveSocket(Socket *pcSocket);
	
	/* should be called after event has been removed, ie. data read */
	void Poll();

	virtual void HandleMessage(os::Message *pcMessage);
	
private:
	enum { MSG_CLEAR, MSG_ADD, MSG_REMOVE, MSG_POLL };
	os::Looper *m_pcLooper;       // whom do we send messages
	
	pollfd *m_pcPollFDs;
	Socket **m_ppcSockets;
	int m_nMaxSocketCount;
	int m_nSocketCount;

	int m_nMessageCode;
};
	

#endif


