#include "stdafx.h"
#include "CTcpClient.h"
#include <iostream>
#include <sstream>
#include "iniconfig.h"
#ifdef WIN32
#include <Winsock2.h>
#endif

using namespace std;

#ifdef WIN32
DWORD WINAPI TCPClientReceiveThread(LPVOID lpParam);
HANDLE g_hEventClientThread;
#endif

char g_szClientRecvBuf[RECV_BUF_SIZE];


CTcpClient::CTcpClient()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
        {
#ifdef WIN32
		cerr << "Create socket error: " << WSAGetLastError() << endl;
                WSACleanup();
#else
                cerr << "Create socket error: " << errno << endl;
#endif
	}
}


CTcpClient::~CTcpClient()
{
	if (m_socket != INVALID_SOCKET)
        {
#ifdef WIN32
		shutdown(m_socket, SD_BOTH);
                closesocket(m_socket);
#else
                shutdown(m_socket, SHUT_RDWR);
                close(m_socket);
#endif
        }
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
#ifdef WIN32
        ioctlsocket(m_socket, FIONBIO, &iMode);
#else
        ioctl(m_socket, FIONBIO, &iMode);
#endif

	//----------------------
	// Connect to server.
        connect(m_socket, (struct sockaddr*)&clientService, sizeof(clientService));

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
#ifdef WIN32
		closesocket(m_socket);
                WSACleanup();
#else
                close(m_socket);
#endif
		return SOCK_TIMEOUT;
	}

	if (RecvCallback() != nullptr)
        {
#ifdef WIN32
		g_hEventClientThread = CreateEvent(NULL, TRUE, TRUE, NULL);
                m_hThread = CreateThread(NULL, 0, TCPClientReceiveThread, this, 0, NULL);
#endif
	}

	return SOCK_SUCCESS;
}

int CTcpClient::Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort)
{
	int iResult = send(m_socket, szSendBuf, nLen, 0);
	if (iResult == SOCKET_ERROR)
        {
#ifdef WIN32
                cerr << "Send error: " << WSAGetLastError() << endl;
#else
                cerr << "Send error: " << errno << endl;
#endif
		iResult = SOCK_ERROR;
	}
	return iResult;
}

int CTcpClient::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	if (RecvCallback() != nullptr)
	{
		return SOCK_ERROR_ID;
	}

	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_socket, &r);

	struct timeval timeout;
	timeout.tv_sec = nTimeoutMs / 1000;
	timeout.tv_usec = (nTimeoutMs % 1000) * 1000;
	int ret = select(0, &r, 0, 0, &timeout);

	int nRet = 0;

	if (ret == 0)
	{
		cerr << "Receive timeout!" << endl;
		return SOCK_TIMEOUT;
	}
	else if (ret == SOCKET_ERROR)
	{
		return SOCK_ERROR;
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

#ifdef WIN32
DWORD WINAPI TCPClientReceiveThread(LPVOID lpParam)
{
	CTcpClient* pClient = static_cast<CTcpClient*>(lpParam);

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(pClient->GetSocket(), &fd);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	sockaddr_in  addrTemp;
	int iTempLen = sizeof(addrTemp);
	getpeername(pClient->GetSocket(), (sockaddr *)&addrTemp, &iTempLen);

	while (pClient->ThreadEventIsSet())
	{
		fd_set fdOld = fd;

		int iResult = select(0, &fdOld, NULL, NULL, &timeout);

		if (iResult == SOCKET_ERROR)
		{
			cerr << "Failed to select socket, error: " << WSAGetLastError() << endl;
			pClient->RecvCallback()(SOCK_ERROR, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pClient->UserParam());
		}
		else if (iResult > 0)
		{
			memset(g_szClientRecvBuf, 0, RECV_BUF_SIZE);
			int iRecvSize = recv(pClient->GetSocket(), g_szClientRecvBuf, RECV_BUF_SIZE, 0);

			if (SOCKET_ERROR == iRecvSize)
			{
				pClient->RecvCallback()(RECV_ERROR, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pClient->UserParam());
				closesocket(pClient->GetSocket());
				break;
			}
			else if (0 == iRecvSize)
			{
				//客户socket关闭    
				pClient->RecvCallback()(RECV_CLOSE, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pClient->UserParam());
				closesocket(pClient->GetSocket());
				break;
			}
			else if (0 < iRecvSize)
			{
				//打印接收的数据    
				pClient->RecvCallback()(RECV_DATA, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), iRecvSize, g_szClientRecvBuf, pClient->UserParam());
			}
		}
	}
	return 0;
}
#endif
