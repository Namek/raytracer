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
		Vector3d(float x, float y, float z, bool norm = false);
		Vector3d(float yaw, float pitch);
		~Vector3d(void) { };
		void set(float x, float y, float z);
		void set(float yaw, float pitch);
		float dotProduct(const Vector3d&) const;
		Vector3d crossProduct(const Vector3d&, bool) const;
		void rotateX(float pitch);
		void rotateY(float yaw);
		void rotateZ(float roll);
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

		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) 
		{
			return os << v.x << " " << v.y << " " << v.z << "\n";
		};
		void normalize(void);
		float length(void);
	};
}

