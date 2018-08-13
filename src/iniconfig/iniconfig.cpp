#include "stdafx.h"
#include "iniconfig.h"
#include <iostream>
#include <sstream>
#include <map>
#include <memory>
#include <string.h>
#include "iniparser.h"

#if __cplusplus >= 201103L
using namespace std;
#else
using namespace std::tr1;
#endif

std::map<int, shared_ptr<iniparser>> g_mapParser;


int __stdcall InitIniFile(int nId, const char* szFile)
{
	if (g_mapParser.find(nId) != g_mapParser.end())
	{
		g_mapParser.erase(nId);
	}
	g_mapParser[nId] = shared_ptr<iniparser>(new iniparser(szFile));
	return 0;
}

int __stdcall ReadIniInt(int nId, const char* szSection, const char* szKey, int nDefault)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return nDefault;
	}
	return g_mapParser[nId]->ReadInt(szSection, szKey, nDefault);
}

void __stdcall WriteIniInt(int nId, const char* szSection, const char* szKey, int nVal)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return;
	}
	g_mapParser[nId]->WriteInt(szSection, szKey, nVal);
}

double __stdcall ReadIniDouble(int nId, const char* szSection, const char* szKey,
		double dbDefault)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return dbDefault;
	}
	return g_mapParser[nId]->ReadDouble(szSection, szKey, dbDefault);
}

void __stdcall WriteIniDouble(int nId, const char* szSection, const char* szKey,
		double dbVal)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return;
	}
	g_mapParser[nId]->WriteDouble(szSection, szKey, dbVal);
}

std::string __stdcall ReadIniStdString(int nId, const char* szSection, const char* szKey,
		std::string strDefault)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return strDefault;
	}
	return g_mapParser[nId]->ReadString(szSection, szKey, strDefault);
}

void __stdcall WriteIniStdString(int nId, const char* szSection, const char* szKey,
		std::string str)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return;
	}
	g_mapParser[nId]->WriteString(szSection, szKey, str);
}

void __stdcall ReadIniString(int nId, const char* szSection, const char* szKey,
		const char* szDefault, char* szOut)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return;
	}
	string val = g_mapParser[nId]->ReadString(szSection, szKey, szDefault);
	strcpy(szOut, val.c_str());
}

void __stdcall WriteIniString(int nId, const char* szSection, const char* szKey,
		const char* szVal)
{
	if (g_mapParser.find(nId) == g_mapParser.end())
	{
		return;
	}
	g_mapParser[nId]->WriteString(szSection, szKey, szVal);
}

