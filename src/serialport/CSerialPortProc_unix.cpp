#include "CSerialPortProc_unix.h"
#include "iniconfig.h"
#include "../common/CriticalLock.h"
#include <sstream>

using namespace std;

CSerialPortProc::CSerialPortProc()
{

}

CSerialPortProc::~CSerialPortProc()
{

}

void CSerialPortProc::SetProcParam(ProcParam procParam)
{
	m_procParam = procParam;
}

int CSerialPortProc::Open()
{

}

int CSerialPortProc::Write(const char * szBuf, int nLen)
{

}

int CSerialPortProc::Read(char * szBuf, int nBufLen, int nTimeout)
{

}
