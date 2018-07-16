#include "stdafx.h"
#include <iostream>
#include "CSocketLib.h"
#include "CTcpServer.h"
#include "CTcpClient.h"

using namespace std;

CSocketLib::CSocketLib()
#ifdef WIN32
: m_hThread(INVALID_HANDLE_VALUE)
, m_socket(INVALID_SOCKET)
#endif
{
#ifdef WIN32
	WSADATA wsaData;
	//----------------------
	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		cerr << "Initialize socket error: " << WSAGetLastError() << endl;
	}
	m_hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
#else
	pthread_mutex_init(&m_mutex, NULL);
#endif
}

CSocketLib::~CSocketLib()
{
#ifdef WIN32
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	CloseHandle(m_hEvent);
#else
	pthread_mutex_destroy(&m_mutex);
#endif
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

int CSocketLib::Send(const char* szSendBuf, int nLen, const char* szDstIP,
		int nDstPort)
{
	return SOCK_ERROR_ID;
}

int CSocketLib::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs,
		const char * szDstIP, int nDstPort)
{
	return SOCK_ERROR_ID;
}

bool CSocketLib::ThreadEventIsSet()
{
#ifdef WIN32
	return (WaitForSingleObject(m_hEvent, 0) == WAIT_OBJECT_0);
#else
	int ret = pthread_mutex_trylock(&m_mutex);
	if (ret == 0)
	{
		pthread_mutex_unlock(&m_mutex);
		return false;
	}
	else
	{
		return true;
	}
#endif
}

void CSocketLib::ResetThreadEvent()
{
#ifdef WIN32
	ResetEvent(m_hEvent);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}

void CSocketLib::SetThreadEvent()
{
#ifdef WIN32
	SetEvent(m_hEvent);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}
