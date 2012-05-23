#pragma once

#include "Vector3d.h"
#include "../Texture.h"
#include <map>

namespace nprt
{
	class Triangle 
	{
		public:
			static float MaxDistance;

			Triangle();
			Triangle(Point3d, Point3d, Point3d, int);
			Triangle(Point3d, Point3d, Point3d, int, int);

			void setMaterialIndex(int);

			// Returns the intersection distance or -1
			float intersection(const Vector3d& observer_pos, const Vector3d& rayDirection) const;
			static float intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection);
			static float intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection, float& out_u, float& out_v);
			float displacedIntersection(const Point3d& rayOrigin, const Vector3d& rayDirection) const;

			inline void getUV(const Point3d& pointInTriangle, float& u, float& v) const;
			static void getUV(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Point3d& pointInTriangle, float& out_u, float& out_v);
			
			void SetTexcoords(float u1, float v1, float u2, float v2, float u3, float v3);

			// Checks if Axis-Aligned Bounding Box contains (minimum partly containment) this triangle.
			bool overlapsWithAABB(const Point3d& minDomain, const Point3d& maxDomain) const;
			bool overlapsWithAABB_byDomainCenter(const Point3d& boxCenter, const Point3d& boxHalfSize) const;

			friend std::ostream& operator<<(std::ostream& os, const Triangle& t) 
			{
				return os << t.p1 << t.p2 << t.p3 <<  t.materialIndex << " " << t.d << "\n" << t.norm
						  << t.a1 << " " << t.b1 << " " << t.d1 << "\n" << t.a2 << " " << t.b2 << " " << t.d2 << "\n"
						  << t.a3 << " " << t.b3 << " " << t.d3 << "\n"
						  << t.max1 << " " << t.min1 << " " << t.max2 << " " << t.min2 << " " << t.plane << "\n\n";
			}

			Point3d p1;
			Point3d p2;
			Point3d p3;
			int materialIndex;
			float a1, b1, a2, b2, a3, b3, d1, d2, d3;
			float u1, v1, u2, v2, u3, v3;
			float max1, max2, min1, min2;
			int plane;
			int ind;
			Vector3d norm;
			float d;

			mutable bool hasDisplacement;
			mutable Texture* texture;
			

		private:
			void preprocess(void);
	};
}