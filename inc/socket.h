#ifndef __SOCKET_H
#define __SOCKET_H

#ifdef __GNUC__
#ifndef __stdcall
#define __stdcall
#endif
#endif

#define TCP_SERVER		0
#define TCP_CLIENT		1
#define UDP             2

#define RECV_ERROR		0
#define RECV_CLOSE		1
#define RECV_DATA		2
#define RECV_SOCKET		3

#define SOCK_SUCCESS		0
#define SOCK_ERROR			-1
#define SOCK_TIMEOUT		-2
#define SOCK_CLOSED			-3
#define SOCK_ERROR_ID		-4

typedef int(*SocketRecvCallback)(int nType, const char* szIP, int nPort, int nSize, const char* szRecv);

int __stdcall InitSocket(int nID, int nType, const char* szIniPath = 0, SocketRecvCallback pCallback = 0);
void __stdcall UninitSocket(int nID);

int __stdcall TCPConnect(int nID, int nTimeoutMs);
int __stdcall TCPSend(int nID, const char* szSendBuf, int nlen, const char* szDstIP = 0, int nDstPort = 0);
int __stdcall TCPRecv(int nID, char* szRecvBuf, int nBufLen, int nTimeoutMs, const char* szDstIP = 0, int nDstPort = 0);

#endif
