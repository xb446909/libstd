#pragma once
#include "socket.h"
#include <WinSock2.h>
#include <memory>
#include <string>

using namespace std::tr1;

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
	SOCKET m_socket;
	SocketParam m_param;
	HANDLE m_hThread;
	HANDLE m_hEvent;
};

typedef shared_ptr<CSocketLib::SocketParam> socket_param_ptr;