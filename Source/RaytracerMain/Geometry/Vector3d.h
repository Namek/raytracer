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
		Vector3d(float, float, float, bool);
		~Vector3d(void) { };
		float dotProduct(Vector3d&);
		Vector3d crossProduct(Vector3d&, bool);
		friend std::ostream& operator<<(std::ostream& os, const Vector3d& v) 
		{
			return os << v.x << " " << v.y << " " << v.z << "\n";
		};
		void normalize(void);
	//private:
		float length(void);
	};
}

