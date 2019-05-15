#include "iniparser.h"
#include <fstream>
#include <sstream>

iniparser::iniparser(string inipath) :
		m_szIniPath(inipath)
{
	parse(m_szIniPath);
}

iniparser::~iniparser()
{
}

void iniparser::WriteString(string section, string key, string val)
{
	m_map[section][key] = val;
	write();
}

string iniparser::ReadString(string section, string key, string defaultVal)
{
	if (m_map.find(section) == m_map.end())
	{
		WriteString(section, key, defaultVal);
		return defaultVal;
	}
	if (m_map[section].find(key) == m_map[section].end())
	{
		WriteString(section, key, defaultVal);
		return defaultVal;
	}
	return m_map[section][key];
}

void iniparser::WriteInt(string section, string key, int val)
{
	stringstream ss;
	ss << val;
	WriteString(section, key, ss.str());
}

int iniparser::ReadInt(string section, string key, int defaultVal)
{
	stringstream ss;
	ss << defaultVal;
	string val = ReadString(section, key, ss.str());
	return atoi(val.c_str());
}

void iniparser::WriteDouble(string section, string key, double val)
{
	stringstream ss;
	ss << val;
	WriteString(section, key, ss.str());
}

double iniparser::ReadDouble(string section, string key, double defaultVal)
{
	stringstream ss;
	ss << defaultVal;
	string val = ReadString(section, key, ss.str());
	return atof(val.c_str());
}

void iniparser::parse(string inipath)
{
	ifstream ifs(inipath);
	if (ifs.is_open())
	{
		string s;
		string section;
		while (getline(ifs, s))
		{
			if ((s.c_str()[0] == '[') && (s.c_str()[s.length() - 1] == ']'))
			{
				section = s.substr(1, s.length() - 2);
				continue;
			}
			size_t pos = s.find_first_of('=');
			if (pos != string::npos)
			{
				string key = s.substr(0, pos);
				string val = s.substr(pos + 1);
				trim(key);
				trim(val);
				m_map[section][key] = val;
			}
		}
		ifs.close();
	}
}

void iniparser::trim(string& str)
{
	size_t index = 0;
	if (!str.empty())
	{
		index = str.find_first_not_of(' ', 0);
		if (index > 0)
		{
			str.erase(0, index);
		}
		index = str.find_last_not_of(' ');
		if (index > 0)
		{
			str.erase(index + 1);
		}
	}
}

void iniparser::write()
{
	ofstream ofs(m_szIniPath, ios::out | ios::trunc);
	for (auto it = m_map.begin(); it != m_map.end(); it++)
	{
		ofs << '[' << it->first << ']' << endl;
		for (auto ite = it->second.begin(); ite != it->second.end(); ite++)
		{
			ofs << ite->first << "=" << ite->second << endl;
		}
	}
	ofs.close();
}
