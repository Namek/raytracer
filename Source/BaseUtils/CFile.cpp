#include "BaseCore.h"


using namespace Utils;


////////////////////////////////////////////////////////////////////////////////////////////////////
//CFile members
////////////////////////////////////////////////////////////////////////////////////////////////////


bool CFile::Create(const std::string& DefFilename, unsigned DefOpenMode)
{	
	if(!DefFilename.length())
	{
		m_strError = "CFile::CreateError: Invalid filename";
		return false;
	}
	if(!DefOpenMode)
	{
		m_strError = "CFile::CreateError: Invalid open flags";
		return false;
	}

	m_strFilename = DefFilename;
	m_nOpenFlags = DefOpenMode;	
	return true;
}


void CFile::Destroy()
{
	m_nFileSize = 0;
	m_strError = m_strFilename = "";
	m_nOpenFlags = 0;
}


bool CFile::Open()
{
	if(CheckFlagErrors()) return false;

	m_Stream.open(m_strFilename.c_str(), m_nOpenFlags);
	
	if(!IsOpen())
	{
		m_strError = "CFile::OpenError: (Filename: " + m_strFilename + ")";
		return false;		
	}
	
	UpdateFileSize();
	ClearError();

	return true;
}


void CFile::Close()
{
	m_Stream.close();
	ClearError();
}


void CFile::MoveInputPtr(unsigned Pos)
{
	if(Pos > GetFileSize()) m_Stream.seekg(GetFileSize());
	else m_Stream.seekg(Pos);
}


void CFile::MoveOutputPtr(unsigned Pos)
{
	if(Pos > GetFileSize()) m_Stream.seekp(GetFileSize());
	else m_Stream.seekp(Pos);
}


bool CFile::ReadData(char* Buffer, unsigned Size)
{
	if(CheckIOErrors(Buffer, Size)) return false;

	m_Stream.read(Buffer, Size);

	return (!IsBad());
}


bool CFile::ReadData(std::string& Buffer, unsigned Size)
{
	if(CheckIOErrors(Size)) return false;

	char InputChr = 0;
	for(unsigned i = 0; i < Size; i++)
	{
		if(!IsEoF())
		{
			m_Stream.get(InputChr);
			Buffer += InputChr;
		}
	}

	return (!IsBad());
}


bool CFile::WriteData(const char* const Buffer, unsigned Size)
{
	if(CheckIOErrors(Buffer, Size)) return false;

	m_Stream.write(Buffer, Size);

	UpdateFileSize();

	return (!IsBad());
}


bool CFile::WriteData(const std::string& Buffer, unsigned Size)
{
	if(CheckIOErrors(Size)) return false;
	
	for(unsigned i = 0; i < Size; i++)
	{
		if(!IsEoF())
		{
			m_Stream.put(Buffer[i]);			
		}
	}

	UpdateFileSize();

	return (!IsBad());
}


bool CFile::CheckIOErrors(const char* const Buffer, unsigned Size)
{
	ClearError();

	if(!Buffer) m_strError = "CFile::IOError: Buffer set to NULL";
	if(!Size) m_strError = "CFile::IOError: Buffer size set to 0";
	if(!IsOpen()) m_strError = "CFile::IOError: the file is not open";

	return (!m_strError.empty());
}


bool CFile::CheckIOErrors(unsigned Size)
{
	ClearError();
	
	if(!Size) m_strError = "CFile::IOError: Buffer size set to 0";
	if(!IsOpen()) m_strError = "CFile::IOError: the file is not open";

	return (!m_strError.empty());
}


bool CFile::CheckFlagErrors()
{
	ClearError();

	if(!m_strFilename.length())
	{
		m_strError = "CFile::OpenError: Invalid filename";
		return true;
	}
	if(!m_nOpenFlags)
	{
		m_strError = "CFile::OpenError: Invalid file open flags";
		return true;
	}

	return false;
}


