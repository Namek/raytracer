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
	m_DomainSize(0, 0, 0),
	m_MaxDivideDepth(0)
{
}

Octree::OctreeNode::OctreeNode(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int maxDivideDepth, int currentDivideDepth) :
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
	divide(minDomain, maxDomain, triangles, maxDivideDepth, currentDivideDepth);
}

bool Octree::OctreeNode::containsPoint(const Point3d& point, float epsilon) const
{
	return point.isInAABB(m_MinDomain, m_MaxDomain, epsilon);
}

void Octree::OctreeNode::divide(Point3d minDomain, Point3d maxDomain, const std::vector<Triangle>& triangles, int maxDivideDepth, int currentDepth)
{	
	float epsilon = currentDepth == 0 ? 0.00001f :  0.000001f;
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
	
	if (currentDepth+1 <= maxDivideDepth)
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
				// TODO: Stupid FIX!! It shall be eleminated.
				for (int subNodeIndex = 0; subNodeIndex < CHILD_SUBNODES_COUNT; ++subNodeIndex)
				{
					nodesTriangles[subNodeIndex].push_back(triangle);
				}

				//printf("N %d\n", currentDepth);//debug
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
				maxDivideDepth,
				currentDepth + 1				// deeper depth
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
	m_MaxDivideDepth(0),
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
	m_pRoot.reset(new OctreeNode(minDomain, maxDomain, triangles, m_MaxDivideDepth));
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
		const Plane* planes = m_pRoot->m_DomainPlanes.get();
		float smallestDist = numeric_limits<float>::max();
		Point3d nearestIntersectionPoint;
		Point3d intersectionPoint;

		for (int i = 0; i < 6; ++i)
		{
			const Plane& plane = planes[i];
			float distance = plane.intersectLine(rayOrigin, rayDirection, intersectionPoint);

			if (distance > 0)
			{
				// Check if point is contained in the rectangle (plane has infinite size)
				if (m_pRoot->containsPoint(intersectionPoint))
				{
					// Choose a point with the shortest distance
					if (distance < smallestDist && distance > 0)
					{
						smallestDist = distance;
						nearestIntersectionPoint = intersectionPoint;
					}
				}
			}
		}

		// The point with the shortest distance to ray origin is the point lying on the boundary of the domain
		currentRayOrigin = rayOrigin + rayDirection * smallestDist*1.000001f;

		// If point still is outside of the domain then there's no ray-triangle intersection
		if (currentRayOrigin.x < m_MinDomain.x || currentRayOrigin.x > m_MaxDomain.x ||
			currentRayOrigin.y < m_MinDomain.y || currentRayOrigin.y > m_MaxDomain.y ||
			currentRayOrigin.z < m_MinDomain.z || currentRayOrigin.z > m_MaxDomain.z)
		{
			return;
		}
	}

	pair<Triangle, Point3d> triangleWithIntersectionPoint;
	Vector3d currentRayDirection = rayDirection;
	int rayNumber = 1;

	while (rayNumber++ <= MAX_SECONDARY_RAYS_NUMBER && castRayForTriangle(currentRayOrigin, currentRayDirection, triangleWithIntersectionPoint))
	{
		// Store found triangle and ray-triangle intersection point
		out_intersectedTriangles.push_back(triangleWithIntersectionPoint);

		// New ray's origin is the intersection point of old ray and some triangle
		currentRayOrigin = triangleWithIntersectionPoint.second;
		
		// Calcule new ray's direction
		const Triangle& triangle = triangleWithIntersectionPoint.first;
		Vector3d currentRayDirection = triangle.norm * 2 * triangle.norm.dotProduct(currentRayDirection) - currentRayDirection;
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
		correctedRayOrigin.x = m_MinDomain.x - rayOrigin.x + m_MaxDomain.x;
		correctedRayDirection.x = - rayDirection.x;
		indexSwapper |= 4;
	}

	if (rayDirection.y < 0)
	{
		correctedRayOrigin.y = m_MinDomain.y + m_MaxDomain.y - rayOrigin.y;
		correctedRayDirection.y = - rayDirection.y;
		indexSwapper |= 2;
	}

	if (rayDirection.z < 0)
	{
		correctedRayOrigin.z = m_MinDomain.z + m_MaxDomain.z - rayOrigin.z;
		correctedRayDirection.z = - rayDirection.z;
		indexSwapper |= 1;
	}

	correctedRayDirection.normalize();

	// Calculate parametric equations for ray
	float tx0 = (m_MinDomain.x - correctedRayOrigin.x) / correctedRayDirection.x;
	float ty0 = (m_MinDomain.y - correctedRayOrigin.y) / correctedRayDirection.y;
	float tz0 = (m_MinDomain.z - correctedRayOrigin.z) / correctedRayDirection.z;
	float tx1 = (m_MaxDomain.x - correctedRayOrigin.x) / correctedRayDirection.x;
	float ty1 = (m_MaxDomain.y - correctedRayOrigin.y) / correctedRayDirection.y;
	float tz1 = (m_MaxDomain.z - correctedRayOrigin.z) / correctedRayDirection.z;

	// Don't check ray-triangle intersections until the starting node isn't found
	if (max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1)))
	{
		std::vector<std::pair<Triangle, Point3d>> foundTriangles;
		procSubtree(tx0, ty0, tz0, tx1, ty1, tz1, m_pRoot.get(), indexSwapper, rayOrigin, rayDirection, foundTriangles);

		if (foundTriangles.size() > 0)
		{
			float smallestDist = numeric_limits<float>::max();
			int retIndex = 0;

			for (int i = 0; i < (int)foundTriangles.size(); ++i)
			{
				float dist = (foundTriangles[i].second - rayOrigin).length();
				if (dist < smallestDist)
				{
					smallestDist = dist;
					retIndex = i;
				}
			}

			triangleWithIntersectionPoint = foundTriangles[retIndex];
			return true;
		}
	}

	return false;
}

