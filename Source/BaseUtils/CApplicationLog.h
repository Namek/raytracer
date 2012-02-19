#pragma once


/*!
	\file CApplicationLog.h
	\brief This file contains the definition of the CApplicationLog class.
*/

namespace Utils
{

//! The maximum log entry length.
const unsigned MAX_LOG_ENTRY_LENGTH = 512;



//! A class representing a log.
typedef class CApplicationLog
{
	public:
		/*! The default constructor. Clears the object.
			@see Destroy
		*/
		CApplicationLog() {m_OutputFile.Destroy(); m_bForceFlush = m_bSeparateEntries = false;};


		/*! @{
			@name Main functions
			These are the main functions.
		*/
		/*! Creates the log object.
			@param Filename The default filename.
			@param bSeparateEntries Whether to put an '\n' after each Append call.
			@param bForcedFlush Whether to flush after every Append call.
			@return True on success.
		*/
		bool Create(const std::string& Filename, bool bSeparateEntries = true, bool bForcedFlush = false);
		//! Clears the log object.
		inline void Destroy() {m_OutputFile.Destroy(); m_bSeparateEntries = m_bForceFlush = false;};		
		/*! Opens a file.
			@param OpenFlags The open flags.
		*/
		inline void Open(unsigned OpenFlags = std::ios::out | std::ios::app) {m_OutputFile.SetFileOpenFlags(OpenFlags); m_OutputFile.Open();};
		//! Closes the log file.
		void Close();
		//! @}

				
		/*! @{
			@name Output functions
		*/
		/*! Appends a string to the log (parses the string for variables).
			@param values The output format string.
		*/
		void Append(const char* values, ...);
		/*! Appends a string to the log (does not parse the string for variables).
			@param str The output string.
		*/
		void Append(const std::string& str);
		//! Flushes the file stream.
		inline void Flush()
		{
			if(m_bSeparateEntries) m_OutputFile.WriteData(std::string("\n"), 1);
			m_OutputFile.Flush();
		}
		//! @}


		/*! @{
			@name Get accessors
			The Get accessor functions.
		*/
		inline CFile* GetFile() {return &m_OutputFile;};
		inline bool IsForcedFlush() const {return m_bForceFlush;};
		inline bool IsSeparateEntries() const {return m_bSeparateEntries;};
		//! @}


		/*! 
			@name Set accessors.
			The Set accessor functions.
		*/
		inline void EnableForcedFlush(bool bEnable = true) {m_bForceFlush = bEnable;};
		inline void EnableSeparateEntries(bool bEnable = true) {m_bSeparateEntries = bEnable;};
		//! @}


		/*! @{ 
			@name Overloaded operators
		*/
		CApplicationLog& operator<<(unsigned int v);
		CApplicationLog& operator<<(int v);
		CApplicationLog& operator<<(float v);
		CApplicationLog& operator<<(double v);
		CApplicationLog& operator<<(long double v);
		CApplicationLog& operator<<(unsigned char v);
		CApplicationLog& operator<<(char v);
		CApplicationLog& operator<<(const std::string& v);
		CApplicationLog& operator<<(bool v);
		CApplicationLog& operator<<(const char* v);
		CApplicationLog& operator<<(void* v);
		inline CApplicationLog& operator<<(unsigned long v) {return operator<<(static_cast<unsigned>(v));};
		inline CApplicationLog& operator<<(unsigned short v) {return operator<<(static_cast<unsigned>(v));};
		//! @}

		

		/*! The destructor. Clears the object.
			@see Destroy
		*/
		~CApplicationLog(){m_OutputFile.Destroy(); m_bForceFlush = m_bSeparateEntries = false;};

	private:		
		//! The output file.
		CFile m_OutputFile;
		
		
		//! Whether to flush after every Append call.
		bool m_bForceFlush;
		//! Whether to put an '\n' after each Append call.
		bool m_bSeparateEntries;


} CAppLog;

}