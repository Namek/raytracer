#include "Math.h"

using namespace nprt::math;

float InverseSquareRoot(float x)
{
	float xhalf = 0.5f*x;
	unsigned int i;
	std::memcpy(&i, &x, sizeof(i));
	i = 0x5f3759df - (i>>1);
	std::memcpy(&x, &i, sizeof(i));
	x = x*(1.5f - xhalf*x*x);
	return x;
}

float nprt::math::fsqrt(float n)
{
	__asm
	{
		fld n
		fsqrt
	}
}   

float __declspec (naked) __fastcall dsqrt(double n)
{
	_asm fld qword ptr [esp+4]
	_asm fsqrt
	_asm ret 8
}