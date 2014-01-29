#pragma once

#include <cstring>

namespace nprt
{
	namespace math
	{
		/**
		 * Fast inverse sqrt.
		 *
		 * Source: http://stackoverflow.com/questions/17789928/fast-inverse-square-root-algorithm-in-modern-c
		 */
		float InverseSquareRoot(float x);
		float fsqrt(float n);
		float __fastcall dsqrt(double n);
	}
}