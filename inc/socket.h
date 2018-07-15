#ifndef __SOCKET_H
#define __SOCKET_H

#ifndef WIN32
#ifndef __stdcall
#define __stdcall
#endif

#define SOCKET_ERROR    -1
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

/*    用于Server的回调函数
请参照以下例子使用：
int SocketRecvCallbackFun(int nType, const char* szIP, int nPort, int nSize, const char* szRecv)
{
	int nCnt = 0;
	switch (nType)
	{
	case RECV_SOCKET:
		//LOG_DEBUG("SocketRecvCallbackFun::%s:%d RECV_SOCKET!", szIP, nPort);
	break;
	case RECV_DATA:
		//DealReciveSocket(szIP, nPort, nSize, szRecv);
	break;
	case RECV_CLOSE:
		//LOG_DEBUG("SocketRecvCallbackFun::%s:%d RECV_CLOSE!", szIP, nPort);
	break;
	case RECV_ERROR:
		//LOG_DEBUG("SocketRecvCallbackFun::%s:%d RECV_ERROR!", szIP, nPort);
	break;
	default:
	break;
}
return 0;
}*/

typedef int(*SocketRecvCallback)(int nType, const char* szIP, int nPort, int nSize, const char* szRecv, void* pParam);

/*InitSocket 参数nID ：创建该端的唯一标识符调用该库时不能冲突  （以下nID以同样的使用方法）*/
int __stdcall InitSocket(int nID, int nType, const char* szIniPath = 0, SocketRecvCallback pCallback = 0, void* pParam = 0);
void __stdcall UninitSocket(int nID);

/*TCPConnect return 0 为成功
-1 没有fInitSocket
-2 connect失败，可能是没有找到相应IP端口的服务器而超时  */
int __stdcall TCPConnect(int nID, int nTimeoutMs);

/*TCPSend return >0 为成功发送数据的长度
-1 没有Connect
当作为客户端发送数据的时候szDstIP和nDstPort为null
可发送的长度至少大于15000（测试的长度，好像没有长度）*/
int __stdcall TCPSend(int nID, const char* szSendBuf, int nlen, const char* szDstIP = 0, int nDstPort = 0);

/*    TCPRecv  最大一次性接收4096
return -2 超时（1、没有数据可以接受 2、没有connect）
>0 的数为接收数据长度  */
int __stdcall TCPRecv(int nID, char* szRecvBuf, int nBufLen, int nTimeoutMs, const char* szDstIP = 0, int nDstPort = 0);

#endif
