#include "floats.h"

using namespace nprt::math;

float nprt::math::dot(const float3& a, const float3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/*inline float dot___(const vector3& b) const
{
	return _mm_cvtss_f32(_mm_dp_ps(mmvalue, b.mmvalue, 0x71));
}*/

__m128 nprt::math::LoadFloat3(const float3& value)
{
	__m128 x = _mm_load_ss(&value.x);
	__m128 y = _mm_load_ss(&value.y);
	__m128 z = _mm_load_ss(&value.z);
	__m128 xy = _mm_movelh_ps(x, y);
	return _mm_shuffle_ps(xy, z, _MM_SHUFFLE(2, 0, 2, 0));
}

__m128 nprt::math::Load3Floats(const float& vx, const float& vy, const float& vz)
{
	__m128 x = _mm_load_ss(&vx);
	__m128 y = _mm_load_ss(&vy);
	__m128 z = _mm_load_ss(&vz);
	__m128 xy = _mm_movelh_ps(x, y);
	return _mm_shuffle_ps(xy, z, _MM_SHUFFLE(2, 0, 2, 0));
}

// sqrt(x^2 + y^2 + z^2)
float nprt::math::length3(__m128 v)
{
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(v, v, 0x71)));
}

// (x,y,z) / sqrt(x^2 + y^2 + z^2)
__m128 nprt::math::normalize3_accurate(__m128 v)
{
	__m128 norm = _mm_sqrt_ps(_mm_dp_ps(v, v, 0x7F));
	return _mm_div_ps(v, norm);
}

float nprt::math::dot_product(float *a, float *b)
{
	float arr[4];	
	float total;
	int i;
	__m128 num1, num2, num3, num4;
	num4= _mm_setzero_ps();  //sets sum to zero
	//for(i=0; i<SIZE; i+=4)
	{
		num1 = _mm_loadu_ps(a+i);   //loads unaligned array a into num1  num1= a[3]  a[2]  a[1]  a[0]
		num2 = _mm_loadu_ps(b+i);   //loads unaligned array b into num2  num2= b[3]   b[2]   b[1]  b[0]
		num3 = _mm_mul_ps(num1, num2); //performs multiplication   num3 = a[3]*b[3]  a[2]*b[2]  a[1]*b[1]  a[0]*b[0]
		num3 = _mm_hadd_ps(num3, num3); //performs horizontal addition
									//num3=  a[3]*b[3]+ a[2]*b[2]  a[1]*b[1]+a[0]*b[0]  a[3]*b[3]+ a[2]*b[2]  a[1]*b[1]+a[0]*b[0]
		num4 = _mm_add_ps(num4, num3);  //performs vertical addition
	}
	num4= _mm_hadd_ps(num4, num4);
	_mm_store_ss(&total,num4);
	return total;
}