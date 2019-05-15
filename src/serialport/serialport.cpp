// libstd.cpp: 定义 DLL 应用程序的导出函数。
//

#include "serialport.h"
#include "CSerialPort.h"

CSerialPort g_serialPort;

int __stdcall InitCOM(int nId, const char * szIniPath,
		SerialPortRecvCallback pCallback, void* pParam)
{
	g_serialPort.RemoveProc(nId);
	return g_serialPort.AddProc(nId, szIniPath, pCallback, pParam);
}

int __stdcall UninitCOM(int nId)
{
	g_serialPort.RemoveProc(nId);
	return COM_SUCCESS;
}

int __stdcall OpenCOM(int nId)
{
	return g_serialPort.OpenProc(nId);
}

int __stdcall WriteCOM(int nId, const char * szSend, int nLen)
{
	return g_serialPort.WriteProc(nId, szSend, nLen);
}

int __stdcall ReadCOM(int nId, char * szBuf, int nBufLen, int nTimeout)
{
	return g_serialPort.ReadProc(nId, szBuf, nBufLen, nTimeout);
}
