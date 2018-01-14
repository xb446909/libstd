#ifndef __SERIALPORT_H
#define __SERIALPORT_H

#ifdef __GNUC__
#ifndef __stdcall
#define __stdcall
#endif
#endif

#define COM_SUCCESS		(0)
#define COM_ERROR		(-1)
#define COM_ERROR_ID	(-2)

typedef int(*SerialPortRecvCallback)(const char* szPort, int nSize, const char* szRecv);

int __stdcall InitCOM(int nID, const char* szIniPath = 0, SerialPortRecvCallback pCallback = 0);
int __stdcall UninitCOM(int nID);
int __stdcall OpenCOM(int nID);
int __stdcall WriteCOM(int nID, const char* szSend, int nLen);

#endif // __SERIALPORT_H

