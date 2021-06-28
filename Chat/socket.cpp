#include <util/message.h>
#include <atheos/time.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "socket.h"
#include "error.h"


using namespace os;

void Socket::Error(int nType, const char *fmt, ...) {
	if (m_pcLooper) {
		Message *m = new Message(m_nMsgID);
		char buf[1024];
		va_list l;
		
		va_start(l,fmt);
		vsnprintf(buf,1024,fmt,l);
		va_end(l);
		
		m->AddInt32("type",nType);
		m->AddString("msg",buf);
		m_pcLooper->PostMessage(m);
	}
}


Socket::Socket(int nBufSize, os::Looper *pcLooper, int nMsgID) {
	m_nBufSize = nBufSize;
	m_nBufPos = 0;
	m_pnBuf = new char[nBufSize];
	m_bIsTimeout = false;
	m_pcLooper = pcLooper;
	m_nMsgID = nMsgID;
	if (m_pnBuf == NULL) {
		Error(INIT_ERROR,"Cannot allocate buffer");
		m_nState = ST_NOINIT;
	}
	else {
		m_nState = ST_NOCONNECT;
	}
}
	
Socket::~Socket() {
	if (m_pnBuf)
		delete[] m_pnBuf;
}


int Socket::Connect(const char *pzHost, int nPort) {
	sockaddr_in saddr;
	struct hostent *ha;
	
	// check our state
	if (m_nState != ST_NOCONNECT) {
		if (m_nState == ST_NOINIT)
			Error(BADCALL_ERROR,"Class not initialized properly");
		else
			Error(BADCALL_ERROR,"Already connected!");
		return FAIL;
	}
	
	ha = gethostbyname(pzHost);
	if (ha == NULL) {
		Error(CONNECT_ERROR,"Cannot resolve host '%s'",pzHost);
		return FAIL;
	}
	
	m_nSock = socket(PF_INET,SOCK_STREAM,0);
	if (m_nSock < 0) {
		Error(CONNECT_ERROR,"Cannot create socket, reason: %s",
					 strerror(errno));
		return FAIL;
	}

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(nPort);
	memcpy((void *)&saddr.sin_addr.s_addr,ha->h_addr_list[0],ha->h_length);
	if (connect(m_nSock,(struct sockaddr *)&saddr,sizeof(saddr)) != 0) {
		close(m_nSock);
		Error(CONNECT_ERROR,"Cannot connect to server, reason: %s",
					 strerror(errno));
		return FAIL;
	}
	
	m_nState = ST_CONNECT;
	return OK;
}

int Socket::Flush() {
	int n;
	
	if (m_nState != ST_CONNECT) {
		Error(BADCALL_ERROR, "Not connected!");
		return FAIL;
	}
	
	if (m_nBufPos == 0)
		return OK;
	
	n = WriteRaw(m_pnBuf,m_nBufPos);
	if (n != m_nBufPos) {
		memmove(m_pnBuf,m_pnBuf+n,m_nBufPos-n);
		m_nBufPos -= n;
		return FAIL;
	}
	else {
		m_nBufPos = 0;
		return OK;
	}
}


int Socket::Close() {
	if (m_nState != ST_CONNECT) {
		Error(BADCALL_ERROR, "Not connected!");
		return FAIL;
	}

	int val = Flush();
	Abort();
	return val;
}
	
int Socket::Abort() {
	if (m_nState != ST_CONNECT) {
		Error(BADCALL_ERROR, "Not connected!");
		return FAIL;
	}

	close(m_nSock);
	m_nState = ST_NOCONNECT;
	return OK;
}



int Socket::Write(const char *pzString) {
	return Write(pzString, strlen(pzString));
}

int Socket::Write(const char *pnBuf, int nSize) {
	int n;
	
	if (m_nState != ST_CONNECT) {
		Error(BADCALL_ERROR,"Not connected!");
		return 0;
	}
	
	if (m_nBufPos > 0 && m_nBufPos+nSize > m_nBufSize) {
		n = WriteRaw(m_pnBuf, m_nBufPos);
		if (n != m_nBufPos) {
			memmove(m_pnBuf,m_pnBuf+n,m_nBufPos-n);
			m_nBufPos -= n;
			return 0;
		}
		m_nBufPos = 0;
	}
	if (m_nBufPos + nSize <= m_nBufSize) {
		memcpy(m_pnBuf+m_nBufPos,pnBuf,nSize);
		m_nBufPos += nSize;
		return nSize;
	}
	
	// m_nBufPos == 0
	return WriteRaw(pnBuf,nSize);
}

