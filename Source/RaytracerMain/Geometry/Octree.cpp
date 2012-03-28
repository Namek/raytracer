#include "Octree.h"

using namespace nprt;
using namespace std;


Octree::OctreeNode::OctreeNode() : 
	m_isLeaf(false),
	m_TrianglesInclusiveCount(0),
	m_NearNodes(new OctreeNode*[NEAR_NODES_COUNT]),
	m_Subnodes(new OctreeNode*[CHILD_SUBNODES_COUNT]),
	m_Triangles(),
	m_DomainPlanes(new Plane[6]),
	m_MaxDomain(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity()),
	m_MinDomain(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_DomainSize(0, 0, 0)
{
}

Octree::OctreeNode::OctreeNode(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int divideDepth) :
	m_isLeaf(false),
	m_TrianglesInclusiveCount(0),
	m_NearNodes(new OctreeNode*[NEAR_NODES_COUNT]),
	m_Subnodes(new OctreeNode*[CHILD_SUBNODES_COUNT]),
	m_Triangles(),
	m_DomainPlanes(new Plane[6]),
	m_MaxDomain(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity()),
	m_MinDomain(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_DomainSize(0, 0, 0)
{
	// Generate whole tree
	divide(minDomain, maxDomain, triangles, divideDepth);
}

Octree::OctreeNode* Octree::OctreeNode::findNode(const Vector3d& point, const Vector3d& ray)
{
	Vector3d p = point;

	// If outside of the domain
	if (point.x < m_MinDomain.x || point.x > m_MaxDomain.x ||
		point.y < m_MinDomain.y || point.y > m_MaxDomain.y ||
		point.z < m_MinDomain.z || point.z > m_MaxDomain.z)
	{
		// Find plane nearest to the observer point
		float dist = m_DomainPlanes[0].distance(point);
		Plane& nearestPlane = m_DomainPlanes[0];

		for (int i = 1; i < 6; i++)
		{
			float tmpDist = m_DomainPlanes[i].distance(point);

			if (tmpDist < dist)
			{
				dist = tmpDist;
				nearestPlane = m_DomainPlanes[i];
			}
		}

		// Calculate ray's intersection point with nearest plane
		p = point + ray*dist;
	}
	
	// Now we have point inside of the domain.
	// Let's find out in which node the point 'p' is located.
	return findNode(p);
}

// We assume that the point is located inside of the current domain.
Octree::OctreeNode* Octree::OctreeNode::findNode(const Vector3d& point)
{
	OctreeNode* node = this;
	
	/*do
	{
		switch (node->m_CutAxisPlane)
		{
			case AXIS_PLANE_XOY:
			{
				if (point.x < m_MinDomain.x + m_DomainSize.x/2)
					node = m_pLeftChild.get();
				else
					node = m_pRightChild.get();
			}
			break;

			case AXIS_PLANE_XOZ:
			{
				if (point.y < m_MinDomain.y + m_DomainSize.y/2)
					node = m_pLeftChild.get();
				else
					node = m_pRightChild.get();
			}
			break;

			case AXIS_PLANE_YOZ:
			{
				if (point.z < m_MinDomain.z + m_DomainSize.z/2)
					node = m_pLeftChild.get();
				else
					node = m_pRightChild.get();
			}
		}
	}
	while (node->m_CutAxisPlane != AXIS_PLANE_NONE);*/

	return node;
}


void Octree::OctreeNode::divide(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int depth)
{
	m_MinDomain = minDomain;
	m_MaxDomain = maxDomain;
	m_DomainSize = m_MaxDomain - m_MinDomain;
	
	if (depth < MAX_DIVIDE_DEPTH)
	{
		std::vector<Triangle> nodesTriangles[CHILD_SUBNODES_COUNT];
		pair<Point3d, Point3d> nodesDomains[CHILD_SUBNODES_COUNT];
		Vector3d halfDomainSize = m_DomainSize * 0.5f;
		Point3d center = m_MinDomain + halfDomainSize;

		// Calculate subboxes' domains
		nodesDomains[CHILD_LOWER_NEAR_LEFT_INDEX] = pair<Point3d, Point3d>(minDomain, center);
		nodesDomains[CHILD_LOWER_FAR_LEFT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(0, 0, halfDomainSize.z), center + Vector3d(0, 0, halfDomainSize.z));
		nodesDomains[CHILD_UPPER_NEAR_LEFT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(0, halfDomainSize.y, 0), center + Vector3d(0, halfDomainSize.y, 0));
		nodesDomains[CHILD_UPPER_FAR_LEFT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(0, halfDomainSize.y, halfDomainSize.z), center + Vector3d(0, halfDomainSize.y, halfDomainSize.z));

		nodesDomains[CHILD_LOWER_NEAR_RIGHT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(halfDomainSize.x, 0, 0), center + Vector3d(halfDomainSize.x, 0, 0));
		nodesDomains[CHILD_LOWER_FAR_RIGHT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(halfDomainSize.x, 0, halfDomainSize.z), center + Vector3d(halfDomainSize.x, 0, halfDomainSize.z));
		nodesDomains[CHILD_UPPER_NEAR_RIGHT_INDEX] = pair<Point3d, Point3d>(minDomain + Vector3d(halfDomainSize.x, halfDomainSize.y, 0), center + Vector3d(halfDomainSize.x, halfDomainSize.y, 0));
		nodesDomains[CHILD_UPPER_FAR_RIGHT_INDEX] = pair<Point3d, Point3d>(center, center + Vector3d(halfDomainSize.x, halfDomainSize.y, halfDomainSize.z));


		// Assign triangles to the subnodes. Some triangles may exist in multiple subnodes.
		int assignments = 0;
		for (int triangleIndex = 0, trianglesCount = triangles.size(); triangleIndex < trianglesCount; ++triangleIndex)
		{
			const Triangle& triangle = triangles[triangleIndex];

			bool assigned = false;
			for (int subNodeIndex = 0; subNodeIndex < CHILD_SUBNODES_COUNT; ++subNodeIndex)
			{
				if (triangle.overlapsWithAABB(nodesDomains[subNodeIndex].first, nodesDomains[subNodeIndex].second))
				{
					nodesTriangles[subNodeIndex].push_back(triangle);
					assigned = true;
					++assignments;
				}
			}
		}

		// Create subnodes
		for (int i = 0; i < CHILD_SUBNODES_COUNT; ++i)
		{
			m_Subnodes[i] = new OctreeNode(
				nodesDomains[i].first,			// min domain
				nodesDomains[i].second,			// max domain
				nodesTriangles[i],				// set of triangles
				depth + 1						// deeper depth
			);
			m_TrianglesInclusiveCount += m_Subnodes[i]->m_TrianglesInclusiveCount;
		}
	}
	else
	{
		// We won't divide deeper so gimme teh your triangles.
		m_Triangles.assign(triangles.begin(), triangles.end());
		m_TrianglesInclusiveCount = m_Triangles.size();
		m_isLeaf = true;
	}
}

void Octree::OctreeNode::updateNearNodes(bool isRoot)
{
	if (!m_isLeaf)
	{
		for (int i = 0; i < CHILD_SUBNODES_COUNT; ++i)
		{
			bool onTop = false;
			bool onLeftEdge = false;
			bool onRightEdge = false;
			bool onBottom = false;

			//m_Subnodes[i]->m_NearNodes[NEAR_NODE_ABOVE_INDEX] = isRoot && onTop ? nullptr : m_Subnodes[i^2];
		}
	}
}


///////////////////////////////////////////////////////////////

Octree::Octree() : 
	m_Triangles(),
	m_pRoot(std::nullptr_t()),
	m_MaxDomain(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity()),
	m_MinDomain(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_DomainSize(0, 0, 0),
	m_SmallestNodeDivide(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity())
{
}

void Octree::buildTree(const std::vector<Triangle>& triangles, const Point3d& minDomain, const Point3d& maxDomain)
{
	// Create root node. Save domain boundaries and build whole tree!
	m_pRoot.reset(new OctreeNode(minDomain, maxDomain, triangles));

	//m_pRoot->updateNearNodes(true);
	// TODO
	// in the end update near nodes
}

void Octree::setObserverPoint(const Point3d& point)
{
	m_ObserverPoint = point;
}

bool Octree::castRayForTriangle(const Vector3d& ray, Triangle& out_triangle) const
{
	//TODO cast ray from observer point

	return false;
}