// Returns true if there's an ray-triangle intersection.
bool Octree::procSubtree(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, const OctreeNode* node,
	int indexSwapper, const Point3d& rayOrigin, const Vector3d& rayDirection, std::vector<std::pair<Triangle, Point3d>>& foundTriangles) const
{
	if (tx1 < 0 || ty1 < 0 || tz1 < 0)
		return false;


	if (node->m_isLeaf)
	{
		bool foundTriangle = false;
		float minDistance = numeric_limits<float>::max();
		std::pair<Triangle, Point3d> triangleWithIntersectionPoint;

		// Check ray's collision with triangles in the leaf node
		for (int i = 0, n = node->m_Triangles.size(); i < n; ++i)
		{
			// Test ray-triangle intersection.
			const Triangle& triangle = node->m_Triangles[i];

			float intersectionDist;
			Triangle microTriangle;

			// TODO leave only first subcondition
			if (triangle.hasDisplacement && triangle.texture->currentX == /*389*/111394 && triangle.ind == 3270)
			{
				intersectionDist = triangle.displacedIntersection(rayOrigin, rayDirection, microTriangle);
			}
			else
			{
				intersectionDist = triangle.intersection(rayOrigin, rayDirection);
			}

			if (intersectionDist > 0 && intersectionDist < minDistance)// && node->containsPoint(intersectionPoint, 0.000001f))
			{
				foundTriangle = true;
				const Point3d intersectionPoint = rayOrigin + rayDirection*intersectionDist;
				const Triangle& intersectedTriangle = /*triangle.hasDisplacement ? microTriangle :*/ triangle;

				triangleWithIntersectionPoint = std::pair<Triangle, Point3d>(intersectedTriangle, intersectionPoint);
				minDistance = intersectionDist;
			}
		}

		if (foundTriangle)
			foundTriangles.push_back(triangleWithIntersectionPoint);

		return false;
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
			case 0: if (procSubtree(tx0,ty0,tz0, txm,tym,tzm, node->m_Subnodes[0^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(txm,tym,tzm,4,2,1);
					break;

			case 1: if (procSubtree(tx0,ty0,tzm, txm,tym,tz1, node->m_Subnodes[1^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(txm,tym,tz1,5,3,8);
					break;

			case 2: if (procSubtree(tx0,tym,tz0, txm,ty1,tzm, node->m_Subnodes[2^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;				
					currNode = nextNode(txm,ty1,tzm,6,8,3);
					break;

			case 3: if (procSubtree(tx0,tym,tzm, txm,ty1,tz1, node->m_Subnodes[3^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(txm,ty1,tz1,7,8,8);
					break;

			case 4: if (procSubtree(txm,ty0,tz0, tx1,tym,tzm, node->m_Subnodes[4^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(tx1,tym,tzm,8,6,5);
					break;

			case 5: if (procSubtree(txm,ty0,tzm, tx1,tym,tz1, node->m_Subnodes[5^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(tx1,tym,tz1,8,7,8);
					break;

			case 6: if (procSubtree(txm,tym,tz0, tx1,ty1,tzm, node->m_Subnodes[6^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
						return true;
					currNode = nextNode(tx1,ty1,tzm,8,8,7);
					break;

			case 7: if (procSubtree(txm,tym,tzm, tx1,ty1,tz1, node->m_Subnodes[7^indexSwapper], indexSwapper, rayOrigin, rayDirection, foundTriangles))
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
		nodeIndex |= (txm < tz0) << 2;
		nodeIndex |= (tym < tz0) << 1;
	}

	// Entry plane: XOZ
	else if (tmax == ty0)
	{
		nodeIndex |= (txm < ty0) << 2;
		nodeIndex |= (tzm < ty0) << 0;
	}

	// Entry plane: YOZ
	else if (tmax == tx0)
	{
		nodeIndex |= (tym < tx0) << 1;
		nodeIndex |= (tzm < tx0) << 0;
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
