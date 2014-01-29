#pragma once

#include <mmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>

namespace nprt
{
	// source: http://fastcpp.blogspot.com/p/common-datatypes.html
	namespace math
	{
		struct float3
		{
			float3() {};
			float3(float s) : x(s), y(s), z(s) {}
			float3(float x, float y, float z) : x(x), y(y), z(z) {}
			float x, y, z;
 
			inline float3 operator*(float s) const { return float3(x*s, y*s, z*s); }
			inline float3 operator+(const float3& a) const { return float3(x+a.x, y+a.y, z+a.z); }
		};
 
		// dot product of two float3 vectors
		float dot(const float3& a, const float3& b);

		__m128 LoadFloat3(const float3& value);

		__m128 Load3Floats(const float& vx, const float& vy, const float& vz);

		// sqrt(x^2 + y^2 + z^2)
		float length3(__m128 v);

		// (x,y,z) / sqrt(x^2 + y^2 + z^2)
		__m128 normalize3_accurate(__m128 v);

		float dot_product(float *a, float *b);
	}
}