#include <iostream>
#include "Params.h"
#include "BaseCore.h"

using namespace nprt;
using namespace std;


Params::Params(const char* filename)
{
	ifstream file;	
	string line, key, val;
	stringstream lineStream;

	file.open(filename);
	if (file.is_open())
	{
		cout << "Params from " << filename << ":" << endl;
		while (!file.eof())
		{
			getline(file, line);
			lineStream.clear();
			lineStream.str(line);
			
			if (lineStream >> key) 
			{
				val.clear();
				if (!lineStream.eof() && lineStream >> val)
				{					
					if (val == "True" || val == "true")
						val = "1";
					else if (val == "False" || val == "false")
						val = "0";

					m_Values[key] = val;
				}
				else
				{
					m_Values[key] = "";
				}

				cout << "  " << key << " = " << "\"" << val << "\"" << endl;
				key.clear();
			}
		}
	}
	else
	{
		cout << "No params file exists.\n";
	}
	file.close();
}

int Params::GetInt(const char* name, int defaultValue) const
{
	if (m_Values.find(name) != m_Values.end())
		return atoi(m_Values[name].c_str());
	else
		return defaultValue;
}
		
const char* Params::GetString(const char* name, const char* defaultStr) const
{
	if (m_Values.find(name) != m_Values.end())
		return m_Values[name].c_str();
	else
		return defaultStr;
}

bool Params::GetBool(const char* name, bool defaultValue) const
{
	if (m_Values.find(name) != m_Values.end())
		return m_Values[name] == "1";
	else
		return defaultValue;
}

float Params::GetFloat(const char* name, float defaultValue) const
{
	if (m_Values.find(name) != m_Values.end())
		return (float)atof(m_Values[name].c_str());
	else
		return defaultValue;
}