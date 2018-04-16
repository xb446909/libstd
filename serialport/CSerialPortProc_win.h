#pragma once
#include <string>
#include "serialport.h"

#define BUF_SIZE		4096

class CSerialPortProc
{
public:
	typedef struct _tagProcParam
	{
		int nId;
		std::string strIniPath;
		SerialPortRecvCallback recvCallback;

		_tagProcParam()
			: nId(0)
			, strIniPath("")
			, recvCallback(nullptr)
		{}

		_tagProcParam& operator =(_tagProcParam& src)
		{
			nId = src.nId;
			strIniPath = src.strIniPath;
			recvCallback = src.recvCallback;
			return *this;
		}
	}ProcParam;

	CSerialPortProc();
	~CSerialPortProc();
	void SetProcParam(ProcParam procParam);
	int Open();
	int Write(const char *szBuf, int nLen);
	int Read(char * szBuf, int nBufLen, int nTimeout);
	std::string GetPortName() { return m_szPortName; }

	ProcParam m_procParam;
	HANDLE m_hThreadEvent;

private:
	std::string m_szPortName;
	HANDLE m_hSerialPort;

	OVERLAPPED m_wrOverlapped;
	OVERLAPPED m_rdOverlapped;

	HANDLE m_hThread;
};

