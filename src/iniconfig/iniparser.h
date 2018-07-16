#ifndef INIPARSER_H
#define INIPARSER_H
#include <string>
#include <vector>
#include <map>

using namespace std;

class iniparser
{
public:
	iniparser(string inipath);
	~iniparser();

	void WriteString(string section, string key, string val);
	string ReadString(string section, string key, string defaultVal);
	void WriteInt(string section, string key, int val);
	int ReadInt(string section, string key, int defaultVal);
	void WriteDouble(string section, string key, double val);
	double ReadDouble(string section, string key, double defaultVal);
private:
	void parse(string inipath);
	void trim(string& str);
	void write();

	string m_szIniPath;
	map<string, map<string, string>> m_map;
};

#endif
