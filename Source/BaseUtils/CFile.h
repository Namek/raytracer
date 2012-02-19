#pragma once


/*! \file CFile.h
	\brief This file contains file utility classes.
*/

namespace Utils
{


//! The maximum file extry length in bytes.
const unsigned int MAX_FILE_ENTRY_LENGTH = 512;

	
//! A class for reading and writing unformatted or binary data, uses \<fstream>
class CFile
{
	public:
		/*! The default constructor. Clears the object.
			@see Destroy
		*/
		CFile(){Destroy();};


		/*! @{ 
			@name Main functions
			These are the main functions.
		*/
		/*! Initializes the internal variables
			@param DefFilename The filename.
			@param DefFlags Default flags.
			@return Returns true on success.
		*/
		bool Create(const std::string& DefFilename, unsigned DefFlags = std::ios::in | std::ios::out | std::ios::trunc);
		//! Clears the internal variables
		void Destroy();
		/*! Opens the file using current parameters.
			@return Returns True on success.
		*/
		bool Open();
		/*! Opens the file.
			@param Filename The filename.
			@param OpenFlags File open flags.
		*/
		inline bool Open(const std::string& Filename, unsigned OpenFlags) {m_strFilename = Filename; m_nOpenFlags = OpenFlags; return Open();};
		//! Closes the file.
		void Close();
		//! @}


		/*! @{
			@name Reading and writing
			These functions allow reading and writing.
		*/
		bool ReadData(char* Buffer, unsigned Size);
		bool ReadData(std::string& Buffer, unsigned Size);
		bool WriteData(const char* const Buffer, unsigned Size);
		bool WriteData(const std::string& Buffer, unsigned Size);
		inline void Flush() {m_Stream.flush();};
		//! @}



		/*! @{
			@name Get accessors
			The Get accessor functions.
		*/
		inline unsigned GetFileSize() const {return m_nFileSize;};
		inline unsigned GetOpenFlags() const {return m_nOpenFlags;};
		inline long GetOutputPtrPos() {return static_cast<int>(m_Stream.tellp());};
		inline long GetInputPtrPos() {return static_cast<int>(m_Stream.tellg());};
		inline const std::string& GetFilename() const {return m_strFilename;};
		inline bool IsOpen() const {return m_Stream.is_open();};
		inline std::fstream& GetStream() {return m_Stream;};
		//! @}


		/*! @{
			@name Set accessors
			The Set accessor functions.
		*/
		inline void SetFilename(const std::string& Filename) {m_strFilename = Filename;};
		inline void SetFileOpenFlags(unsigned Flags) {m_nOpenFlags = Flags;};
		//! @}


		/*! @{
			@name File pointer operations	
		*/
		//! Synchronizes the input ptr with output ptr
		inline void SyncIwO() {m_Stream.seekg(m_Stream.tellp());};
		//! Synchronizes the output ptr with input ptr
		inline void SyncOwI(){m_Stream.seekp(m_Stream.tellg());};
		/*! Move the input ptr.
			@param Pos The position in the file in bytes.
		*/
		void MoveInputPtr(unsigned Pos);
		/*! Move the output ptr.
			@param Pos The position in the file in bytes.
		*/
		void MoveOutputPtr(unsigned Pos);
		//! @}
		


		/*! @{
			@name Overloaded operators
		*/
		inline CFile& operator<<(long long v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(unsigned long long v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(unsigned int v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(int v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(float v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(double v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(long double v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(unsigned char v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(char v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(const std::string& v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(bool v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(const char* v) {if(IsGood()) m_Stream << v; return *this;};
		inline CFile& operator<<(void* v) {if(IsGood()) m_Stream << v; return *this;};
		//! @}


		/*!	@{
			@name Error access
		*/
		inline const std::string& GetLastError() const {return m_strError;};
		inline void ClearError() {m_strError = ""; m_Stream.clear();};
		inline bool IsBad() const {return m_Stream.bad();};
		inline bool IsGood() const {return m_Stream.good();};
		inline bool IsFail() const {return m_Stream.fail();};
		inline bool IsEoF() const {return m_Stream.eof();};
		inline unsigned ReadState() const {return m_Stream.rdstate();};
		//! @}


		/*! The destructor. Clears the object.
			@see Destroy
		*/
		~CFile(){Destroy();};

	private:
		//! The stream structure.
		std::fstream m_Stream;

		//! The file flags.
		unsigned m_nOpenFlags;		

		//! The filename.
		std::string m_strFilename;
		//! The file size.
		unsigned m_nFileSize;
		
		//! The error data.
		std::string m_strError;


		/*! @{
			@name Internal functions
			These internal functions are used only by the class.
		*/
		//! Retrieves the current file size.
		inline void UpdateFileSize()
		{			
			int PrevPos = static_cast<int>(m_Stream.tellg());
			m_Stream.seekg(0, std::ios::end);
			m_nFileSize = static_cast<unsigned>(m_Stream.tellg());
			m_Stream.seekg(PrevPos);
		}
		//! Checks for any IO errors (returns true when any errors are encountered)		
		bool CheckIOErrors(const char* const Buffer, unsigned Size);
		//! Checks for any IO errors (returns true when any errors are encountered)
		bool CheckIOErrors(unsigned Size);
		//! Checks the filename and open flags.
		bool CheckFlagErrors();
		//! @}
};


}

