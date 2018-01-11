#include "stdafx.h"
#include "iniconfig.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string.h>

template <typename T>
T ReadVal(const char* szSection, const char* szKey, T val, const char* szFile);

template <typename T>
void WriteVal(const char* szSection, const char* szKey, T val, const char* szFile);


template <typename T>
T ReadVal(const char* szSection, const char* szKey, T def_val, const char* szFile)
{
    T val;
    try
    {
        boost::property_tree::ptree pt, tag_setting;
        boost::property_tree::ini_parser::read_ini(szFile, pt);
        tag_setting = pt.get_child(szSection);
        val = tag_setting.get<T>(szKey, def_val);
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        val = def_val;
    }
    WriteVal(szSection, szKey, val, szFile);
    return val;
}

template <typename T>
void WriteVal(const char* szSection, const char* szKey, T val, const char* szFile)
{
    boost::property_tree::ptree pt;
    std::string path(szSection);
    path += ".";
    path += szKey;
    pt.put<T>(path, val);
    boost::property_tree::ini_parser::write_ini(szFile, pt);
}

int __stdcall ReadIniInt(const char* szSection, const char* szKey, int nDefault, const char* szFile)
{
    return (int)ReadVal(szSection, szKey, nDefault, szFile);
}

void __stdcall WriteIniInt(const char* szSection, const char* szKey, int nVal, const char* szFile)
{
    WriteVal(szSection, szKey, nVal, szFile);
}

double __stdcall ReadIniDouble(const char* szSection, const char* szKey, double dbDefault, const char* szFile)
{
    return (double)ReadVal(szSection, szKey, dbDefault, szFile);
}

void __stdcall WriteIniDouble(const char* szSection, const char* szKey, double dbVal, const char* szFile)
{
    WriteVal(szSection, szKey, dbVal, szFile);
}

std::string __stdcall ReadIniStdString(const char* szSection, const char* szKey, std::string strDefault, const char* szFile)
{
    return (std::string)ReadVal(szSection, szKey, strDefault, szFile);
}

void __stdcall WriteIniStdString(const char* szSection, const char* szKey, std::string str, const char* szFile)
{
    WriteVal(szSection, szKey, str, szFile);
}

void __stdcall ReadIniString(const char* szSection, const char* szKey, char* szDefault, char* szOut, const char* szFile)
{
    std::string str = ReadVal(szSection, szKey, std::string(szDefault), szFile);
    strcpy(szOut, str.c_str());
}

void __stdcall WriteIniString(const char* szSection, const char* szKey, char* szVal, const char* szFile)
{
    WriteVal(szSection, szKey, std::string(szVal), szFile);
}

