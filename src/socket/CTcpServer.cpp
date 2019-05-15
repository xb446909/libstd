#include "CTcpServer.h"
#include <sstream>
#include <iostream>
#include <vector>
#include "../iniconfig/iniparser.h"

using namespace std;

#ifdef WIN32
DWORD WINAPI TCPListenReceiveThread(LPVOID lpParam);
#else
void* TCPListenReceiveThread(void* lpParam);
#endif

char g_szServerRecvBuf[RECV_BUF_SIZE];

CTcpServer::CTcpServer()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		cerr << "Create socket error: " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
}

CTcpServer::~CTcpServer()
{
	ResetThreadEvent();
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
#ifdef WIN32
	if ((m_hThread != INVALID_HANDLE_VALUE) && (WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0))
	{
		DWORD dwExitcode;
		GetExitCodeThread(m_hThread, &dwExitcode);
		TerminateThread(m_hThread, dwExitcode);
	}
#endif
}

void CTcpServer::SetParam(shared_ptr<CSocketLib::SocketParam> param)
{
	CSocketLib::SetParam(param);
	int bOptVal = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &bOptVal,
			sizeof(bOptVal));
	if (BindSocket())
	{
		if (param->callback != nullptr)
		{
#ifdef WIN32
			m_hThread = CreateThread(NULL, 0, TCPListenReceiveThread, this, 0, NULL);
#else
		pthread_t tid;
		int err = pthread_create(&tid, NULL, TCPListenReceiveThread, this);
		if (err != 0)
		{
			cerr << "Create thread error!" << endl;
			return;
		}
		pthread_detach(tid);
#endif
		}
	}
}

int CTcpServer::Send(const char* szSendBuf, int nLen, const char* szDstIP,
		int nDstPort)
{
	int nIndex = FindSocket(szDstIP, nDstPort);
	if (nIndex == -1)
		return SOCK_ERROR_ID;
	int nRet = send(m_vecClients[nIndex].AcceptSocket, szSendBuf, nLen, 0);
	if (nRet == SOCKET_ERROR)
	{
		cerr << "Send error: " << WSAGetLastError() << endl;
		return SOCK_ERROR;
	}
	return nRet;
}

