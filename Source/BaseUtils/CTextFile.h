#pragma once


namespace Utils
{

//! A class for reading and writing formatted text data, uses \<cstdio>
class CTextFile
{
	public:
		/*! The default constructor. Clears the object.
			@see Destroy
		*/
		CTextFile(){Destroy();};


		/* @{
			@name Main functions
			These are the main functions.
		*/
		/*! Initializes the internal variables
			@param DefFilename The filename.
			@param DefOpenMode The open mode.
			@return Returns true on success.
		*/
		bool Create(const std::string& DefFilename, const char* DefOpenMode = "r+");
		//! Clears the internal variables
		void Destroy();
		/*! Opens the file using current parameters.
			@return Returns True on success.
		*/		
		bool Open();
		/*! Opens the file.
			@param Filename The filename.
			@param OpenMode File open mode.
		*/
		inline bool Open(const std::string& Filename, const char* OpenMode) {SetOpenMode(OpenMode); SetFilename(Filename); return Open();};
		//! Closes the file.
		inline void Close() {fclose(m_pFile); m_pFile = NULL; m_bIsOpen = false;};
		//! @}



		/*! @{
			@name Reading and writing
			These functions allow reading and writing.
		*/
		bool WriteData(const char* values, ...);
		bool ReadData(unsigned char* const Buffer, unsigned DataAmount);
		//! @}



		/*! @{
			@name Get accessors
			The Get accessor functions
		*/
		inline unsigned GetFileSize() const {return m_nFileSize;};
		inline const std::string& GetFilename() const {return m_strFilename;};
		inline const char* GetOpenMode() const {return m_strFileOpenMode;};
		inline bool IsOpen() const {return m_bIsOpen;};
		inline FILE* GetFileStruct() {return m_pFile;};
		//! @}

		
		/*! @{
			@name Set accessors
			The Set accessor functions.
		*/
		inline void SetOpenMode(const char* OpenMode) {m_strFileOpenMode = OpenMode;};
		inline void SetFilename(const std::string& Filename) {m_strFilename = Filename;};
		//! @}


		/* @{
			@name Error access
		*/
		inline const std::string& GetError() const {return m_strError;};
		inline void ClearError() {m_strError = "";};
		//! @}

		
		/*! The destructor. Clears the object.
			@see Destroy
		*/
		~CTextFile(){Destroy();};
	
	private:
		//! Internal FILE structure.
		FILE* m_pFile;
		//! File flags.
		const char* m_strFileOpenMode;
		
		
		//! The filename.
		std::string m_strFilename;
		//! The current file size.
		unsigned m_nFileSize;
		//! Whether the file is open.
		bool m_bIsOpen;


		//! Extended error data.
		std::string m_strError;


		//! Updates the file size.
		void UpdateFileSize();

};

}