#pragma once

#include <iostream>

namespace nprt 
{
	class Vector3d;
	typedef Vector3d Point3d;

	class Vector3d
	{
	public:
		float x, y, z;
		Vector3d(void);
		Vector3d(const Point3d& point1, const Point3d& point2, bool normalize = false);
		Vector3d(float x, float y, float z, bool normalize = false);
		Vector3d(float yaw, float pitch);
		~Vector3d(void) { };
		void set(float x, float y, float z);
		void set(float yaw, float pitch);
		void set(const Vector3d& v);
		float dotProduct(const Vector3d&) const;
		Vector3d crossProduct(const Vector3d& otherVector, bool normalize = false) const;
		void rotateX(float pitch);
		void rotateY(float yaw);
		void rotateZ(float roll);
		static float pointLineDistance(const Point3d& point, const Point3d& linePoint1, const Point3d& linePoint2);
		float pointLineDistance(const Point3d& point) const;
		static float lineLineDistance(const Point3d& line1Point1, const Point3d& line1Point2, const Point3d& line2Point1, const Point3d& line2Point2);
		float lineLineDistance(const Vector3d& otherLinePoint1, const Vector3d& otherLinePoint2) const;
		bool isInAABB(const Point3d& minDomain, const Point3d& maxDomain, float epsilon = 0.00002f) const;

		
		inline Vector3d operator*(float scalar) const
		{
			return Vector3d(x * scalar, y * scalar, z * scalar);
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
	};
}

