#include "StdAfx.h"
#include "CTcpServer.h"
#include "iniconfig.h"
#include <sstream>
#include <Winsock2.h>
#include <iostream>

using namespace std;

DWORD WINAPI TCPListenReceiveThread(LPVOID lpParam);
HANDLE g_hEventThread;

const int RECV_BUF_SIZE = 1024 * 1024;

char g_szRecvBuf[RECV_BUF_SIZE];

CTcpServer::CTcpServer()
	: m_hThread(INVALID_HANDLE_VALUE)
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		cerr << "Create socket error: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	g_hEventThread = CreateEvent(NULL, TRUE, TRUE, NULL);
}


CTcpServer::~CTcpServer()
{
	ResetEvent(g_hEventThread);
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	WSACleanup();

	if ((m_hThread != INVALID_HANDLE_VALUE) && (WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0))
	{
		DWORD dwExitcode;
		GetExitCodeThread(m_hThread, &dwExitcode);
		TerminateThread(m_hThread, dwExitcode);
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}
	CloseHandle(g_hEventThread);
	g_hEventThread = INVALID_HANDLE_VALUE;
}

void CTcpServer::SetParam(boost::shared_ptr<CSocketLib::SocketParam> param)
{
	CSocketLib::SetParam(param);
	BOOL bOptVal = TRUE;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&bOptVal, sizeof(bOptVal));
	if (BindSocket())
	{
		if (param->callback != nullptr)
		{
			m_hThread = CreateThread(NULL, 0, TCPListenReceiveThread, this, 0, NULL);
		}
	}
}

int CTcpServer::Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort)
{
	return 0;
}

int CTcpServer::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort)
{
	return 0;
}

void CTcpServer::EraseClient(RecvSocket client)
{
}

void CTcpServer::EraseClient(sockaddr_in client)
{
	for (size_t i = 0; i < m_vecClients.size(); i++)
	{
		if (IsSame(client, m_vecClients[i].addr))
		{
			m_vecClients.erase(m_vecClients.begin() + i);
			return;
		}
	}
}

bool CTcpServer::IsSame(sockaddr_in addr1, sockaddr_in addr2)
{
	return ((addr1.sin_addr.S_un.S_addr == addr2.sin_addr.S_un.S_addr) &&
		(addr1.sin_family == addr2.sin_family) &&
		(addr1.sin_port == addr2.sin_port));
}

bool CTcpServer::BindSocket()
{
	std::stringstream section;
	section << "TcpServer" << m_param.nId;
	string strIp = ReadIniStdString(section.str().c_str(), "Address", "0.0.0.0", m_param.szIniPath.c_str());
	int nPort = ReadIniInt(section.str().c_str(), "Port", 10000, m_param.szIniPath.c_str());

	struct sockaddr_in clientService;

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(strIp.c_str());
	clientService.sin_port = htons(nPort);

	if (::bind(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		cerr << "Bind socket error: " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}


DWORD WINAPI TCPListenReceiveThread(LPVOID lpParam)
{
	CTcpServer* pServer = static_cast<CTcpServer*>(lpParam);

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(pServer->GetSocket(), &fd);

	int iResult;
	int iRecvSize;
	sockaddr_in addrAccept, addrTemp;
	int iAcceptLen = sizeof(addrAccept);
	int iTempLen = sizeof(addrTemp);

	if (listen(pServer->GetSocket(), SOMAXCONN) == SOCKET_ERROR)
	{
		return SOCK_ERROR;
	}

	CTcpServer::RecvSocket RecvS;
	while (WaitForSingleObject(g_hEventThread, 0) == WAIT_OBJECT_0)
	{
		fd_set fdOld = fd;
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 50000;
		iResult = select(0, &fdOld, NULL, NULL, &timeout);
		if (0 < iResult)
		{
			for (size_t i = 0; i < fd.fd_count; i++)
			{
				if (FD_ISSET(fd.fd_array[i], &fdOld))
				{
					//如果socket是服务器，则接收连接  
					if (fd.fd_array[i] == pServer->GetSocket())
					{
						memset(&addrAccept, 0, sizeof(addrAccept));
						SOCKET socketAccept = accept(pServer->GetSocket(), (sockaddr *)&addrAccept, &iAcceptLen);

						if (INVALID_SOCKET != socketAccept)
						{
							if (pServer->RecvCallback() != NULL)
								pServer->RecvCallback()(RECV_SOCKET, inet_ntoa(addrAccept.sin_addr), ntohs(addrAccept.sin_port), 0, nullptr);
							FD_SET(socketAccept, &fd);

							RecvS.AcceptSocket = socketAccept;
							memcpy(&RecvS.addr, &addrAccept, sizeof(sockaddr_in));
							pServer->AddClient(RecvS);
						}
					}
					else //非服务器,接收数据(因为fd是读数据集)    
					{
						memset(g_szRecvBuf, 0, RECV_BUF_SIZE);
						iRecvSize = recv(fd.fd_array[i], g_szRecvBuf, RECV_BUF_SIZE, 0);
						memset(&addrTemp, 0, sizeof(addrTemp));
						iTempLen = sizeof(addrTemp);
						getpeername(fd.fd_array[i], (sockaddr *)&addrTemp, &iTempLen);

						if (SOCKET_ERROR == iRecvSize)
						{
							if (pServer->RecvCallback() != NULL)
								pServer->RecvCallback()(RECV_ERROR, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL);
							closesocket(fd.fd_array[i]);
							FD_CLR(fd.fd_array[i], &fd);
							i--;
							continue;
						}
						else if (0 == iRecvSize)
						{
							//客户socket关闭    
							if (pServer->RecvCallback() != NULL)
								pServer->RecvCallback()(RECV_CLOSE, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL);
							closesocket(fd.fd_array[i]);
							FD_CLR(fd.fd_array[i], &fd);
							pServer->EraseClient(addrTemp);
							i--;
						}
						else if (0 < iRecvSize)
						{
							//打印接收的数据    
							if (pServer->RecvCallback() != NULL)
								pServer->RecvCallback()(RECV_DATA, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), iRecvSize, g_szRecvBuf);
						}
					}
				}
			}
		}
		else if (SOCKET_ERROR == iResult)
		{
			WSACleanup();
			cerr << "Failed to select socket, error: " << WSAGetLastError() << endl;
			Sleep(100);
		}
	}
	return SOCK_SUCCESS;
}