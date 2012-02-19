#pragma once

#include <map>
#include <string>

namespace nprt
{

class Params
{
	public: 
		Params(const char* filename);

		int GetInt(const char* name, int defaultValue = 0);
		const char* GetString(const char* name, const char* defaultStr = "");
		
	private:
		std::map<std::string, std::string> m_Values;
};

}