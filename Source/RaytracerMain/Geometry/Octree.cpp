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

bool Octree::OctreeNode::containsPoint(const Point3d& point) const
{
	return point.isInAABB(m_MinDomain, m_MaxDomain);
}

void Octree::OctreeNode::divide(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int depth)
{	
	float epsilon = depth == 0 ? 0.000001f : 0;
	Vector3d epsilonVect = Vector3d(epsilon, epsilon, epsilon);

	m_MinDomain = minDomain - epsilonVect;
	m_MaxDomain = maxDomain + epsilonVect;
	m_DomainSize = m_MaxDomain - m_MinDomain;

	// Calculate planes for this domain.
	m_DomainPlanes[DOMAIN_PLANE_BOTTOM] = Plane(m_MinDomain, Vector3d(0, 1, 0));
	m_DomainPlanes[DOMAIN_PLANE_TOP]	= Plane(m_MaxDomain, Vector3d(0, -1, 0));
	m_DomainPlanes[DOMAIN_PLANE_LEFT]	= Plane(m_MinDomain, Vector3d(1, 0, 0));
	m_DomainPlanes[DOMAIN_PLANE_RIGHT]	= Plane(m_MaxDomain, Vector3d(-1, 0, 0));
	m_DomainPlanes[DOMAIN_PLANE_NEAR]	= Plane(m_MinDomain, Vector3d(0, 0, 1));
	m_DomainPlanes[DOMAIN_PLANE_FAR]	= Plane(m_MaxDomain, Vector3d(0, 0, -1));
	
	if (depth+1 <= MAX_DIVIDE_DEPTH)
	{
		std::vector<Triangle> nodesTriangles[CHILD_SUBNODES_COUNT];
		pair<Point3d, Point3d> nodesDomains[CHILD_SUBNODES_COUNT];
		Vector3d halfDomainSize = m_DomainSize * 0.5f;
		Point3d center = m_MinDomain + halfDomainSize;

		// Calculate subboxes' domains
		nodesDomains[CHILD_LOWER_NEAR_LEFT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain, center);
		nodesDomains[CHILD_LOWER_FAR_LEFT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(0, 0, halfDomainSize.z), center + Vector3d(0, 0, halfDomainSize.z));
		nodesDomains[CHILD_UPPER_NEAR_LEFT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(0, halfDomainSize.y, 0), center + Vector3d(0, halfDomainSize.y, 0));
		nodesDomains[CHILD_UPPER_FAR_LEFT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(0, halfDomainSize.y, halfDomainSize.z), center + Vector3d(0, halfDomainSize.y, halfDomainSize.z));

		nodesDomains[CHILD_LOWER_NEAR_RIGHT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(halfDomainSize.x, 0, 0), center + Vector3d(halfDomainSize.x, 0, 0));
		nodesDomains[CHILD_LOWER_FAR_RIGHT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(halfDomainSize.x, 0, halfDomainSize.z), center + Vector3d(halfDomainSize.x, 0, halfDomainSize.z));
		nodesDomains[CHILD_UPPER_NEAR_RIGHT_INDEX]	= pair<Point3d, Point3d>(m_MinDomain + Vector3d(halfDomainSize.x, halfDomainSize.y, 0), center + Vector3d(halfDomainSize.x, halfDomainSize.y, 0));
		nodesDomains[CHILD_UPPER_FAR_RIGHT_INDEX]	= pair<Point3d, Point3d>(center, m_MaxDomain);

		
		for (int i = 0; i < CHILD_SUBNODES_COUNT; ++i)
		{
			nodesDomains[i] = pair<Point3d, Point3d>(nodesDomains[i].first - epsilonVect, nodesDomains[i].second + epsilonVect);
		}


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

			if (!assigned)
			{
				printf("N %d\n", depth);//debug
				// TODO If triangle wasn't assigned anywhere,
				// then probably it's a wall (or floor or ceiling) of a domain.

				float minDistance = numeric_limits<float>::max();
				Point3d triangleCenter = ((triangle.p1 + triangle.p2) * 0.5f + triangle.p3) * 0.5f;

				for (int subNodeIndex = 0; subNodeIndex < CHILD_SUBNODES_COUNT; ++subNodeIndex)
				{
					//float distance 
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
		}
	}
	else
	{
		// We won't divide deeper so gimme teh your triangles.
		m_Triangles.assign(triangles.begin(), triangles.end());
		m_isLeaf = true;
	}
	
	m_TrianglesInclusiveCount = m_Triangles.size();
}

// Deprecated
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
	m_pRoot(std::nullptr_t()),
	m_MaxDomain(-numeric_limits<float>::infinity(), -numeric_limits<float>::infinity(), -numeric_limits<float>::infinity()),
	m_MinDomain(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_DomainSize(0, 0, 0),
	m_SmallestNodeDivide(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), numeric_limits<float>::infinity()),
	m_ObserverPoint(),
	m_NearestPlane(),
	m_DistanceToNearestPlane(0)
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

// may be deprecated
void Octree::setObserverPoint(const Point3d& point) const
{
	m_ObserverPoint = point;

	// Find plane nearest to the observer point
	//m_DistanceToNearestPlane = distanceToNearestPlane(m_ObserverPoint);
}

void Octree::traceRayForTriangles(const Point3d& rayOrigin, const Vector3d& rayDirection, std::vector<std::pair<Triangle, Point3d>>& out_intersectedTriangles) const
{
	// Starting point must be located in the domain.
	// If it isn't then cast ray at the domain to calculate the starting point.
	Vector3d currentRayOrigin = rayOrigin;

	// If outside of the domain.
	if (currentRayOrigin.x < m_MinDomain.x || currentRayOrigin.x > m_MaxDomain.x ||
		currentRayOrigin.y < m_MinDomain.y || currentRayOrigin.y > m_MaxDomain.y ||
		currentRayOrigin.z < m_MinDomain.z || currentRayOrigin.z > m_MaxDomain.z)
	{
		// Calculate ray's intersection point with nearest plane
		Plane plane;
		float distance = distanceToNearestPlane(rayOrigin, plane);
		Point3d pointOnPlane = plane.intersectLine(rayOrigin, rayDirection);

		// TODO rethink that thing.
		//currentRayOrigin = pointOnPlane;

		// If point still is outside of the domain then there's no ray-triangle intersection
		if (currentRayOrigin.x < m_MinDomain.x || currentRayOrigin.x > m_MaxDomain.x ||
			currentRayOrigin.y < m_MinDomain.y || currentRayOrigin.y > m_MaxDomain.y ||
			currentRayOrigin.z < m_MinDomain.z || currentRayOrigin.z > m_MaxDomain.z)
		{
			//return;
		}
	}

	pair<Triangle, Point3d> triangleWithIntersectionPoint;
	Vector3d currentRayDirection = rayDirection;
	int rayNumber = 1;

	while (castRayForTriangle(currentRayOrigin, currentRayDirection, triangleWithIntersectionPoint) && ++rayNumber <= MAX_SECONDARY_RAYS_NUMBER)
	{
		out_intersectedTriangles.push_back(triangleWithIntersectionPoint);

		currentRayOrigin = triangleWithIntersectionPoint.second;
		// TODO !!!!!1111 calculate reflection ====> currentRayDirection = ...
		
		break;//TODO REmove that so bad thing!
	}
}

// Returns true if there's an ray-triangle intersection.
bool Octree::castRayForTriangle(const Point3d& rayOrigin, const Vector3d& rayDirection, std::pair<Triangle, Point3d>& triangleWithIntersectionPoint) const
{
	// Swap ray direction and origin due to the domain (X, Y, Z must be non-negative)
	int indexSwapper = 0;
	Vector3d correctedRayOrigin = rayOrigin;
	Vector3d correctedRayDirection = rayDirection;

	if (rayDirection.x < 0)
	{
		correctedRayOrigin.x = m_DomainSize.x - rayOrigin.x;
		correctedRayDirection.x = - rayDirection.x;
		indexSwapper |= 4;
	}

	if (rayDirection.y < 0)
	{
		correctedRayOrigin.y = m_DomainSize.y - rayOrigin.y;
		correctedRayDirection.y = - rayDirection.y;
		indexSwapper |= 2;
	}

	if (rayDirection.z < 0)
	{
		correctedRayOrigin.z = m_DomainSize.z - rayOrigin.z;
		correctedRayDirection.z = - rayDirection.z;
		indexSwapper |= 1;
	}

	// Calculate parametric equations for ray
	float tx0 = (m_MinDomain.x - correctedRayOrigin.x) / correctedRayDirection.x;
	float tx1 = (m_MaxDomain.x - correctedRayOrigin.x) / correctedRayDirection.x;
	float ty0 = (m_MinDomain.y - correctedRayOrigin.y) / correctedRayDirection.y;
	float ty1 = (m_MaxDomain.y - correctedRayOrigin.y) / correctedRayDirection.y;
	float tz0 = (m_MinDomain.z - correctedRayOrigin.z) / correctedRayDirection.z;
	float tz1 = (m_MaxDomain.z - correctedRayOrigin.z) / correctedRayDirection.z;

	//if (max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1)))
	{
		// Don't check ray-triangle intersections until the starting node isn't found
		return procSubtree(tx0, ty0, tz0, tx1, ty1, tz1, m_pRoot.get(), indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint);
	}

	return false;
}

// Returns true if there's an ray-triangle intersection.
bool Octree::procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node,
	int indexSwapper, const Point3d& rayOrigin, const Vector3d& rayDirection, std::pair<Triangle, Point3d>& triangleWithIntersectionPoint) const
{
	//if (tx1 < 0 || ty1 < 0 || tz1 < 0 && !node->m_isLeaf)
	//	return false;


	if (node->m_isLeaf)
	{
		bool foundTriangle = false;
		float minDistance = numeric_limits<float>::max();

		// Check ray's collision with triangles in the leaf node
		for (int i = 0; i < node->m_Triangles.size(); ++i)
		{
			// Test ray-triangle intersection.
			// Note: it may be optimized by using  't' parameter.
			
			const Triangle& triangle = node->m_Triangles[i];

			float intersectionDist = triangle.intersection(rayOrigin, rayDirection);
			if (intersectionDist > -1.0f && intersectionDist < minDistance)// && node->containsPoint(intersectionPoint))
			{
				foundTriangle = true;
				triangleWithIntersectionPoint = std::pair<Triangle, Point3d>(triangle, rayOrigin + rayDirection*intersectionDist);
				minDistance = intersectionDist;
			}
		}

		return foundTriangle;
	}

	// Calculate center of current node
	float txm = 0.5f * (tx0 + tx1);
	float tym = 0.5f * (ty0 + ty1);
	float tzm = 0.5f * (tz0 + tz1);

	int currNode = firstNode(tx0, ty0, tz0, txm, tym, tzm); 

	do
	{
		switch(currNode)
		{
			case 0: if (procSubtree(tx0,ty0,tz0, txm,tym,tzm, node->m_Subnodes[0^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(txm,tym,tzm,4,2,1);
					break;

			case 1: if (procSubtree(tx0,ty0,tzm, txm,tym,tz1, node->m_Subnodes[1^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(txm,tym,tz1,5,3,8);
					break;

			case 2: if (procSubtree(tx0,tym,tz0, txm,ty1,tzm, node->m_Subnodes[2^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;						
					currNode = nextNode(txm,ty1,tzm,6,8,3);
					break;
			case 3: if (procSubtree(tx0,tym,tzm, txm,ty1,tz1, node->m_Subnodes[3^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(txm,ty1,tz1,7,8,8);
					break;

			case 4: if (procSubtree(txm,ty0,tz0, tx1,tym,tzm, node->m_Subnodes[4^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(tx1,tym,tzm,8,6,5);
					break;

			case 5: if (procSubtree(txm,ty0,tzm, tx1,tym,tz1, node->m_Subnodes[5^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(tx1,tym,tz1,8,7,8);
					break;

			case 6: if (procSubtree(txm,tym,tz0, tx1,ty1,tzm, node->m_Subnodes[6^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = nextNode(tx1,ty1,tzm,8,8,7);
					break;

			case 7: if (procSubtree(txm,tym,tzm, tx1,ty1,tz1, node->m_Subnodes[7^indexSwapper], indexSwapper, rayOrigin, rayDirection, triangleWithIntersectionPoint))
						return true;
					currNode = 8;
					break;
		}
	} while (currNode < 8); 

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

float Octree::distanceToNearestPlane(const Point3d& point, Plane& out_nearestPlane) const
{
	Plane* domainPlanes = m_pRoot->m_DomainPlanes.get();
	float smallestDistance = domainPlanes[0].distance(point);
	out_nearestPlane = domainPlanes[0];

	for (int i = 1; i < 6; i++)
	{
		float tmpDistance = domainPlanes[i].distance(point);

		if (tmpDistance < smallestDistance)
		{
			smallestDistance = tmpDistance;
			out_nearestPlane = domainPlanes[i];
		}
	}

	return smallestDistance;
}