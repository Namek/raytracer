#include "BaseCore.h"


using namespace Utils;


////////////////////////////////////////////////////////////////////////////////////////////////////
//CTextFile members
////////////////////////////////////////////////////////////////////////////////////////////////////



void CTextFile::Destroy()
{
	m_pFile = NULL;
	m_strFilename = "";
	m_nFileSize = 0;
	m_strFileOpenMode = NULL;
	ClearError();
}


bool CTextFile::Create(const std::string& DefFilename, const char* DefOpenMode)
{
	if(DefFilename.empty())
	{
		m_strError = "CTextFile::CreateError Filename is empty.";
		return false;
	}
	if(!DefOpenMode)
	{
		m_strError = "CTextFile::CreateError DefOpenMode is empty.";
		return false;
	}

	Destroy();

	m_strFilename = DefFilename;
	m_strFileOpenMode = DefOpenMode;

	return true;
}


bool CTextFile::Open()
{		
	if(fopen_s(&m_pFile, m_strFilename.c_str(), m_strFileOpenMode))
	{
		m_strError = "CFile::OpenError fopen_s failed (" + m_strFilename + ")";
		return false;
	}
	
	m_bIsOpen = true;
	UpdateFileSize();
	
	return true;
}


void CTextFile::UpdateFileSize()
{
	if(!IsOpen()) return;

	unsigned CurrPos = ftell(m_pFile);
	fseek(m_pFile, 0, SEEK_END);
	m_nFileSize = ftell(m_pFile);
	fseek(m_pFile, CurrPos, SEEK_SET);
}


bool CTextFile::WriteData(const char* values, ...)
{
	if(!values) 
	{
		m_strError = "CTextFile::WriteDataError: There is nothing to write";
		return false;
	}
	if(!IsOpen())
	{
		m_strError = "CTextFile::WriteDataError: The file is not open";
		return false;
	}

	va_list argList;
	char txt[MAX_FILE_ENTRY_LENGTH] = {0};
	

	va_start(argList, values);
		vsprintf_s(txt, values, argList);
	va_end(argList);

	std::string OutputData = txt;

	fputs(OutputData.c_str(), m_pFile);

	return true;
}


bool CTextFile::ReadData(unsigned char* const Buffer, unsigned DataAmount)
{
	if(!IsOpen())
	{
		m_strError = "CTextFile::ReadDataError: The file is not open";
		return false;
	}
	if(!Buffer)
	{
		m_strError = "CTextFile::ReadDataError: The buffer pointer is invalid";
		return false;
	}

	fread(Buffer, sizeof(unsigned char), DataAmount, m_pFile);

	return true;
}
