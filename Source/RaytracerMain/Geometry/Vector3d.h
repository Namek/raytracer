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
		Vector3d(Point3d&, Point3d&, bool);
		Vector3d(float, float, float, bool norm = false);
		~Vector3d(void) { };
		void set(float x, float y, float z);
		float dotProduct(const Vector3d&) const;
		Vector3d crossProduct(const Vector3d&, bool) const;
		bool isInAABB(const Point3d& minDomain, const Point3d& maxDomain) const;
		
		inline Vector3d operator*(float scalar) const
		{
			return Vector3d(x * scalar, y * scalar, z * scalar);
		}
		inline Vector3d operator+(const Vector3d& vec) const
		{
			return Vector3d(x + vec.x, y + vec.y, z + vec.z);
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

		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) 
		{
			return os << v.x << " " << v.y << " " << v.z << "\n";
		};
		void normalize(void);
		float length(void);
	};
}

