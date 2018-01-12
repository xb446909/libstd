#ifndef CSOCKET_H
#define CSOCKET_H

#include "socket.h"
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

class CSocket;

typedef struct _tagSocketParam
{
    int nId;
    int nType;
    boost::shared_ptr<CSocket> socket;
    std::string szIniPath;
    RecvCallback callback;
    _tagSocketParam(int nId, int nType, boost::shared_ptr<CSocket> socket, std::string szIniPath, RecvCallback callback)
        : nId(nId)
        , nType(nType)
        , socket(socket)
        , szIniPath(szIniPath)
        , callback(callback)
    {
    }

}SocketParam;

typedef boost::shared_ptr<SocketParam> socket_param_ptr;

class CSocket
{
public:
    CSocket();
	virtual ~CSocket();
    static boost::shared_ptr<CSocket> Create(int nType);
    virtual void SetParam(socket_param_ptr& param) { m_param = param; }
	virtual int Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort);
	virtual int Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	virtual int Connect(int nTimeoutMs);
protected:
    socket_param_ptr m_param;
    boost::asio::io_service m_io_service;
};

#endif // CSOCKET_H
