#include <atheos/threads.h>
#include <util/looper.h>
#include <util/locker.h>
#include <util/message.h>
#include <poll.h>
#include "poller.h"
#include "socket.h"
#include "error.h"


using namespace os;


Poller::Poller(int nMaxSockets, Looper *pcLooper, int nMessageCode)
        :Looper("poller",false) {
	// FIXME: test oom
	m_pcLooper = pcLooper;
	m_nMessageCode = nMessageCode;
			
	m_pcPollFDs = new pollfd[nMaxSockets];
	m_ppcSockets = new Socket *[nMaxSockets];
	m_nSocketCount = 0;
	m_nMaxSocketCount = nMaxSockets;
}

Poller::~Poller() {
	if (m_ppcSockets) delete[] m_ppcSockets;
	if (m_pcPollFDs) delete[] m_pcPollFDs;
}



void Poller::ClearSockets() {
	Message msg(MSG_CLEAR);
	PostMessage(&msg);
}

void Poller::AddSocket(Socket *pcSocket) {
	Message msg(MSG_ADD);
	msg.AddPointer("socket",*(void **)&pcSocket);
	PostMessage(&msg);
}

void Poller::RemoveSocket(Socket *pcSocket) {
	Message msg(MSG_REMOVE);
	msg.AddPointer("socket",*(void **)pcSocket);
	PostMessage(&msg);
}

void Poller::Poll() {
	Message msg(MSG_POLL);
	PostMessage(&msg);
}

/* Thread entry func */
void Poller::HandleMessage(Message *pcMessage) {
	
	switch (pcMessage->GetCode()) {

	case MSG_CLEAR:
		m_nSocketCount = 0;
		break;
		
	case MSG_ADD:
		{
			Socket *pcSocket;
			
			pcMessage->FindPointer("socket", (void **)&pcSocket);
			
			if (m_nSocketCount != m_nMaxSocketCount) {
				m_ppcSockets[m_nSocketCount] = pcSocket;
				m_nSocketCount ++;
			}
		}
		break;
		
	case MSG_REMOVE:
		{
			Socket *pcSocket;
			int i;
			
			pcMessage->FindPointer("socket", (void **)&pcSocket);
			
			for (i = 0; i < m_nSocketCount; i ++) {
				if (m_ppcSockets[i] == pcSocket) {
					if (i < m_nSocketCount-1)
						m_ppcSockets[i] = m_ppcSockets[m_nSocketCount-1];
					m_nSocketCount--;
					break;
				}
			}
		}
		break;
		
	case MSG_POLL:
		{
			int i,ev;
			int32 actions;
			
			for (i = 0; i < m_nSocketCount; i ++) {
				m_pcPollFDs[i].fd = m_ppcSockets[i]->m_nSock;
				m_pcPollFDs[i].events = POLLIN|POLLPRI;
			}
			
			// FIXME: nebude se chovat slusne pri vic socketech
			poll(m_pcPollFDs, m_nSocketCount, -1);
			for (i = 0; i < m_nSocketCount; i ++) {
				ev = m_pcPollFDs[i].revents;
				actions = 0;
				if (ev & POLLIN)
					actions |= READ;
				if (ev & POLLHUP)
					actions |= CLOSED;
				if (ev & POLLERR)
					actions |= ERROR;
				
				if (actions != 0) {
					Message msg(m_nMessageCode);
					msg.AddPointer("socket",*(void **)&m_ppcSockets[i]);
					msg.AddInt32("actions",actions);
					m_pcLooper->PostMessage(&msg,m_pcLooper);
				}
			}
		}
		break;
		
	default:
		Looper::HandleMessage(pcMessage);
	}
}




