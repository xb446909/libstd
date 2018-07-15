#include "stdafx.h"
#include "CriticalLock.h"
#ifndef  WIN32
#include <pthread.h>
#endif // ! WIN32

#ifdef WIN32
CRITICAL_SECTION g_csLock;
#else
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif // WIN32

CriticalLock::CriticalLock()
{
#ifdef WIN32
	EnterCriticalSection(&g_csLock);
#else
    pthread_mutex_lock(&mutex);
#endif
}


CriticalLock::~CriticalLock()
{
#ifdef WIN32
	LeaveCriticalSection(&g_csLock);
#else
    pthread_mutex_unlock(&mutex);
#endif
}
