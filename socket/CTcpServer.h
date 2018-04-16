#pragma once
#include "CSocketLib.h"
#include <vector>

using namespace std;

class CTcpServer :
	public CSocketLib
{
public:
	typedef struct _tagRecvSocket
	{
		SOCKET      AcceptSocket;
		sockaddr_in addr;

		bool operator==(const _tagRecvSocket& src)
		{
			return ((AcceptSocket == src.AcceptSocket) &&
				(addr.sin_addr.S_un.S_addr == src.addr.sin_addr.S_un.S_addr) &&
				(addr.sin_family == src.addr.sin_family) &&
				(addr.sin_port == src.addr.sin_port));
		}
	}RecvSocket, *pRecvSocket;

	CTcpServer();
	~CTcpServer();
	virtual void SetParam(boost::shared_ptr<CSocketLib::SocketParam> param);
	virtual int Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort);
	virtual int Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
	void AddClient(RecvSocket client) { m_vecClients.push_back(client); }
	void EraseClient(RecvSocket client);
	void EraseClient(sockaddr_in client);

private:
	bool BindSocket();
	bool IsSame(sockaddr_in addr1, sockaddr_in addr2);
	HANDLE m_hThread;
	vector<RecvSocket> m_vecClients;
};

