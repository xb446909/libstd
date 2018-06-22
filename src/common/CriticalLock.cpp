#include "StdAfx.h"
#include "CriticalLock.h"

CRITICAL_SECTION g_csLock;

CriticalLock::CriticalLock()
{
	EnterCriticalSection(&g_csLock);
}


CriticalLock::~CriticalLock()
{
	LeaveCriticalSection(&g_csLock);
}
