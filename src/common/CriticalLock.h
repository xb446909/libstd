#pragma once

#include <Windows.h>

class CriticalLock
{
public:
	CriticalLock();
	~CriticalLock();
};
#ifdef WIN32
extern CRITICAL_SECTION g_csLock;
#endif
