// SocketLibrary.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "socket.h"
#include "iniconfig.h"
#include "CSocketLib.h"
#include <map>
#include <memory>

using namespace std::tr1;

std::map<int, shared_ptr<CSocketLib> > g_mapSockets;

int __stdcall InitSocket(int nID, int nType, const char* szIniPath, SocketRecvCallback pCallback, void* pParam)
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

	shared_ptr<CSocketLib> socket = CSocketLib::Create(nType);
	socket_param_ptr param = shared_ptr<CSocketLib::SocketParam>(new CSocketLib::SocketParam(nID, nType, strIni, pCallback, pParam));
	g_mapSockets.insert(std::pair<int, shared_ptr<CSocketLib> >(nID, socket));
	socket->SetParam(param);

	return SOCK_SUCCESS;
}

void __stdcall UninitSocket(int nID)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return;
	g_mapSockets.erase(nID);
}

int __stdcall TCPConnect(int nID, int nTimeoutMs)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR_ID;
	return g_mapSockets[nID]->Connect(nTimeoutMs);
}

int __stdcall TCPSend(int nID, const char * szSendBuf, int nlen, const char * szDstIP, int nDstPort)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR_ID;

	return g_mapSockets[nID]->Send(szSendBuf, nlen, szDstIP, nDstPort);
}

int __stdcall TCPRecv(int nID, char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	if (g_mapSockets.find(nID) == g_mapSockets.end())
		return SOCK_ERROR_ID;
	return g_mapSockets[nID]->Receive(szRecvBuf, nBufLen, nTimeoutMs, szDstIP, nDstPort);
}

