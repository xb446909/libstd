#include "stdafx.h"
#ifdef WIN32

#include "CSerialPortProc_win.h"
#include "iniconfig.h"
#include <sstream>
#include "../common/CriticalLock.h"
#include <iostream>

DWORD WINAPI SerialPortRecvThread(__in LPVOID lpParameter);

CSerialPortProc::CSerialPortProc()
: m_hThread(INVALID_HANDLE_VALUE)
, m_hSerialPort(INVALID_HANDLE_VALUE)
{
	ZeroMemory(&m_wrOverlapped, sizeof(OVERLAPPED));
	ZeroMemory(&m_rdOverlapped, sizeof(OVERLAPPED));

	m_wrOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_rdOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_hThreadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CSerialPortProc::~CSerialPortProc()
{
	ResetEvent(m_hThreadEvent);
	if (m_procParam.recvCallback && (WaitForSingleObject(m_hThread, 1000) != WAIT_OBJECT_0))
	{
		DWORD dwExitcode;
		GetExitCodeThread(m_hThread, &dwExitcode);
		TerminateThread(m_hThread, dwExitcode);
	}
	if (m_hSerialPort != INVALID_HANDLE_VALUE)
	{
		PurgeComm(m_hSerialPort, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
		if (!CloseHandle(m_hSerialPort))
		{
			DWORD dwError = GetLastError();
		}
		m_hSerialPort = INVALID_HANDLE_VALUE;
	}
	CloseHandle(m_hThreadEvent);
	CloseHandle(m_hThread);
	CloseHandle(m_wrOverlapped.hEvent);
	CloseHandle(m_rdOverlapped.hEvent);
}

void CSerialPortProc::SetProcParam(ProcParam procParam)
{
	m_procParam = procParam;
}

int CSerialPortProc::Open()
{
	std::stringstream ssSection;
	ssSection << "SerialPort" << m_procParam.nId;

	m_szPortName = ReadIniStdString(ssSection.str().c_str(), "PortName", "COM1", m_procParam.strIniPath.c_str());
	int nBaudRate = ReadIniInt(ssSection.str().c_str(), "BaudRate", 115200, m_procParam.strIniPath.c_str());
	int nDataBits = ReadIniInt(ssSection.str().c_str(), "DataBits", 8, m_procParam.strIniPath.c_str());
	int nStopBits = ReadIniInt(ssSection.str().c_str(), "StopBits", 1, m_procParam.strIniPath.c_str());
	int nParity = ReadIniInt(ssSection.str().c_str(), "Parity", 0, m_procParam.strIniPath.c_str());
	int nFlowControl = ReadIniInt(ssSection.str().c_str(), "FlowControl", 0, m_procParam.strIniPath.c_str());

	m_hSerialPort = CreateFileA(m_szPortName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL,
			NULL);

	if (m_hSerialPort == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hSerialPort);
		DWORD dwErr = GetLastError();
		return COM_ERROR;
	}

	SetupComm(m_hSerialPort, BUF_SIZE, BUF_SIZE);
	DCB dcb;
	GetCommState(m_hSerialPort, &dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = nBaudRate;
	dcb.ByteSize = nDataBits;
	dcb.Parity = nParity;
	dcb.StopBits = (nStopBits == 2) ? 2 : 0;

	COMMTIMEOUTS TimeOuts;
	int nReadTimeout = ReadIniInt(ssSection.str().c_str(), "ReadInterval", 50, m_procParam.strIniPath.c_str());
	TimeOuts.ReadIntervalTimeout = nReadTimeout;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	TimeOuts.WriteTotalTimeoutMultiplier = 0;
	TimeOuts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(m_hSerialPort, &TimeOuts);
	SetCommState(m_hSerialPort, &dcb);
	PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_RXABORT | PURGE_TXABORT); //���һ�»�����

	if (m_procParam.recvCallback != nullptr)
	{
		SetEvent(m_hThreadEvent);
		m_hThread = CreateThread(NULL, 0, SerialPortRecvThread, this, 0, NULL);
	}

	return 0;
}

int CSerialPortProc::Write(const char * szBuf, int nLen)
{
	CriticalLock lock;

	DWORD nWritten = 0;

	DWORD error;
	if (ClearCommError(m_hSerialPort, &error, NULL) && error > 0) //�������
	PurgeComm(m_hSerialPort, PURGE_TXABORT | PURGE_TXCLEAR);

	if (!WriteFile(m_hSerialPort, szBuf, nLen, &nWritten, &m_wrOverlapped))
	{
		if (error = GetLastError() == ERROR_IO_PENDING)
		{
			while (!GetOverlappedResult(m_hSerialPort, &m_wrOverlapped, &nWritten, FALSE))
			{
				if (GetLastError() == ERROR_IO_INCOMPLETE)
				{
					continue;
				}
				else
				{
					ClearCommError(m_hSerialPort, &error, NULL);
					nWritten = COM_ERROR;
					break;
				}
			}
		}
	}

	return nWritten;
}

int CSerialPortProc::Read(char * szBuf, int nBufLen, int nTimeout)
{
	CriticalLock lock;

	int nRet = 0;
	DWORD dwError;
	DWORD dwRealRead = 0;
	ClearCommError(m_hSerialPort, &dwError, NULL);

	int nReadRet = ReadFile(m_hSerialPort, szBuf, nBufLen, &dwRealRead, &m_rdOverlapped);
	if (!nReadRet)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(m_rdOverlapped.hEvent, nTimeout) == WAIT_OBJECT_0)
			{
				if (!GetOverlappedResult(m_hSerialPort, &m_rdOverlapped, &dwRealRead, FALSE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE) //��������
					{
						std::cout << "nRet: " << nRet << std::endl;
						nRet = COM_ERROR;
					}
				}
				else
				{
					nRet = dwRealRead;
				}
			}
			else
			{
				nRet = COM_ERROR_TIMEOUT;
			}
		}
	}
	return nRet;
}

DWORD WINAPI SerialPortRecvThread(__in LPVOID lpParameter)
{
	CSerialPortProc* pProc = (CSerialPortProc*)lpParameter;
	CSerialPortProc::ProcParam param = pProc->m_procParam;

	char recvBuf[BUF_SIZE] =
	{	0};

	while (true)
	{
		if (WaitForSingleObject(pProc->m_hThreadEvent, 0) != WAIT_OBJECT_0) break;
		memset(recvBuf, 0, BUF_SIZE);
		int nRet = pProc->Read(recvBuf, BUF_SIZE, 500);
		if ((param.recvCallback) && (nRet != 0))
		{
			param.recvCallback(pProc->GetPortName().c_str(), nRet, recvBuf, param.pParam);
		}
		Sleep(10);
	}
	return 0;
}

#endif // WIN32
