#pragma once
#include "serialport.h"
#include <boost/shared_ptr.hpp>
#include <map>

#ifdef WIN32
#include "CSerialPortProc_win.h"
#endif // WIN32


class CSerialPort
{
public:
	CSerialPort();
	~CSerialPort();

	void RemoveProc(int nId);
	int AddProc(int nId, const char * szIniPath, SerialPortRecvCallback pCallback, void* pParam);
	int OpenProc(int nId);
	int WriteProc(int nId, const char* szBuf, int nLen);
	int ReadProc(int nId, char * szBuf, int nBufLen, int nTimeout);

private:
	std::map<int, boost::shared_ptr<CSerialPortProc> > m_serialportProc;
};

