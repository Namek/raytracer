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
			OctreeNode(const Vector3d& minDomain, const Vector3d& maxDomain, const std::vector<Triangle>& triangles, int maxDivideDepth, int currentDivideDepth = 0);

			bool containsPoint(const Vector3d& point, float epsilon = 0.000005f) const;
			
			bool m_isLeaf;
			int m_TrianglesInclusiveCount;
			int m_TrianglesExclusiveCount;
			std::unique_ptr<OctreeNode*[]> m_NearNodes;
			std::unique_ptr<OctreeNode*[]> m_Subnodes;
			std::vector<Triangle> m_Triangles;
			std::unique_ptr<Plane[]> m_DomainPlanes;
			Vector3d m_MaxDomain;
			Vector3d m_MinDomain;
			Vector3d m_DomainSize;
			int m_MaxDivideDepth;

		private:
			void divide(const Vector3d& minDomain, const Vector3d& maxDomain, const std::vector<Triangle>& triangles, int maxDivideDepth, int currentDepth);
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
		};

	public:
		Octree();

		void setMaxDivideDepth(int maxDivideDepth) { m_MaxDivideDepth = maxDivideDepth; }
		void buildTree(const std::vector<Triangle>& triangles, const Vector3d& minDomain, const Vector3d& maxDomain);
		Vector3d getDomainSize() const { return m_DomainSize; }

		void traceRayForTriangles(const Vector3d& rayOrigin, const Vector3d& rayDirection, std::vector<std::pair<Triangle, AlignedVector3d>>& out_intersectedTriangles) const;
		bool castRayForTriangle(const Vector3d& rayOrigin, const Vector3d& rayDirection, std::pair<Triangle, AlignedVector3d>& triangleWithIntersectionPoint) const;

	private:		
		bool procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node, int indexSwapper,
			const Vector3d& rayOrigin, const Vector3d& rayDirection, std::vector<std::pair<Triangle, AlignedVector3d>>& foundTriangles) const;
		inline int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) const;
		inline int nextNode(float tx, float ty, float tz, int ix, int iy, int iz) const;


	private:
		std::unique_ptr<OctreeNode> m_pRoot;
		int m_MaxDivideDepth;
		
		Vector3d m_MaxDomain;
		Vector3d m_MinDomain;
		Vector3d m_DomainSize;
		Vector3d m_SmallestNodeDivide;
		
		static const int MAX_SECONDARY_RAYS_NUMBER = 1;
	};
}
