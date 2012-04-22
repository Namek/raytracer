#pragma once

#include <vector>
#include "Triangle.h"
#include "Plane.h"

namespace nprt
{
	class Octree
	{
	private:
		class OctreeNode
		{
		public:
			OctreeNode();
			OctreeNode(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int divideDepth = 0);

			bool containsPoint(const Point3d& point) const;
			
			bool m_isLeaf;
			int m_TrianglesInclusiveCount;
			std::unique_ptr<OctreeNode*[]> m_NearNodes;
			std::unique_ptr<OctreeNode*[]> m_Subnodes;
			std::vector<Triangle> m_Triangles;
			std::unique_ptr<Plane[]> m_DomainPlanes;
			Point3d m_MaxDomain;
			Point3d m_MinDomain;
			Point3d m_DomainSize;

		private:
			void divide(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int depth);
			void updateNearNodes(bool isRoot);

			static const int NEAR_NODE_ABOVE_INDEX = 0;
			static const int NEAR_NODE_BELOW_INDEX = 1;
			static const int NEAR_NODE_LEFT_INDEX = 2;
			static const int NEAR_NODE_RIGHT_INDEX = 3;
			static const int NEAR_NODE_BEFORE_INDEX = 4;
			static const int NEAR_NODE_BEHIND_INDEX = 5;
			static const int NEAR_NODES_COUNT = 6;

			static const int CHILD_LOWER_NEAR_LEFT_INDEX = 0;
			static const int CHILD_LOWER_FAR_LEFT_INDEX = 1;
			static const int CHILD_UPPER_NEAR_LEFT_INDEX = 2;
			static const int CHILD_UPPER_FAR_LEFT_INDEX = 3;
			static const int CHILD_LOWER_NEAR_RIGHT_INDEX = 4;
			static const int CHILD_LOWER_FAR_RIGHT_INDEX = 5;
			static const int CHILD_UPPER_NEAR_RIGHT_INDEX = 6;
			static const int CHILD_UPPER_FAR_RIGHT_INDEX = 7;
			static const int CHILD_SUBNODES_COUNT = 8;

			static const int AXIS_PLANE_NONE = 0;
			static const int AXIS_PLANE_XOY = 1;
			static const int AXIS_PLANE_XOZ = 2;
			static const int AXIS_PLANE_YOZ = 3;

			static const int DOMAIN_PLANE_BOTTOM = 0;
			static const int DOMAIN_PLANE_TOP = 1;
			static const int DOMAIN_PLANE_LEFT = 2;
			static const int DOMAIN_PLANE_RIGHT = 3;
			static const int DOMAIN_PLANE_NEAR = 4;
			static const int DOMAIN_PLANE_FAR = 5;

			static const int MAX_DIVIDE_DEPTH = 0;
		};

	public:
		Octree();

		void buildTree(const std::vector<Triangle>& triangles, const Point3d& minDomain, const Point3d& maxDomain);
		void setObserverPoint(const Point3d& point) const;//may be deprecated

		void traceRayForTriangles(const Point3d& rayOrigin, const Vector3d& rayDirection, std::vector<std::pair<Triangle, Point3d>>& out_intersectedTriangles) const;


	private:
		bool castRayForTriangle(const Vector3d& rayDir, const Vector3d& rayDirection, std::pair<Triangle, Point3d>& triangleWithIntersectionPoint) const;
		bool procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node, int indexSwapper,
			const Point3d& rayOrigin, const Vector3d& rayDirection, std::pair<Triangle, Point3d>& triangleWithIntersectionPoint) const;
		inline int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) const;
		inline int nextNode(float tx, float ty, float tz, int ix, int iy, int iz) const;
		float distanceToNearestPlane(const Point3d& point, Plane& out_nearestPlane) const;


	private:
		//std::vector<Triangle> m_Triangles;
		std::unique_ptr<OctreeNode> m_pRoot;
		
		Point3d m_MaxDomain;
		Point3d m_MinDomain;
		Point3d m_DomainSize;
		Point3d m_SmallestNodeDivide;

		mutable Point3d m_ObserverPoint;
		mutable Plane m_NearestPlane;
		mutable float m_DistanceToNearestPlane;

		static const int MAX_SECONDARY_RAYS_NUMBER = 3;
	};
}
