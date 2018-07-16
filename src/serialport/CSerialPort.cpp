#include "stdafx.h"
#include "CSerialPort.h"

CSerialPort::CSerialPort()
{
}

CSerialPort::~CSerialPort()
{
}

void CSerialPort::RemoveProc(int nId)
{
	m_serialportProc.erase(nId);
}

int CSerialPort::AddProc(int nId, const char * szIniPath,
		SerialPortRecvCallback pCallback, void* pParam)
{
	CSerialPortProc::ProcParam param;
	param.nId = nId;
	param.strIniPath = std::string(szIniPath);
	param.recvCallback = pCallback;
	param.pParam = pParam;
	shared_ptr<CSerialPortProc> proc(new CSerialPortProc());
	proc->SetProcParam(param);

	m_serialportProc.insert(
			std::pair<int, shared_ptr<CSerialPortProc>>(nId, proc));
	return 0;
}

int CSerialPort::OpenProc(int nId)
{
	if (m_serialportProc.find(nId) == m_serialportProc.end())
		return COM_ERROR_ID;
	return m_serialportProc[nId]->Open();
}

int CSerialPort::WriteProc(int nId, const char * szBuf, int nLen)
{
	if (m_serialportProc.find(nId) == m_serialportProc.end())
		return COM_ERROR_ID;
	return m_serialportProc[nId]->Write(szBuf, nLen);
}

int CSerialPort::ReadProc(int nId, char * szBuf, int nBufLen, int nTimeout)
{
	if (m_serialportProc.find(nId) == m_serialportProc.end())
		return COM_ERROR_ID;
	return m_serialportProc[nId]->Read(szBuf, nBufLen, nTimeout);
}
