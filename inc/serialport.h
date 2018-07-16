#ifndef __SERIALPORT_H
#define __SERIALPORT_H

#ifndef WIN32
#define __stdcall
#endif

#define COM_SUCCESS         (0)
#define COM_ERROR           (-1)
#define COM_ERROR_ID        (-2)
#define COM_ERROR_TIMEOUT	(-3)

typedef int (*SerialPortRecvCallback)(const char* szPort, int nSize,
		const char* szRecv, void* pParam);

int __stdcall InitCOM(int nID, const char* szIniPath = 0,
		SerialPortRecvCallback pCallback = 0, void* pParam = 0);
int __stdcall UninitCOM(int nID);
int __stdcall OpenCOM(int nID);
int __stdcall WriteCOM(int nID, const char* szSend, int nLen);
int __stdcall ReadCOM(int nID, char* szBuf, int nBufLen, int nTimeout);

#endif // __SERIALPORT_H

