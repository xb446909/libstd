#ifndef CSERAIL_PORT_PROC_H
#define CSERAIL_PORT_PROC_H

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
		void* pParam;

		_tagProcParam()
			: nId(0)
			, strIniPath("")
			, recvCallback(nullptr)
			, pParam(nullptr)
		{}

		_tagProcParam& operator =(_tagProcParam& src)
		{
			nId = src.nId;
			strIniPath = src.strIniPath;
			recvCallback = src.recvCallback;
			pParam = src.pParam;
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

private:
	std::string m_szPortName;
};


#endif /* CSERAIL_PORT_PROC_H */
