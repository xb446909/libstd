#ifndef __INI_CONFIG_H
#define __INI_CONFIG_H

#include <string>

#ifndef WIN32
#define __stdcall
#endif

int __stdcall InitIniFile(int nId, const char* szFile);

int __stdcall ReadIniInt(int nId, const char* szSection, const char* szKey, int nDefault);
void __stdcall WriteIniInt(int nId, const char* szSection, const char* szKey, int nVal);

double __stdcall ReadIniDouble(int nId, const char* szSection, const char* szKey, double dbDefault);
void __stdcall WriteIniDouble(int nId, const char* szSection, const char* szKey, double dbVal);

std::string __stdcall ReadIniStdString(int nId, const char* szSection, const char* szKey, std::string strDefault);
void __stdcall WriteIniStdString(int nId, const char* szSection, const char* szKey, std::string str);

void __stdcall ReadIniString(int nId, const char* szSection, const char* szKey, const char* szDefault, char* szOut);
void __stdcall WriteIniString(int nId, const char* szSection, const char* szKey, const char* szVal);

#endif
