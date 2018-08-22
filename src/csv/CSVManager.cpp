#include "stdafx.h"
#include "CSVManager.h"
#include "csv.h"
#include <sstream>
#include <string.h>
#ifdef WIN32
#include "Shlwapi.h"

#pragma comment(lib, "Shlwapi.lib")
#endif

CSVManager::CSVManager()
{
}

CSVManager::~CSVManager()
{
}

int CSVManager::Open(const char * szPath, int nFlag)
{
	if (m_fs.is_open())
	{
		m_fs.close();
	}
	if (nFlag == FLAG_OPEN_CREATE)
	{
		//if (!PathFileExistsA(szPath))
		//{
		//	stringstream ss;
		//	char szDir[MAX_PATH] = { 0 };
		//	strcpy(szDir, szPath);
		//	char* r = strrchr(szDir, '\\');
		//	if (r != NULL)
		//	{
		//		ss << "mkdir " << szDir;
		//		system(ss.str().c_str());
		//	}
		//}
		m_fs.open(szPath, ios::in | ios::out | ios::trunc);
		return CSV_SUCCESS;
	}
	else if (nFlag == FLAG_OPEN_ONLY)
	{
		m_fs.open(szPath, ios::in | ios::out | ios::app);
		if (!m_fs)
		{
			return CSV_ERROR_FILE_NOT_EXIST;
		}
		else
		{
			return LoadFile();
		}
	}
	return CSV_ERROR;
}

std::vector<std::vector<std::string>> CSVManager::GetData()
{
	return m_vecData;
}

int CSVManager::GetData(int nRow, int nCol, string & str)
{
	if (nRow >= m_vecData.size())
		return CSV_ERROR_DATA_OVERFLOW;
	if (nCol >= m_vecData[nRow].size())
		return CSV_ERROR_DATA_OVERFLOW;
	str = m_vecData[nRow][nCol];
	return CSV_SUCCESS;
}

int CSVManager::WriteData(std::vector<std::vector<std::string>> data)
{
	if (!m_fs.is_open())
		return CSV_ERROR_FILE_NOT_OPEN;
	for (size_t i = 0; i < data.size(); i++)
	{
		for (size_t j = 0; j < data[i].size(); j++)
		{
			m_fs << data[i][j] << ",";
		}
		m_fs << "\n";
	}
	m_fs.flush();
	m_fs.close();
	return CSV_SUCCESS;
}

int CSVManager::LoadFile()
{
	const char delims[] = ",";
	m_vecData.clear();
	while (!m_fs.eof())
	{
		string strline;
		getline(m_fs, strline);
		if (strline.compare("") == 0)
			continue;

		vector<string> vecString;
		char* str = new char[strline.length() + 1];
		strcpy(str, strline.c_str());
		char* r = strtok(str, delims);
		while (r != NULL)
		{
			vecString.push_back(string(r));
			r = strtok(NULL, delims);
		}
		delete[] str;
		m_vecData.push_back(vecString);
	}
	return CSV_SUCCESS;
}
