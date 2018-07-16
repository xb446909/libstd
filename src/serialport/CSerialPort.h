#pragma once
#include "serialport.h"
#include <memory>
#include <map>

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std::tr1;
#endif

#ifdef WIN32
#include "CSerialPortProc_win.h"
#else
#include "CSerialPortProc_unix.h"
#endif // WIN32

class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();

	void RemoveProc(int nId);
	int AddProc(int nId, const char * szIniPath,
			SerialPortRecvCallback pCallback, void* pParam);
	int OpenProc(int nId);
	int WriteProc(int nId, const char* szBuf, int nLen);
	int ReadProc(int nId, char * szBuf, int nBufLen, int nTimeout);

private:
	std::map<int, shared_ptr<CSerialPortProc> > m_serialportProc;
};

