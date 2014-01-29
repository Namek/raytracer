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
			Triangle(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, int ind = -1);
			Triangle(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, int materialIndex, int ind);

			void setMaterialIndex(int);

			// Returns the intersection distance or -1
			float intersection(const Vector3d& observer_pos, const Vector3d& rayDirection) const;
			static float intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection);
			static float intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection, Vector3d& out_intersectionPoint);
			static float intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection, float& out_u, float& out_v);
			//float displacedIntersection(const Vector3d& rayOrigin, const Vector3d& rayDirection) const;
			float displacedIntersection(const Vector3d& rayOrigin, const Vector3d& rayDirection, Triangle& out_Triangle) const;


			inline void getUV(const Vector3d& pointInTriangle, float& u, float& v) const;
			static void getUV(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& pointInTriangle, float& out_u, float& out_v);
			Vector3d getPoint(float u, float v) const;
			static bool isRightTriangle(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, Vector3d& out_largestAnglePoint, Vector3d& out_otherPoint1, Vector3d& out_otherPoint2);
			
			void SetTexcoords(float u1, float v1, float u2, float v2, float u3, float v3);
			inline void setP1(const Vector3d& p) {this->p1 = p;};
			inline void setP3(const Vector3d& p) {this->p3 = p;};

			// Checks if Axis-Aligned Bounding Box contains (minimum partly containment) this triangle.
			bool overlapsWithAABB(const Vector3d& minDomain, const Vector3d& maxDomain) const;
			bool overlapsWithAABB_byDomainCenter(const Vector3d& boxCenter, const Vector3d& boxHalfSize) const;			

			Vector3d p1;
			Vector3d p2;
			Vector3d p3;
			int materialIndex;
			float u1, v1, u2, v2, u3, v3;
			float max1, max2, min1, min2;
			int ind;
			Vector3d norm;
			float d;

			//barycentric coordinates cache
			float dot00, dot01, dot11;
			Vector3d _v0, _v1;

			mutable bool hasDisplacement;
			mutable Texture* texture;
	};

}