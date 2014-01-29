#pragma once

#include <iostream>

#include <mmintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <smmintrin.h>

namespace nprt
{
	//_MM_ALIGN16 // alignment is needed to instantiate and use mmvalue properly which is needed to use SSE intrinsics
	class Vector3d
	{
	public:
		union
		{
		struct { float x, y, z; };
		//__m128 mmvalue;
		 float coords[4];
		};

		Vector3d();// : mmvalue(_mm_setzero_ps()) { };
		Vector3d(const Vector3d& point1, const Vector3d& point2, bool normalize = false);
		Vector3d(float x, float y, float z, bool normalize = false);
		Vector3d(float yaw, float pitch);

		void set(float x, float y, float z);
		void set(float yaw, float pitch);
		void set(const Vector3d& v);
		float dotProduct(const Vector3d&) const;
		Vector3d crossProduct(const Vector3d& otherVector, bool normalize = false) const;
		void rotateX(float pitch);
		void rotateY(float yaw);
		void rotateZ(float roll);
		static float pointLineDistance(const Vector3d& point, const Vector3d& linePoint1, const Vector3d& linePoint2);
		float pointLineDistance(const Vector3d& point) const;
		static float lineLineDistance(const Vector3d& line1Point1, const Vector3d& line1Point2, const Vector3d& line2Point1, const Vector3d& line2Point2);
		float lineLineDistance(const Vector3d& otherLinePoint1, const Vector3d& otherLinePoint2) const;
		bool isInAABB(const Vector3d& minDomain, const Vector3d& maxDomain, float epsilon = 0.00002f) const;

		
		inline Vector3d operator*(float scalar) const
		{
			return Vector3d(x * scalar, y * scalar, z * scalar);
		}
		inline Vector3d operator*(const Vector3d& vec) const
		{
			return Vector3d(x * vec.x, y * vec.y, z * vec.z);
		}
		inline Vector3d operator+(const Vector3d& vec) const
		{
			return Vector3d(x + vec.x, y + vec.y, z + vec.z);
		}
		inline void operator+=(const Vector3d& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
		}
		inline Vector3d operator-(const Vector3d& vec) const
		{
			return Vector3d(x - vec.x, y - vec.y, z - vec.z);
		}
		inline void operator/=(float factor)
		{
			x /= factor;
			y /= factor;
			z /= factor;
		}
		inline void operator*=(float factor)
		{
			x *= factor;
			y *= factor;
			z *= factor;
		}
		inline Vector3d operator-() const
		{
			return Vector3d(-x, -y, -z);
		}
		inline bool operator==(const Vector3d& rhs) const
		{
			return rhs.x == x && rhs.y == y && rhs.z == z;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) 
		{
			return os << v.x << " " << v.y << " " << v.z << "\n";
		};
		void normalize(void);
		float length(void) const;

		// overloaded operators that ensure alignment
		inline void* operator new[](size_t x) { return _aligned_malloc(x, 16); }
		inline void operator delete[](void* x) { if (x) _aligned_free(x); }
	};


	/**
	 * Hack made due to compiler's C2719 error which was invoked by Vector3d alignment.
	 *
	 * source: http://stackoverflow.com/questions/9409591/self-contained-stl-compatible-implementation-of-stdvector/9414618#9414618
	 */
	template <typename T>
	struct V3dWrapper : public T
	{
		V3dWrapper() { }
		V3dWrapper(const T& rhs) : T(rhs) { }
		V3dWrapper(const Vector3d& point1, const Vector3d& point2, bool normalize = false) : T(point1, point2, normalize) { }
		V3dWrapper(float x, float y, float z, bool normalize = false) : T(x, y, z, normalize) { }
		V3dWrapper(float yaw, float pitch) : T(yaw, pitch) { };
	};

	typedef V3dWrapper<Vector3d> AlignedVector3d;
}

