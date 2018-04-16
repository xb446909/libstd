#include "StdAfx.h"
#include "CTcpClient.h"
#include <Winsock2.h>
#include <iostream>
#include <sstream>
#include "iniconfig.h"

using namespace std;

CTcpClient::CTcpClient()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		cerr << "Create socket error: " << WSAGetLastError() << endl;
		WSACleanup();
	}
}


CTcpClient::~CTcpClient()
{
	if (m_socket != INVALID_SOCKET)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
	}
	WSACleanup();
}

int CTcpClient::Connect(int nTimeoutMs)
{
	std::stringstream section;
	section << "TcpClient" << m_param.nId;
	string strIp = ReadIniStdString(section.str().c_str(), "Address", "127.0.0.1", m_param.szIniPath.c_str());
	int nPort = ReadIniInt(section.str().c_str(), "Port", 10000, m_param.szIniPath.c_str());

	struct sockaddr_in clientService;

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(strIp.c_str());
	clientService.sin_port = htons(nPort);

	u_long iMode = 1;
	ioctlsocket(m_socket, FIONBIO, &iMode);

	//----------------------
	// Connect to server.
	connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService));

	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket, &r);
	struct timeval timeout;
	timeout.tv_sec = nTimeoutMs / 1000;
	timeout.tv_usec = (nTimeoutMs % 1000) * 1000;
	int ret = select(0, 0, &r, 0, &timeout);

	if (ret <= 0)
	{
		cerr << "Connect server timeout!" << endl;
		closesocket(m_socket);
		WSACleanup();
		return SOCK_TIMEOUT;
	}
	return SOCK_SUCCESS;
}

int CTcpClient::Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort)
{
	int iResult = send(m_socket, szSendBuf, nLen, 0);
	if (iResult == SOCKET_ERROR)
	{
		cerr << "Send error: " << WSAGetLastError() << endl;
		iResult = SOCK_ERROR;
	}
	return iResult;
}

int CTcpClient::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket, &r);

	struct timeval timeout;
	timeout.tv_sec = nTimeoutMs / 1000;
	timeout.tv_usec = (nTimeoutMs % 1000) * 1000;
	int ret = select(0, &r, 0, 0, &timeout);

	int nRet = 0;

	if (ret <= 0)
	{
		cerr << "Receive timeout!" << endl;
		return SOCK_TIMEOUT;
	}
	nRet = recv(m_socket, szRecvBuf, nBufLen, 0);
	if (nRet == SOCKET_ERROR)
	{
		nRet = SOCK_ERROR;
	}
	else if (nRet == 0) 
	{
		nRet = SOCK_CLOSED;
	}
	return nRet;
}
