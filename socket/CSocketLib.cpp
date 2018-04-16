#include "StdAfx.h"
#include <iostream>
#include "CSocketLib.h"
#include "CTcpServer.h"
#include "CTcpClient.h"

using namespace std;

CSocketLib::CSocketLib()
{
	WSADATA wsaData;
	//----------------------
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		cerr << "Initialize socket error: " << WSAGetLastError() << endl;
	}
}


CSocketLib::~CSocketLib()
{
}

boost::shared_ptr<CSocketLib> CSocketLib::Create(int nType)
{
	boost::shared_ptr<CSocketLib> socket;
	switch (nType)
	{
	case TCP_CLIENT:
		socket = boost::shared_ptr<CSocketLib>(new CTcpClient());
		break;
	case TCP_SERVER:
		socket = boost::shared_ptr<CSocketLib>(new CTcpServer());
		break;
	default:
		break;
	}
	return socket;
}

void CSocketLib::SetParam(boost::shared_ptr<CSocketLib::SocketParam> param)
{
	m_param = *param;
}

int CSocketLib::Connect(int nTimeoutMs)
{
	return SOCK_ERROR_ID;
}

int CSocketLib::Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort)
{
	return SOCK_ERROR_ID;
}

int CSocketLib::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	return SOCK_ERROR_ID;
}
