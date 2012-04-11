#pragma once

#include <cstdlib>

/*!	\file Base.h
	\brief Thes file contains various functions (converting, math, sorting).
*/

namespace Utils
{

/*! @{
	@name Helper functions
*/
/*! Releases and sets to 0 a pointer to an object.
	@param v A reference to a pointer of the object to be deleted.
*/
template<typename T> inline void SafeDelete(T& v) {delete v; v = 0;}
/*! Releases and sets to 0 a pointer to an dynamically allocated array.
	@param v A reference to a pointer of the dynamically allocated array to be deleted.
*/
template<typename T> inline void SafeDeleteArr(T& v) {delete [] v; v = 0;};

/*! Performs a safe release of a COM interface.
	@param v A reference to a COM object pointer.
*/
template<typename T> inline void SafeRelease(T& v) {if(v) {v->Release(); v = 0;}};
//Utility functions and definitions
//! Defines a functions for obtaining the size of an object.
#define AUTO_SIZE unsigned long Size() {return sizeof(*this);};
//! @}



//! Generates a pseudorandom integer number from the given range.
/*!
	@param Min The minimum number to generate.
	@param Max The maximum number to generate.
	@return The generated number.
*/
inline int Rand(int Min, int Max) {return ((rand() % Max - Min) + Min);};


/*! @{
	@name Comparison functions
*/
template <typename T> inline bool LessThan(const T& v1, const T& v2) {return (v1 < v2);};
template <typename T> inline bool GreaterThan(const T& v1, const T& v2) {return (v1 > v2);};
//! @}



/*! @{
	@name Math functions
*/
//! Returns the next power of 2
template<typename T> inline T NextPowerOf2(T Value)
{	
	T rval = 1;
	while(rval < Value) rval <<= 1;	
	return rval;
};
//! Check whether the number is a power of 2.
template<typename T> inline T IsPowerOf2(T Value) {return (!(Value & (Value - 1)));}
//! Checks whether the point is inside a cubic.
inline bool IsInCubic(float x, float y, float z, float left, float top, float front, float right, float bottom, float back){return ((x >= left && x <= right) && (y >= top && y <= bottom) && (z >= front && z <= back));}
//! Checks whether a point is inside a rectangle.
inline bool IsInRect(float x, float y, float left, float top, float right, float bottom) {return ((x >= left && x <= right) && (y <= top && y >= bottom));}
//! Converts radians to degrees.
inline double DegToRad(const double& Deg) {return 3.14159265/ 180.0f * Deg;};
//! Converts degrees to radians.
inline double RadToDeg(const double& Rad) {return Rad * 180.0f / 3.14159265;};
// !}
template<typename T> inline T Clamp(T value, T lower_bound, T upper_bound)
{
	if(value > upper_bound)
		return upper_bound;
	else if(value < lower_bound)
		return lower_bound;
	else 
		return value;
}

/*! @{
	@name Type conversion functions
*/

//! Converts float to unsigned long.
inline unsigned long FloatToDWORD(float f) {return *(unsigned long*)(&f);};
//! Converts a number to an std::string.
template<typename T> inline std::string NumToStr(T Value)
{
	std::stringstream ss;
	ss.str("");
	ss << Value;
	return ss.str();
}
//! Converts an std::string to a number.
template<typename Result> inline Result StrToNum(const std::string& Value, Result r)
{
	std::stringstream ss2;
	ss2.flush();
	ss2 << Value;
	Result Num = Result();
	ss2 >> Num;
	return Num;
}
//! Converts an std::string to a number.
template<typename T> inline bool StrToNum(const std::string& str, T& num, std::ios_base& (*f)(std::ios_base&) = std::dec)
{
	static std::istringstream g_iss(str);
	return !(g_iss >> f >> num).fail();
}
//! Absolute cast: direcly maps bits between different types.
template<typename DestType, typename SrcType> inline DestType& absolute_cast(SrcType& Value) {return reinterpret_cast<DestType&>(Value);};
//! Absolute cast: direcly maps bits between different types.
template<typename DestType, typename SrcType> inline const DestType& absolute_cast(const SrcType& Value) {return reinterpret_cast<const DestType&>(Value);};
//! @}


/*! @{
	@name STL container clearing functions
*/
//! Clears an STL container.
template <typename T> void STLContainerClear(T& Container)
{
	for(T::iterator i = Container.begin(); i != Container.end(); ++i) delete(*i);
	Container.clear();
}
template <typename Key, typename Val> void STLMapClear(std::map<Key, Val>& Container)
{
	for(std::map<Key, Val>::iterator i = Container.begin(); i != Container.end(); ++i)
	{
		SafeDelete(i->second);
	}
	Container.clear();
}
//! Clears an STL 2D container (ex. a vector).
template <typename T> void STL2DContainerClear(T& Container)
{
	for(T::iterator i = Container.begin(); i != Container.end(); ++i) 
	{
			(*i).clear();
	}

	Container.clear();
}
//! @}



//**********************************************
//Algorithms
//**********************************************
/*! @{
 @name Sorting algorithms
*/

/*! The natural sort.
	@return Returns the number of swaps performed.
*/
template <typename T> unsigned NaturalSort(T* Elements, unsigned ElemNum, bool AscOrder)
{
	if(!(Elements && ElemNum)) return 0;

	unsigned NumSwaps = 0;
	unsigned TotalSwaps = 0;

	bool (*CmpFunc) (const T&, const T&) = NULL;
	if(AscOrder) CmpFunc = GreaterThan;
	else CmpFunc = LessThan;


	do
	{
		TotalSwaps +=  NumSwaps;
		NumSwaps = 0;

		for(unsigned i = 0; i < ElemNum - 1; ++i)
		{
			if(CmpFunc(Elements[i], Elements[i + 1]))
			{
				T tmp = Elements[i];
				Elements[i] = Elements[i + 1];
				Elements[i + 1] = tmp;
				++NumSwaps;
			}
		}

	}
	while(NumSwaps);

	return TotalSwaps;
}



/*! The bubble sort.
	@return Returns the number of swaps performed.
*/
template <typename T> unsigned BubbleSort(T* Elements, unsigned ElemNum, bool AscOrder)
{
	if(!(Elements && ElemNum)) return 0;

	unsigned NumSwaps = 0;
	unsigned TotalSwaps = 0;
	unsigned LastSwap = 0;


	bool (*CmpFunc) (const T&, const T&) = NULL;
	if(AscOrder) CmpFunc = &GreaterThan;
	else CmpFunc = &LessThan;

	T tmp = T();	
	do
	{
		TotalSwaps += NumSwaps;
		NumSwaps = 0;		

		for(unsigned i = 0; i < ElemNum - 1; ++i)
		{
			if(CmpFunc(Elements[i], Elements[i + 1]))
			{
				tmp = Elements[i];
				Elements[i] = Elements[i + 1];
				Elements[i + 1] = tmp;
				++NumSwaps;
				LastSwap = i;
			}			
		}
		++LastSwap;
	}
	while(NumSwaps);	

	return TotalSwaps;
}
//! @}


}
