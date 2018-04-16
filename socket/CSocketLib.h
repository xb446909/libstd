#pragma once
#include "socket.h"
#include <boost/make_shared.hpp>
#include <WinSock2.h>

class CSocketLib
{
public:
	typedef struct _tagSocketParam
	{
		int nId;
		int nType;
		std::string szIniPath;
		SocketRecvCallback callback;
		_tagSocketParam()
			: nId(0)
			, nType(0)
			, szIniPath("")
			, callback(0)
		{
		}

		_tagSocketParam(int nId, int nType, std::string szIniPath, SocketRecvCallback callback)
			: nId(nId)
			, nType(nType)
			, szIniPath(szIniPath)
			, callback(callback)
		{
		}

		_tagSocketParam& operator=(_tagSocketParam src)
		{
			nId = src.nId;
			nType = src.nType;
			szIniPath = src.szIniPath;
			callback = src.callback;
			return *this;
		}

	}SocketParam;

	CSocketLib();
	virtual ~CSocketLib();

	static boost::shared_ptr<CSocketLib> Create(int nType);
	virtual void SetParam(boost::shared_ptr<CSocketLib::SocketParam> param);
	virtual int Connect(int nTimeoutMs);
	virtual int Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort);
	virtual int Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	SOCKET GetSocket() { return m_socket; }
	SocketRecvCallback RecvCallback() { return m_param.callback; }

protected:
	SOCKET m_socket;
	SocketParam m_param;
};

typedef boost::shared_ptr<CSocketLib::SocketParam> socket_param_ptr;