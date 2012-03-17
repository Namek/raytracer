#pragma once

#include "Point3d.h"


namespace nprt 
{
	class Vector3d
	{
	public:
		float x, y, z;
		Vector3d(void);
		Vector3d(Point3d&, Point3d&, bool);
		Vector3d(float, float, float, bool norm = true);
		~Vector3d(void) { };
		float dotProduct(const Vector3d&) const;
		Vector3d crossProduct(const Vector3d&, bool) const;
		
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
		inline Vector3d operator-(const Point3d& vec) const
		{
			return Vector3d(x - vec.x, y - vec.y, z - vec.z);
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) 
		{
			return os << v.x << " " << v.y << " " << v.z << "\n";
		};
		void normalize(void);
	//private:
		float length(void);
	};
}

