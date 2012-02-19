#pragma once


#pragma warning(disable: 4312)
#pragma warning(disable: 4311)


/*! \file CSingleton.h
	\brief This file contains the definition of the CSingleton class.
*/

namespace Utils
{

/*! 
	\brief A base class used to represent a singleton.
		
	It is used as a base class for singletons. Note that the derived classes are responsible for allocating and deallocating the memory associated with the singleton.
*/
template<typename T> class CSingleton
{
	public:
		//! The constructor. It is responsible for setting the pointer to the valid memory area.
		CSingleton()
		{

			assert(!m_pSingleton);
			
			//use a cunning trick to get the singleton pointing to the start of
			//the whole, rather than the start of the Singleton part of the object
			int offset = (int)(T*)1 - (int)(CSingleton <T>*)(T*)1;
			m_pSingleton = (T*)((int)(this) + offset);			
		}
		
		/*! @{
			@name Get accessors
			The Get accessor functions
		*/
		//! The function returns a reference to the singleton.
		inline static T& GetSingleton() {assert(m_pSingleton); return *m_pSingleton;};
		//! The function returns a pointer to the singleton.
		inline static T* GetSingletonPtr() {assert(m_pSingleton); return m_pSingleton;};
		//! @}


		//! The destructor. Clears the singleton pointer.
		~CSingleton(){assert(m_pSingleton); m_pSingleton = 0;};

	private:
		//! The pointer to the singleton.
		static T* m_pSingleton;
};


template<typename T> T* CSingleton<T>::m_pSingleton = 0;


}