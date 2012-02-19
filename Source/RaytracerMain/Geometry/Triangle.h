#pragma once

#include "Point3d.h"
#include "Vector3d.h"

namespace nprt
{
	class Triangle {
	//private:
	public:
		Point3d p1;
		Point3d p2;
		Point3d p3;
		int atributeIndex;
		float a1, b1, a2, b2, a3, b3, d1, d2, d3;
		float max1, max2, min1, min2;
		int plane;
		int ind;
	//public:
		Vector3d norm;
		float d;
		Triangle();
		Triangle(Point3d, Point3d, Point3d, int);
		Triangle(Point3d, Point3d, Point3d, int, int);
		void setAtributeIndex(int);
		friend std::ostream& operator<<(std::ostream& os, const Triangle& t) 
		{
			return os << t.p1 << t.p2 << t.p3 <<  t.atributeIndex << " " << t.d << "\n" << t.norm
					  << t.a1 << " " << t.b1 << " " << t.d1 << "\n" << t.a2 << " " << t.b2 << " " << t.d2 << "\n"
					  << t.a3 << " " << t.b3 << " " << t.d3 << "\n"
					  << t.max1 << " " << t.min1 << " " << t.max2 << " " << t.min2 << " " << t.plane << "\n\n";
		};
	private:
		void preprocess(void);
	};
}