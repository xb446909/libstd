#include "stdafx.h"
#include "csocket.h"
#include "ctcpserver.h"
#include "ctcpclient.h"

CSocket::CSocket()
{
}

boost::shared_ptr<CSocket> CSocket::Create(int nType)
{
    boost::shared_ptr<CSocket> socket;
    switch (nType)
    {
    case TCP_SERVER:
        socket = boost::make_shared<CTcpServer>();
        break;
	case TCP_CLIENT:
		socket = boost::make_shared<CTcpClient>();
		break;
    default:
        break;
    }
    return socket;
}

int CSocket::Send(const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort)
{
	return SOCK_ERROR;
}

int CSocket::Recv(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	return SOCK_ERROR;
}

int CSocket::Connect(int nTimeoutMs)
{
	return SOCK_ERROR;
}

void CSocket::Close()
{
}

