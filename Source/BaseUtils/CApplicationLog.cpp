#include "BaseCore.h"


using namespace Utils;


bool CApplicationLog::Create(const std::string& Filename, bool SeparateEntries, bool ForcedFlush)
{
	if(Filename.empty()) return false;

	m_OutputFile.Destroy();
	
	
	m_bForceFlush = ForcedFlush;


	if(!m_OutputFile.Create(Filename, std::ios::out | std::ios::trunc)) return false;

	char time_date[MAX_LOG_ENTRY_LENGTH] = {0};	
	std::string tmp = "Log created successfully(";

	_strtime_s(time_date);
	tmp += time_date;
	tmp += " ";
	_strdate_s(time_date);
	tmp += time_date;
	tmp += ")";

	Open(std::ios::out | std::ios::trunc);
	m_bSeparateEntries = false;
	Append(tmp);
	m_bSeparateEntries = SeparateEntries;
	if(m_bForceFlush) Flush();
	m_OutputFile.Close();

	return true;
}


void CApplicationLog::Append(const char* values, ...)
{
	if(!values) return;
	if(!m_OutputFile.IsOpen()) return;

	va_list argList;
	char txt[MAX_LOG_ENTRY_LENGTH] = {0};

	va_start(argList, values);
		vsprintf_s(txt, values, argList);
	va_end(argList);

	std::string OutputData = txt;

	m_OutputFile.WriteData(OutputData, OutputData.length());

	if(m_bForceFlush) Flush();
}


void CApplicationLog::Append(const std::string& str)
{
	if(str.empty()) return;
	if(!m_OutputFile.IsOpen()) return;

	m_OutputFile.WriteData(str, str.length());

	if(m_bForceFlush) Flush();
}


void CApplicationLog::Close()
{
	Open();

	char time_date[MAX_LOG_ENTRY_LENGTH] = {0};	
	std::string tmp = "Log closed successfully(";

	_strtime_s(time_date);
	tmp += time_date;
	tmp += " ";
	_strdate_s(time_date);
	tmp += time_date;
	tmp += ")";


	Open(std::ios::out | std::ios::trunc);
	Append(tmp);	
	Flush();
	m_OutputFile.Close();	
}


CApplicationLog& CApplicationLog::operator<<(int v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(float v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(char v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(const std::string& v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(unsigned int v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(double v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}

CApplicationLog& CApplicationLog::operator<<(long double v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(unsigned char v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}

CApplicationLog& CApplicationLog::operator<<(const char* v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(bool v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << ((v) ? "true" : "false");
	if(m_bForceFlush) Flush();
	return *this;
}


CApplicationLog& CApplicationLog::operator<<(void* v)
{
	if(!m_OutputFile.IsOpen()) return *this;
	m_OutputFile.GetStream() << v;
	if(m_bForceFlush) Flush();
	return *this;
}
