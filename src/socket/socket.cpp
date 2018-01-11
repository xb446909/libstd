#include "stdafx.h"
#include "socket.h"
#include "iniconfig.h"
#include "csocket.h"
#include <map>
#include <boost/make_shared.hpp>

std::map<int, socket_param_ptr> g_mapSockets;

int __stdcall InitSocket(int nID, int nType, const char* szIniPath, RecvCallback pCallback)
{
    g_mapSockets.erase(nID);
    boost::shared_ptr<CSocket> socket = CSocket::Create(nType);
    socket_param_ptr param = boost::make_shared<SocketParam>(nID, nType, socket, std::string(szIniPath), pCallback);
    g_mapSockets.insert(std::pair<int, socket_param_ptr>(nID, param));
    socket->SetParam(param);
    switch (nType)
    {
        case TCP_SERVER:
            break;
        default:
            break;
    }

    return SOCK_SUCCESS;
}
