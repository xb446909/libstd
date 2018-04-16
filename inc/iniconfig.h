#ifndef __INI_CONFIG_H
#define __INI_CONFIG_H

#include <string>

int __stdcall ReadIniInt(const char* szSection, const char* szKey, int nDefault, const char* szFile);
void __stdcall WriteIniInt(const char* szSection, const char* szKey, int nVal, const char* szFile);


double __stdcall ReadIniDouble(const char* szSection, const char* szKey, double dbDefault, const char* szFile);
void __stdcall WriteIniDouble(const char* szSection, const char* szKey, double dbVal, const char* szFile);

std::string __stdcall ReadIniStdString(const char* szSection, const char* szKey, std::string strDefault, const char* szFile);
void __stdcall WriteIniStdString(const char* szSection, const char* szKey, std::string str, const char* szFile);

void __stdcall ReadIniString(const char* szSection, const char* szKey, const char* szDefault, char* szOut, const char* szFile);
void __stdcall WriteIniString(const char* szSection, const char* szKey, const char* szVal, const char* szFile);

#endif