int CTcpServer::Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs,
		const char * szDstIP, int nDstPort)
{
	if (RecvCallback() != NULL)
		return SOCK_ERROR_ID;

	int nIndex = FindSocket(szDstIP, nDstPort);
	if (nIndex == -1)
		return SOCK_ERROR_ID;

	fd_set r;
	FD_ZERO(&r);
	FD_SET(m_vecClients[nIndex].AcceptSocket, &r);

	struct timeval timeout;
	timeout.tv_sec = nTimeoutMs / 1000;
	timeout.tv_usec = (nTimeoutMs % 1000) * 1000;
	int ret = select(m_vecClients[nIndex].AcceptSocket + 1, &r, 0, 0, &timeout);

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
#ifdef WIN32
	return ((addr1.sin_addr.S_un.S_addr == addr2.sin_addr.S_un.S_addr) &&
#else
	return ((addr1.sin_addr.s_addr == addr2.sin_addr.s_addr) &&
#endif
			(addr1.sin_family == addr2.sin_family)
			&& (addr1.sin_port == addr2.sin_port));
}

int CTcpServer::FindSocket(const char * szDstIP, int nDstPort)
{
	for (size_t i = 0; i < m_vecClients.size(); i++)
	{
#ifdef WIN32
		if ((inet_addr(szDstIP) == m_vecClients[i].addr.sin_addr.S_un.S_addr) &&
#else
		if ((inet_addr(szDstIP) == m_vecClients[i].addr.sin_addr.s_addr) &&
#endif
				(ntohs(m_vecClients[i].addr.sin_port) == nDstPort))
			return i;
	}
	return -1;
}

bool CTcpServer::BindSocket()
{
	iniparser parser(m_param.szIniPath.c_str());
	std::stringstream section;
	section << "TcpServer" << m_param.nId;
	string strIp = parser.ReadString(section.str().c_str(), "Address", "0.0.0.0");
	int nPort = parser.ReadInt(section.str().c_str(), "Port", 10000);

	struct sockaddr_in clientService;

	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(strIp.c_str());
	clientService.sin_port = htons(nPort);

	if (::bind(m_socket, (struct sockaddr*) &clientService,
			sizeof(clientService)) == SOCKET_ERROR)
	{
		cerr << "Bind socket error: " << WSAGetLastError() << endl;
		return false;
	}

	return true;
}

#ifdef WIN32
DWORD WINAPI TCPListenReceiveThread(LPVOID lpParam)
{
	CTcpServer* pServer = static_cast<CTcpServer*>(lpParam);

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(pServer->GetSocket(), &fd);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

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
	while (pServer->ThreadEventIsSet())
	{
		fd_set fdOld = fd;

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
							pServer->RecvCallback()(RECV_SOCKET, inet_ntoa(addrAccept.sin_addr), ntohs(addrAccept.sin_port), 0, NULL, pServer->UserParam());
							FD_SET(socketAccept, &fd);

							RecvS.AcceptSocket = socketAccept;
							memcpy(&RecvS.addr, &addrAccept, sizeof(sockaddr_in));
							pServer->AddClient(RecvS);
						}
					}
					else //非服务器,接收数据(因为fd是读数据集)    
					{
						memset(g_szServerRecvBuf, 0, RECV_BUF_SIZE);
						iRecvSize = recv(fd.fd_array[i], g_szServerRecvBuf, RECV_BUF_SIZE, 0);
						memset(&addrTemp, 0, sizeof(addrTemp));
						iTempLen = sizeof(addrTemp);
						getpeername(fd.fd_array[i], (sockaddr *)&addrTemp, &iTempLen);

						if (SOCKET_ERROR == iRecvSize)
						{
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_ERROR, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pServer->UserParam());
							closesocket(fd.fd_array[i]);
							FD_CLR(fd.fd_array[i], &fd);
							i--;
							continue;
						}
						else if (0 == iRecvSize)
						{
							//客户socket关闭    
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_CLOSE, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pServer->UserParam());
							closesocket(fd.fd_array[i]);
							FD_CLR(fd.fd_array[i], &fd);
							pServer->EraseClient(addrTemp);
							i--;
						}
						else if (0 < iRecvSize)
						{
							//打印接收的数据    
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_DATA, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), iRecvSize, g_szServerRecvBuf, pServer->UserParam());
						}
					}
				}
			}
		}
		else if (SOCKET_ERROR == iResult)
		{
			cerr << "Failed to select socket, error: " << WSAGetLastError() << endl;
			Sleep(100);
		}
	}

	for (size_t i = 0; i < fd.fd_count; i++)
	{
		closesocket(fd.fd_array[i]);
	}

	return SOCK_SUCCESS;
}
#else
void* TCPListenReceiveThread(void* lpParam)
{
	CTcpServer* pServer = static_cast<CTcpServer*>(lpParam);

	int maxfd;
	vector<int> vecfds;
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(pServer->GetSocket(), &fd);
	vecfds.push_back(pServer->GetSocket());
	maxfd = pServer->GetSocket();


	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 20000;

	int iResult;
	int iRecvSize;
	sockaddr_in addrAccept, addrTemp;
	socklen_t iAcceptLen = sizeof(addrAccept);
	socklen_t iTempLen = sizeof(addrTemp);

	if (listen(pServer->GetSocket(), SOMAXCONN) == SOCKET_ERROR)
	{
		return nullptr;
	}

	CTcpServer::RecvSocket RecvS;
	while (pServer->ThreadEventIsSet())
	{
		fd_set fdOld = fd;

		iResult = select(maxfd + 1, &fdOld, NULL, NULL, &timeout);
		if (0 < iResult)
		{
			for (size_t i = 0; i < vecfds.size(); i++)
			{
				if (FD_ISSET(vecfds[i], &fdOld))
				{
					//如果socket是服务器，则接收连接
					if (vecfds[i] == pServer->GetSocket())
					{
						memset(&addrAccept, 0, sizeof(addrAccept));
						SOCKET socketAccept = accept(pServer->GetSocket(), (sockaddr *)&addrAccept, &iAcceptLen);

						if (INVALID_SOCKET != socketAccept)
						{
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_SOCKET, inet_ntoa(addrAccept.sin_addr), ntohs(addrAccept.sin_port), 0, NULL, pServer->UserParam());
							FD_SET(socketAccept, &fd);
							if (socketAccept > maxfd)
							{
								maxfd = socketAccept;
							}
							vecfds.push_back(socketAccept);
							RecvS.AcceptSocket = socketAccept;
							memcpy(&RecvS.addr, &addrAccept, sizeof(sockaddr_in));
							pServer->AddClient(RecvS);
						}
					}
					else //非服务器,接收数据(因为fd是读数据集)
					{
						memset(g_szServerRecvBuf, 0, RECV_BUF_SIZE);
						iRecvSize = recv(vecfds[i], g_szServerRecvBuf, RECV_BUF_SIZE, 0);
						memset(&addrTemp, 0, sizeof(addrTemp));
						iTempLen = sizeof(addrTemp);
						getpeername(vecfds[i], (sockaddr *)&addrTemp, &iTempLen);

						if (SOCKET_ERROR == iRecvSize)
						{
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_ERROR, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pServer->UserParam());
							closesocket(vecfds[i]);
							FD_CLR(vecfds[i], &fd);
							vecfds.erase(vecfds.begin() + i);
							i--;
							continue;
						}
						else if (0 == iRecvSize)
						{
							//客户socket关闭
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_CLOSE, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), 0, NULL, pServer->UserParam());
							closesocket(vecfds[i]);
							FD_CLR(vecfds[i], &fd);
							vecfds.erase(vecfds.begin() + i);
							pServer->EraseClient(addrTemp);
							i--;
						}
						else if (0 < iRecvSize)
						{
							//打印接收的数据
							if (pServer->RecvCallback() != NULL)
							pServer->RecvCallback()(RECV_DATA, inet_ntoa(addrTemp.sin_addr), ntohs(addrTemp.sin_port), iRecvSize, g_szServerRecvBuf, pServer->UserParam());
						}
					}
				}
			}
		}
		else if (SOCKET_ERROR == iResult)
		{
			cerr << "Failed to select socket, error: " << WSAGetLastError() << endl;
			sleep(100);
		}
	}

	for (size_t i = 0; i < vecfds.size(); i++)
	{
		closesocket(vecfds[i]);
	}

	return nullptr;
}
#endif
