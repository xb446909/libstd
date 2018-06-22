#include "stdafx.h"
#include "iniconfig.h"
#include <iostream>
#include <sstream>
#include <string.h>

int __stdcall ReadIniInt(const char* szSection, const char* szKey, int nDefault, const char* szFile)
{
	std::stringstream ss;
	ss << nDefault;
    return atoi(ReadIniStdString(szSection, szKey, ss.str(), szFile).c_str());
}

void __stdcall WriteIniInt(const char* szSection, const char* szKey, int nVal, const char* szFile)
{
	std::stringstream ss;
	ss << nVal;
    WriteIniStdString(szSection, szKey, ss.str(), szFile);
}

double __stdcall ReadIniDouble(const char* szSection, const char* szKey, double dbDefault, const char* szFile)
{
	std::stringstream ss;
	ss << dbDefault;
	return atof(ReadIniStdString(szSection, szKey, ss.str(), szFile).c_str());
}

void __stdcall WriteIniDouble(const char* szSection, const char* szKey, double dbVal, const char* szFile)
{
	std::stringstream ss;
	ss << dbVal;
	WriteIniStdString(szSection, szKey, ss.str(), szFile);
}

std::string __stdcall ReadIniStdString(const char* szSection, const char* szKey, std::string strDefault, const char* szFile)
{
	char szTemp[MAX_PATH];
	ReadIniString(szSection, szKey, strDefault.c_str(), szTemp, szFile);
	return std::string(szTemp);
}

void __stdcall WriteIniStdString(const char* szSection, const char* szKey, std::string str, const char* szFile)
{
	WriteIniString(szSection, szKey, str.c_str(), szFile);
}

void __stdcall ReadIniString(const char* szSection, const char* szKey, const char* szDefault, char* szOut, const char* szFile)
{
	GetPrivateProfileStringA(szSection, szKey, szDefault, szOut, MAX_PATH, szFile);
	WritePrivateProfileStringA(szSection, szKey, szOut, szFile);
}

void __stdcall WriteIniString(const char* szSection, const char* szKey, const char* szVal, const char* szFile)
{
	WritePrivateProfileStringA(szSection, szKey, szVal, szFile);
}

