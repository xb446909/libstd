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

/*    ����Server�Ļص�����
�������������ʹ�ã�
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

/*InitSocket ����nID �������ö˵�Ψһ��ʶ�����øÿ�ʱ���ܳ�ͻ  ������nID��ͬ����ʹ�÷�����*/
int __stdcall InitSocket(int nID, int nType, const char* szIniPath = 0, SocketRecvCallback pCallback = 0, void* pParam = 0);
void __stdcall UninitSocket(int nID);

/*TCPConnect return 0 Ϊ�ɹ�
-1 û��fInitSocket
-2 connectʧ�ܣ�������û���ҵ���ӦIP�˿ڵķ���������ʱ  */
int __stdcall TCPConnect(int nID, int nTimeoutMs);

/*TCPSend return >0 Ϊ�ɹ��������ݵĳ���
-1 û��Connect
����Ϊ�ͻ��˷������ݵ�ʱ��szDstIP��nDstPortΪnull
�ɷ��͵ĳ������ٴ���15000�����Եĳ��ȣ�����û�г��ȣ�*/
int __stdcall TCPSend(int nID, const char* szSendBuf, int nlen, const char* szDstIP = 0, int nDstPort = 0);

/*    TCPRecv  ���һ���Խ���4096
return -2 ��ʱ��1��û�����ݿ��Խ��� 2��û��connect��
>0 ����Ϊ�������ݳ���  */
int __stdcall TCPRecv(int nID, char* szRecvBuf, int nBufLen, int nTimeoutMs, const char* szDstIP = 0, int nDstPort = 0);

#endif
