#include "StdAfx.h"
#include <iostream>
#include "CSocketLib.h"
#include "CTcpServer.h"
#include "CTcpClient.h"

using namespace std;

CSocketLib::CSocketLib()
	: m_hThread(INVALID_HANDLE_VALUE)
	, m_socket(INVALID_SOCKET)
{
	WSADATA wsaData;
	//----------------------
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		cerr << "Initialize socket error: " << WSAGetLastError() << endl;
	}
	m_hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
}


CSocketLib::~CSocketLib()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	CloseHandle(m_hEvent);
	WSACleanup();
}

shared_ptr<CSocketLib> CSocketLib::Create(int nType)
{
	shared_ptr<CSocketLib> socket;
	switch (nType)
	{
	case TCP_CLIENT:
		socket = shared_ptr<CSocketLib>(new CTcpClient());
		break;
	case TCP_SERVER:
		socket = shared_ptr<CSocketLib>(new CTcpServer());
		break;
	default:
		break;
	}
	return socket;
}

void CSocketLib::SetParam(shared_ptr<CSocketLib::SocketParam> param)
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

bool CSocketLib::ThreadEventIsSet()
{
	return (WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0);
}

void CSocketLib::ResetThreadEvent()
{
	ResetEvent(m_hEvent);
}

void CSocketLib::SetThreadEvent()
{
	SetEvent(m_hEvent);
}
