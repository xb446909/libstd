#pragma once
class CriticalLock
{
public:
	CriticalLock();
	~CriticalLock();
};

extern CRITICAL_SECTION g_csLock;