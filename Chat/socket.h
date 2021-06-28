#ifndef __SOCKET_H
#define __SOCKET_H

#include <atheos/time.h>
#include <util/looper.h>


class Poller;
	
/**
 *  timeouts are in seconds
 */
class Socket {
friend class Poller;
public:
	enum { BADCALL_ERROR, INIT_ERROR, CONNECT_ERROR,
		   WRITE_ERROR, READ_ERROR, TIMEOUT_ERROR };
	
	Socket(int nBufSize=1400, os::Looper *pcLooper=NULL, int nMsgID=0);
	~Socket();
	
	int Connect(const char *pzHost, int nPort);
	int Flush();
	int Close();
	int Abort();

	/* Returns number of bytes accepted */
	int Write(const char *pzString);
	int Write(const char *pnBuf, int nSize);
	
	/* Timeout affects only Read operations */
	void SetTimeout(int nSec);
	void ClearTimeout();

	/* returns number of bytes read */
	int Read(char *pnBuf, int nSize);
	int ReadNB(char *pnBuf, int nSize); // not-blocking version
	/* Read at most nSize chars, terminate if one of pnChars were read
	 * The terminator will be stored into pnBuf too */
	int ReadTBuf(char *pnBuf, int nSize, const char *pnChars, int nTSize);
	/* Read is terminated only by one of pnChars, first nSize read bytes
	   are stored pnBuf; returns number of bytes read */
	int ReadTAll(char *pnBuf, int nSize, const char *pnChars, int nTSize);
	
	int ReadTBuf(char *pnBuf, int nSize, const char *pzChars) {
		return ReadTBuf(pnBuf,nSize,pzChars,strlen(pzChars));
	}
	
	int ReadTAll(char *pnBuf, int nSize, const char *pzChars) {
		return ReadTAll(pnBuf,nSize,pzChars,strlen(pzChars));
	}
	
	
	
private:
	enum {ST_NOINIT, ST_NOCONNECT, ST_CONNECT};

	void Error(int nType, const char *fmt, ...);

	int WriteRaw(const char *pnBuf, int nSize);
	int ReadRaw(char *pnBuf, int nSize, bool bReadAll=true);
	int ReadRawT(char *pnBuf, int nSize, bigtime_t nTimeout);
		

	int m_nState;
	int m_nSock;  // valid only in ST_CONNECT state

	char *m_pnBuf;
	int m_nBufSize;
	int m_nBufPos;
	
	bool m_bIsTimeout;
	bigtime_t m_nTimeout;
	
	os::Looper *m_pcLooper;
	int m_nMsgID;
};



#endif





