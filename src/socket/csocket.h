#ifndef __CSOCKET_H
#define __CSOCKET_H

#include "socket.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

class CSocket
{
public:
	typedef struct _tagSocketParam
	{
		int nId;
		int nType;
		boost::shared_ptr<CSocket> socket;
		std::string szIniPath;
		SocketRecvCallback callback;
		_tagSocketParam(int nId, int nType, boost::shared_ptr<CSocket> socket, std::string szIniPath, SocketRecvCallback callback)
			: nId(nId)
			, nType(nType)
			, socket(socket)
			, szIniPath(szIniPath)
			, callback(callback)
		{
		}

	}SocketParam;
	
    CSocket();
    static boost::shared_ptr<CSocket> Create(int nType);
    virtual void SetParam(boost::shared_ptr<CSocket::SocketParam>& param) { m_param = param; }
	virtual int Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort);
	virtual int Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	virtual int Connect(int nTimeoutMs);
	virtual void Close();
protected:
	boost::shared_ptr<CSocket::SocketParam> m_param;
    boost::asio::io_service m_io_service;
};

typedef boost::shared_ptr<CSocket::SocketParam> socket_param_ptr;

#endif // __CSOCKET_H
