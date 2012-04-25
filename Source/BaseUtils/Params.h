#pragma once

#include <map>
#include <string>

namespace nprt
{

class Params
{
	public: 
		Params(const char* filename);

		int GetInt(const char* name, int defaultValue = 0) const;
		const char* GetString(const char* name, const char* defaultStr = "") const;
		bool GetBool(const char* name, bool defaultValue = false) const;
		float GetFloat(const char* name, float defaultValue = 0) const;

	private:
		mutable std::map<std::string, std::string> m_Values;
};

}