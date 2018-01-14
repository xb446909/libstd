#include "stdafx.h"
#include "socket.h"
#include "iniconfig.h"
#include "csocket.h"
#include <map>
#include <boost/make_shared.hpp>

std::map<int, socket_param_ptr> g_mapSockets;

int __stdcall InitSocket(int nID, int nType, const char* szIniPath, SocketRecvCallback pCallback)
{
	std::string strIni;
	if ((!szIniPath) || (!strcmp(szIniPath, "")))
	{
		strIni = std::string("./StdSocket.ini");
	}
	else
	{
		strIni = std::string(szIniPath);
	}
    g_mapSockets.erase(nID);
    boost::shared_ptr<CSocket> socket = CSocket::Create(nType);
    socket_param_ptr param = boost::make_shared<SocketParam>(nID, nType, socket, strIni, pCallback);
    g_mapSockets.insert(std::pair<int, socket_param_ptr>(nID, param));
    socket->SetParam(param);

    return SOCK_SUCCESS;
}

void __stdcall UninitSocket(int nID)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return;
	g_mapSockets[nID]->socket->Close();
	g_mapSockets.erase(nID);
}

int __stdcall TCPConnect(int nID, int nTimeoutMs)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR;
	return g_mapSockets[nID]->socket->Connect(nTimeoutMs);
}

int __stdcall TCPSend(int nID, const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR;
	
	return g_mapSockets[nID]->socket->Send(szSendBuf, nlen, szDstIP, nDstPort);
}

int __stdcall TCPRecv(int nID, char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR;
	return g_mapSockets[nID]->socket->Recv(szRecvBuf, nBufLen, nTimeoutMs, szDstIP, nDstPort);
}
