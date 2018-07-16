#include <iostream>
#include <string.h>
#include <stdio.h>
#include "socket.h"

int SocketRecvCallbackFun(int nType, const char* szIP, int nPort, int nSize,
		const char* szRecv, void* pParam)
{
	int nCnt = 0;
	switch (nType)
	{
	case RECV_SOCKET:
		printf("SocketRecvCallbackFun::%s:%d RECV_SOCKET!\n", szIP, nPort);
		break;
	case RECV_DATA:
		printf("SocketRecvCallbackFun::%s:%d RECV_DATA! %s\n", szIP, nPort,
				szRecv);
		break;
	case RECV_CLOSE:
		printf("SocketRecvCallbackFun::%s:%d RECV_CLOSE!\n", szIP, nPort);
		break;
	case RECV_ERROR:
		printf("SocketRecvCallbackFun::%s:%d RECV_ERROR!\n", szIP, nPort);
		break;
	default:
		break;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	InitSocket(0, TCP_CLIENT, "./tcpclient.ini", SocketRecvCallbackFun);
	TCPConnect(0, 1000);
	TCPSend(0, "Hello", strlen("Hello") + 1);
	while (1)
	{
	}
	return 0;
}
