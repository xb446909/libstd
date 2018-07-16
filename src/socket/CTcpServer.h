#pragma once
#include "CSocketLib.h"
#include <vector>
#include <memory>

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std;
using namespace std::tr1;
#endif

class CTcpServer: public CSocketLib
{
public:
	typedef struct _tagRecvSocket
	{
		SOCKET AcceptSocket;
		sockaddr_in addr;

		bool operator==(const _tagRecvSocket& src)
		{
			return ((AcceptSocket == src.AcceptSocket)
					&&
#ifdef WIN32
					(addr.sin_addr.S_un.S_addr == src.addr.sin_addr.S_un.S_addr) &&
#else
					(addr.sin_addr.s_addr == src.addr.sin_addr.s_addr) &&
#endif
					(addr.sin_family == src.addr.sin_family)
					&& (addr.sin_port == src.addr.sin_port));
		}
	} RecvSocket, *pRecvSocket;

	CTcpServer();
	~CTcpServer();
	virtual void SetParam(shared_ptr<CSocketLib::SocketParam> param);
	virtual int Send(const char* szSendBuf, int nLen, const char* szDstIP,
			int nDstPort);
	virtual int Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs,
			const char * szDstIP, int nDstPort);
	void AddClient(RecvSocket client)
	{
		m_vecClients.push_back(client);
	}
	void EraseClient(sockaddr_in client);

private:
	bool BindSocket();
	bool IsSame(sockaddr_in addr1, sockaddr_in addr2);
	int FindSocket(const char* szDstIP, int nDstPort);
	vector<RecvSocket> m_vecClients;
};

