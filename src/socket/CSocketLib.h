#pragma once
#include "socket.h"
#include <memory>
#include <string>
#ifdef WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#endif

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std::tr1;
#endif

#ifndef WIN32
#define SOCKET int
#define INVALID_SOCKET -1
#endif

const int RECV_BUF_SIZE = 1024 * 1024;

class CSocketLib
{
public:
	typedef struct _tagSocketParam
	{
		int nId;
		int nType;
		std::string szIniPath;
		SocketRecvCallback callback;
		void* pParam;
		_tagSocketParam()
			: nId(0)
			, nType(0)
			, szIniPath("")
			, callback(0)
			, pParam(0)
		{
		}

		_tagSocketParam(int nId, int nType, std::string szIniPath, SocketRecvCallback callback, void* pParam)
			: nId(nId)
			, nType(nType)
			, szIniPath(szIniPath)
			, callback(callback)
			, pParam(pParam)
		{
		}

		_tagSocketParam& operator=(_tagSocketParam src)
		{
			nId = src.nId;
			nType = src.nType;
			szIniPath = src.szIniPath;
			callback = src.callback;
			pParam = src.pParam;
			return *this;
		}

	}SocketParam;

	CSocketLib();
	virtual ~CSocketLib();

	static shared_ptr<CSocketLib> Create(int nType);
	virtual void SetParam(shared_ptr<CSocketLib::SocketParam> param);
	virtual int Connect(int nTimeoutMs);
	virtual int Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort);
	virtual int Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	SOCKET GetSocket() { return m_socket; }
	SocketRecvCallback RecvCallback() { return m_param.callback; }
	void* UserParam() { return m_param.pParam; }
	bool ThreadEventIsSet();
	void ResetThreadEvent();
	void SetThreadEvent();

protected:
	SocketParam m_param;

	SOCKET m_socket;
#ifdef WIN32
	HANDLE m_hThread;
	HANDLE m_hEvent;
#endif
};

typedef shared_ptr<CSocketLib::SocketParam> socket_param_ptr;
