#ifndef __SOCKET_H
#define __SOCKET_H

#define TCP_SERVER		0
#define TCP_CLIENT		1
#define UDP             2

#define RECV_ERROR		0
#define RECV_CLOSE		1
#define RECV_DATA		2
#define RECV_SOCKET		3

#define SOCK_SUCCESS	0
#define SOCK_ERROR		-1
#define SOCK_TIMEOUT	-2
#define SOCK_CLOSED		-3

typedef int(*RecvCallback)(int nType, const char* szIP, int nPort, int nSize, const char* szRecv);

int InitSocket(int nID, int nType, const char* szIniPath = 0, RecvCallback pCallback = 0);

#endif
