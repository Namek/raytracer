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
			OctreeNode(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int divideDepth = 1);

			OctreeNode* findNode(const Vector3d& point, const Vector3d& rayDir);		
			OctreeNode* findNode(const Vector3d& point);

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

			static const int MAX_DIVIDE_DEPTH = 1;
		};

	public:
		Octree();

		void buildTree(const std::vector<Triangle>& triangles, const Point3d& minDomain, const Point3d& maxDomain);
		void setObserverPoint(const Point3d& point);
		bool castRayForTriangle(const Vector3d& rayDir, Triangle& out_triangle);

	private:
		inline int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) const;
		inline int nextNode(float tx, float ty, float tz, int ix, int iy, int iz) const;
		void procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node);

	private:
		std::vector<Triangle> m_Triangles;
		std::unique_ptr<OctreeNode> m_pRoot;
		
		Point3d m_MaxDomain;
		Point3d m_MinDomain;
		Point3d m_DomainSize;
		Point3d m_SmallestNodeDivide;

		Point3d m_ObserverPoint;
		int indexSwapper;
	};
}
