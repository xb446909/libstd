#include "stdafx.h"
#include "csocket.h"
#include "ctcpserver.h"

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
    default:
        break;
    }
    return socket;
}

