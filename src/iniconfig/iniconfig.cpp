#include "stdafx.h"
#include "iniconfig.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include "iniparser.h"

int __stdcall ReadIniInt(const char* szSection, const char* szKey, int nDefault, const char* szFile)
{
	iniparser parser(szFile);
    return parser.ReadInt(szSection, szKey, nDefault);
}

void __stdcall WriteIniInt(const char* szSection, const char* szKey, int nVal, const char* szFile)
{
	iniparser parser(szFile);
	parser.WriteInt(szSection, szKey, nVal); 
}

double __stdcall ReadIniDouble(const char* szSection, const char* szKey, double dbDefault, const char* szFile)
{
	iniparser parser(szFile);
	return parser.ReadDouble(szSection, szKey, dbDefault);
}

void __stdcall WriteIniDouble(const char* szSection, const char* szKey, double dbVal, const char* szFile)
{
	iniparser parser(szFile);
	parser.WriteDouble(szSection, szKey, dbVal);
}

std::string __stdcall ReadIniStdString(const char* szSection, const char* szKey, std::string strDefault, const char* szFile)
{
	iniparser parser(szFile);
	return parser.ReadString(szSection, szKey, strDefault);
}

void __stdcall WriteIniStdString(const char* szSection, const char* szKey, std::string str, const char* szFile)
{
	iniparser parser(szFile);
	parser.WriteString(szSection, szKey, str);
}

void __stdcall ReadIniString(const char* szSection, const char* szKey, const char* szDefault, char* szOut, const char* szFile)
{
	iniparser parser(szFile);
	string val = parser.ReadString(szSection, szKey, szDefault);
	strcpy(szOut, val.c_str());
}

void __stdcall WriteIniString(const char* szSection, const char* szKey, const char* szVal, const char* szFile)
{
	iniparser parser(szFile);
	parser.WriteString(szSection, szKey, szVal);
}

