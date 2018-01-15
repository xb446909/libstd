#include "stdafx.h"
#include "serialport.h"
#include "cserialport.h"
#include <map>

std::map<int, serialport_ptr> g_mapSerialPort;


int __stdcall InitCOM(int nID, const char * szIniPath, SerialPortRecvCallback pCallback)
{
	g_mapSerialPort.erase(nID);
	std::string szIni;
	if ((!szIniPath) || (!strcmp(szIniPath, "")))
	{
		szIni = std::string("./SerialPortConfig.ini");
	}
	else
	{
		szIni = std::string(szIniPath);
	}
	serialport_ptr serialPortPtr = boost::make_shared<CSerialPort>(nID, szIni, pCallback);
	g_mapSerialPort.insert(std::pair<int, serialport_ptr>(nID, serialPortPtr));

	return COM_SUCCESS;
}

int __stdcall UninitCOM(int nID)
{
	g_mapSerialPort.erase(nID);
	return COM_SUCCESS;
}


int __stdcall OpenCOM(int nID)
{
	if (g_mapSerialPort.find(nID) == g_mapSerialPort.end())
		return COM_ERROR_ID;
	return g_mapSerialPort[nID]->Open();
}

int __stdcall WriteCOM(int nID, const char * szSend, int nLen)
{
	if (g_mapSerialPort.find(nID) == g_mapSerialPort.end())
		return COM_ERROR_ID;
	return g_mapSerialPort[nID]->Write(szSend, nLen);
}

int __stdcall ReadCOM(int nID, char * szBuf, int nBufLen, int nTimeout)
{
	if (g_mapSerialPort.find(nID) == g_mapSerialPort.end())
		return COM_ERROR_ID;
	return g_mapSerialPort[nID]->Read(szBuf, nBufLen, nTimeout);
}
