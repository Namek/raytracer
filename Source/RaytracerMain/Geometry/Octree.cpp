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

Octree::OctreeNode* Octree::OctreeNode::findNode(const Vector3d& point, const Vector3d& rayDir)
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
		p = point + rayDir*dist;
	}
	
	// Now we have point inside of the domain.
	// Let's find out in which node the point 'p' is located.
	return findNode(p);
}

// We assume that the point is located inside of the current domain.
Octree::OctreeNode* Octree::OctreeNode::findNode(const Vector3d& point)
{
	OctreeNode* node = this;
	
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
	// Save domain
	m_MinDomain = minDomain;
	m_MaxDomain = maxDomain;
	m_DomainSize = maxDomain - minDomain;

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


bool Octree::castRayForTriangle(const Vector3d& rayDir, Triangle& out_triangle)
{
	indexSwapper = 0;
	Vector3d rayOrigin = m_ObserverPoint;

	if (rayDir.x < 0)
	{
		rayOrigin.x = m_DomainSize.x - m_ObserverPoint.x;
		rayOrigin.x = - rayOrigin.x;
		indexSwapper |= 4;
	}

	if (rayDir.y < 0)
	{
		rayOrigin.y = m_DomainSize.y - m_ObserverPoint.y;
		rayOrigin.y = - rayOrigin.y;
		indexSwapper |= 2;
	}

	if (rayDir.z < 0)
	{
		rayOrigin.z = m_DomainSize.z - m_ObserverPoint.z;
		rayOrigin.z = - rayOrigin.z;
		indexSwapper |= 1;
	}

	float tx0 = (m_MinDomain.x - rayOrigin.x) / rayDir.x;
	float tx1 = (m_MaxDomain.x - rayOrigin.x) / rayDir.x;
	float ty0 = (m_MinDomain.y - rayOrigin.y) / rayDir.y;
	float ty1 = (m_MaxDomain.y - rayOrigin.y) / rayDir.y;
	float tz0 = (m_MinDomain.z - rayOrigin.z) / rayDir.z;
	float tz1 = (m_MaxDomain.z - rayOrigin.z) / rayDir.z;

	if (max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1)))
		procSubtree(tx0, ty0, tz0, tx1, ty1, tz1, m_pRoot.get());

	return false;
}

// Implements tables 1 and 2 [Revelles95].
int Octree::firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) const
{
	float tmax = max(tx0, max(ty0, tz0));
	int nodeIndex = 0;

	// Entry plane: XOY
	if (tmax == tz0)
	{
		nodeIndex |= (txm < tz0) << 0;
		nodeIndex |= (tym < tz0) << 1;
	}

	// Entry plane: XOZ
	else if (tmax == ty0)
	{
		nodeIndex |= (txm < ty0) << 0;
		nodeIndex |= (tzm < ty0) << 2;
	}

	// Entry plane: YOZ
	else if (tmax == tx0)
	{
		nodeIndex |= (tym < tx0) << 1;
		nodeIndex |= (tzm < tx0) << 2;
	}

	return nodeIndex;
}

// Returns the i-th integer where the i-th float value is the minimum of the three float values.
inline int Octree::nextNode(float tx, float ty, float tz, int ix, int iy, int iz) const
{
	int ret;

	if (tx < ty)
	{
		if (tx < tz)
			ret = ix;
		else
			ret = iz;
	}
	else
	{
		if (ty < tz)
			ret = iy;
		else
			ret = iz;
	}

	return ret;
}

void Octree::procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node)
{	
	if (tx1 < 0 || ty1 < 0 || tz1 < 0)
		return;


	if (node->m_isLeaf)
	{
		// TODO check ray collision with ray
		// proc_terminal(n)
		return;
	}

	float txm = 0.5f * (tx0 + tx1);
	float tym = 0.5f * (ty0 + ty1);
	float tzm = 0.5f * (tz0 + tz1);

	int currNode = firstNode(tx0, ty0, tz0, txm, tym, tzm); 

	do
	{
		switch(currNode)
		{
			case 0: procSubtree(tx0,ty0,tz0,txm,tym,tzm, node->m_Subnodes[indexSwapper]);
					currNode = nextNode(txm,tym,tzm,4,2,1);
					break;
			case 1: procSubtree(tx0,ty0,tzm,txm,tym,tz1, node->m_Subnodes[1^indexSwapper]);
					currNode = nextNode(txm,tym,tz1,5,3,8);
					break;
			case 2: procSubtree(tx0,tym,tz0,txm,ty1,tzm, node->m_Subnodes[2^indexSwapper]);
					currNode = nextNode(txm,ty1,tzm,6,8,3);
					break;
			case 3: procSubtree(tx0,tym,tzm,txm,ty1,tz1, node->m_Subnodes[3^indexSwapper]);
					currNode = nextNode(txm,ty1,tz1,7,8,8);
					break;
			case 4: procSubtree(txm,ty0,tz0,tx1,tym,tzm, node->m_Subnodes[4^indexSwapper]);
					currNode = nextNode(tx1,tym,tzm,8,6,5);
					break;
			case 5: procSubtree(txm,ty0,tzm,tx1,tym,tz1, node->m_Subnodes[5^indexSwapper]);
					currNode = nextNode(tx1,tym,tz1,8,7,8);
					break;
			case 6: procSubtree(txm,tym,tz0,tx1,ty1,tzm, node->m_Subnodes[6^indexSwapper]);
					currNode = nextNode(tx1,ty1,tzm,8,8,7);
					break;
			case 7: procSubtree(txm,txm,tzm,tx1,ty1,tz1, node->m_Subnodes[7^indexSwapper]);
					currNode = 8;
					break;
		}
	} while (currNode < 8); 
}