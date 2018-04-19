#ifndef __CSV_H
#define __CSV_H

#include <vector>
#include <string>

#define CSV_SUCCESS					(0)
#define CSV_ERROR					(-1)
#define CSV_ERROR_FILE_NOT_EXIST	(-2)
#define CSV_ERROR_FILE_NOT_OPEN		(-3)
#define CSV_ERROR_DATA_OVERFLOW		(-4)

#define FLAG_OPEN_ONLY		(0)
#define FLAG_OPEN_CREATE	(1)

int __stdcall OpenCSVFile(const char* szPath, int nFlag);
std::vector<std::vector<std::string>> __stdcall GetAllData();
int __stdcall GetStdStringData(int nRow, int nCol, std::string& str);
int __stdcall GetDoubleData(int nRow, int nCol, double& dbVal);
int __stdcall GetIntData(int nRow, int nCol, int& nVal);
int __stdcall GetStrData(int nRow, int nCol, char* szBuf);
int __stdcall WriteAllData(std::vector<std::vector<std::string>> data);

#endif // !__CSV_H

