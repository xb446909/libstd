#pragma once
#include "CSocketLib.h"
class CTcpClient :
	public CSocketLib
{
public:
	CTcpClient();
	~CTcpClient();
	virtual int Connect(int nTimeoutMs);
	virtual int Send(const char* szSendBuf, int nLen, const char* szDstIP, int nDstPort);
	virtual int Receive(char * szRecvBuf, int nBufLen, int nTimeoutMs, const char * szDstIP, int nDstPort);
};

