#ifndef __CSV_MANAGER_H
#define __CSV_MANAGER_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class CSVManager
{
public:
	CSVManager();
	~CSVManager();
	int Open(const char* szPath, int nFlag);
	std::vector<std::vector<std::string>> GetData();
	int GetData(int nRow, int nCol, string& str);
	int WriteData(std::vector<std::vector<std::string>> data);
private:
	int LoadFile();

	fstream m_fs;
	std::vector<std::vector<std::string>> m_vecData;
};

#endif // !__CSV_MANAGER_H
