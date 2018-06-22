#include "stdafx.h"
#include "csv.h"
#include "CSVManager.h"

CSVManager g_csvInst;

int __stdcall OpenCSVFile(const char * szPath, int nFlag)
{
	return g_csvInst.Open(szPath, nFlag);
}

std::vector<std::vector<std::string>> __stdcall GetAllData()
{
	return g_csvInst.GetData();
}

int __stdcall GetStdStringData(int nRow, int nCol, std::string & str)
{
	return g_csvInst.GetData(nRow, nCol, str);
}

int __stdcall GetDoubleData(int nRow, int nCol, double & dbVal)
{
	std::string str;
	int nRet = g_csvInst.GetData(nRow, nCol, str);
	if (nRet == CSV_SUCCESS)
	{
		dbVal = atof(str.c_str());
	}
	return nRet;
}

int __stdcall GetIntData(int nRow, int nCol, int & nVal)
{
	std::string str;
	int nRet = g_csvInst.GetData(nRow, nCol, str);
	if (nRet == CSV_SUCCESS)
	{
		nVal = atoi(str.c_str());
	}
	return nRet;
}

int __stdcall GetStrData(int nRow, int nCol, char * szBuf)
{
	std::string str;
	int nRet = g_csvInst.GetData(nRow, nCol, str);
	if (nRet == CSV_SUCCESS)
	{
		strcpy(szBuf, str.c_str());
	}
	return nRet;
}

int __stdcall WriteAllData(std::vector<std::vector<std::string>> data)
{
	return g_csvInst.WriteData(data);
}

