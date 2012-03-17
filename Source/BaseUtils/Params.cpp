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
					m_Values[key] = val;
				else
					m_Values[key] = "";

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

int Params::GetInt(const char* name, int defaultValue)
{
	if (m_Values.find(name) != m_Values.end())
		return atoi(m_Values[name].c_str());
	else
		return defaultValue;
}
		
const char* Params::GetString(const char* name, const char* defaultStr)
{
	if (m_Values.find(name) != m_Values.end())
		return m_Values[name].c_str();
	else
		return defaultStr;
}