int Socket::WriteRaw(const char *pnBuf, int nSize) {
	int m,n;
	const char *buf;

	n = nSize;
	buf = pnBuf;
	while (n > 0 && ((m=write(m_nSock,buf,nSize)) > 0
					 || (m==-1 && errno == EINTR))) {
		if (m>0) {
			buf += m;
			n -= m;
		}
	}
	
	if (n > 0) {
		Error(WRITE_ERROR,"Cannot write to socket, reason: %s",
					 strerror(errno));
		return nSize-n;
	}
	
	return nSize;
}



void Socket::SetTimeout(int nSec) {
	if (nSec > 0) {
		m_bIsTimeout = true;
		m_nTimeout = get_system_time()+((bigtime_t)nSec)*1000000LL;
	}
	else
		m_bIsTimeout = false;
}

void Socket::ClearTimeout() {
	m_bIsTimeout = false;
}


int Socket::ReadNB(char *pnBuf, int nSize) {
	int n;
	fcntl(m_nSock, F_SETFL, O_NONBLOCK);
	n = ReadRaw(pnBuf,nSize,false);
	fcntl(m_nSock, F_SETFL, 0);
	return n;
}

int Socket::Read(char *pnBuf, int nSize) {
	if (m_bIsTimeout)
		return ReadRawT(pnBuf,nSize,m_nTimeout);
	else
		return ReadRaw(pnBuf,nSize);
}

int Socket::ReadTBuf(char *pnBuf, int nSize, const char *pnChars,
					 int nTSize) {
	int i,n;
	char c;
	
	if (nTSize == 0) {
		Error(BADCALL_ERROR, "ReadTBuf(): NO terminating chars specified!");
		return 0;
	}

	n = 0;
	while (n < nSize) {
		if (m_bIsTimeout) {
			if (ReadRawT(&c, 1, m_nTimeout) == 0)
				return n;
		}
		else {
			if (ReadRaw(&c, 1) == 0)
				return n;
		}

		pnBuf[n++] = c;
		
		for (i = 0; i < nTSize; i ++)
			if (c == pnChars[i])
				return n;
	}
	return n;
}

int Socket::ReadTAll(char *pnBuf, int nSize, const char *pnChars, int nTSize) {
	int i,n;
	char c;
	
	if (nTSize == 0) {
		Error(BADCALL_ERROR, "ReadTAll(): NO terminating chars specified!");
		return 0;
	}

	n = 0;
	while (true) {
		if (m_bIsTimeout) {
			if (ReadRawT(&c, 1, m_nTimeout) == 0)
				return n;
		}
		else {
			if (ReadRaw(&c, 1) == 0)
				return n;
		}
		
		if (n < nSize)
			pnBuf[n] = c;
		n++;
		
		for (i = 0; i < nTSize; i ++)
			if (c == pnChars[i])
				return n;
	}
}

	

int Socket::ReadRaw(char *pnBuf, int nSize, bool bReadAll) {
	int m,n;
	char *buf;
	
	n = nSize;
	buf = pnBuf;
	while (n > 0 && ((m=read(m_nSock,buf,n)) > 0
					 || (m==-1 && errno == EINTR))) {
		if (m > 0) {
			buf += m;
			n -= m;
		}
	}
	
	if (bReadAll && n > 0) {
		Error(READ_ERROR,"Cannot read from socket, reason: %s",
				  strerror(errno));
	}
	
	return nSize-n;
}


int Socket::ReadRawT(char *pnBuf, int nSize, bigtime_t nTimeout) {
	
	bigtime_t t;
	int n;
	struct pollfd pfd;
	int val;
	
	fcntl(m_nSock, F_SETFL, O_NONBLOCK);
	
	pfd.fd = m_nSock;
	pfd.events = POLLIN|POLLPRI;
	
	n = ReadRaw(pnBuf,nSize,false);
	while(n < nSize) {
		t = nTimeout - get_system_time();
		if (t <= 0) {
			Error(TIMEOUT_ERROR,"Read timeouted!");
			break;
		}
		
		val = poll(&pfd,1,(int)((t+999LL)/1000LL));
		if (val > 0)
			n += ReadRaw(pnBuf,nSize,false);
		else if (val == 0) {
			Error(TIMEOUT_ERROR,"Read timeouted!!");
			break;
		}
		else {
			Error(READ_ERROR,"poll() error: %s",strerror(errno));
			break;
		}
			
	}
	fcntl(m_nSock, F_SETFL, 0);
	return n;
}





