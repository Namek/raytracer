#include "Triangle.h"
#include "Plane.h"

#include <algorithm>
#include <tuple>

using namespace nprt;


static const float g_M = 0.1f;					//
static const float g_m = 0.0f;					// non-negative values
static const float g_heightSweep = g_M + g_m;	//


float displacementHeight(const Texture* texture, float u, float v)
{
	const Vector3d& color = texture->GetTexel(u, v);
	return (color.x*0.3f + color.y*0.59f + color.z*0.11f) * g_heightSweep - g_m;
}

float displacedIntersection(const Vector3d& rayOrigin, const Vector3d& rayDirection,
	const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& normal,
	const Texture* texture, Triangle& out_Triangle)
{
	// We assume that parameters p1, p2 and p3 are given such as:
	//  * p1 - is the point where angle is 90-degree
	//  * p2 and p3 - the rest of points

	Plane thisTriangle(p1, p2, p3);

	// Firstly, find the closest and the furthest intersection points with top, bottom and side segments.
	Vector3d topCapPoints[] = { p1 + normal*g_M, p2 + normal*g_M, p3 + normal*g_M };
	Vector3d bottomCapPoints[] = { p1 - normal*g_m, p2 - normal*g_m, p3 - normal*g_m };
	Vector3d hypotenuseSideSegmentPoints[] = { p2 + normal*g_M, p2 - normal*g_m, p3 + normal*g_M, p3 - normal*g_m };
	Vector3d otherSideSegment1[] = { p2 + normal*g_M, p2 - normal*g_m, p1 + normal*g_M, p1 - normal*g_m };
	Vector3d otherSideSegment2[] = { p1 + normal*g_M, p1 - normal*g_m, p3 + normal*g_M, p3 - normal*g_m };

	Triangle topCap(topCapPoints[0], topCapPoints[1], topCapPoints[2]);
	Triangle bottomCap(bottomCapPoints[0], bottomCapPoints[1], bottomCapPoints[2]);
	Plane hypotenuseSidePlane(hypotenuseSideSegmentPoints[0], hypotenuseSideSegmentPoints[1], hypotenuseSideSegmentPoints[2]);// side opposite the right angle
	Plane otherSidePlane1(otherSideSegment1[0], otherSideSegment1[1], otherSideSegment1[2]);		// side having right angle
	Plane otherSidePlane2(otherSideSegment2[0], otherSideSegment2[1], otherSideSegment2[2]);		// side having right angle
	
	Vector3d topIntersectionPoint;
	Vector3d bottomIntersectionPoint;
	Vector3d hypotenuseIntersectionPoint;
	Vector3d otherSide1IntersectionPoint;
	Vector3d otherSide2IntersectionPoint;

	float hypotenuseIntersectionDistance = hypotenuseSidePlane.intersectLineInSegment(rayOrigin, rayDirection, hypotenuseSideSegmentPoints[0],
		hypotenuseSideSegmentPoints[1], hypotenuseSideSegmentPoints[2], hypotenuseSideSegmentPoints[3], hypotenuseIntersectionPoint);
	float otherSide1IntersectionDistance = otherSidePlane1.intersectLineInSegment(rayOrigin, rayDirection, 
		otherSideSegment1[0], otherSideSegment1[1], otherSideSegment1[2], otherSideSegment1[3], otherSide1IntersectionPoint);
	float otherSide2IntersectionDistance = otherSidePlane2.intersectLineInSegment(rayOrigin, rayDirection, 
		otherSideSegment2[0], otherSideSegment2[1], otherSideSegment2[2], otherSideSegment2[3], otherSide2IntersectionPoint);
	
	float topIntersectionDistance = topCap.intersection(rayOrigin, rayDirection);
	float bottomIntersectionDistance = bottomCap.intersection(rayOrigin, rayDirection);
	topIntersectionPoint = rayOrigin + rayDirection * topIntersectionDistance;
	bottomIntersectionPoint = rayOrigin + rayDirection * bottomIntersectionDistance;

	// Sort planes by the distance to the viewer
	typedef enum PlaneType
	{
		TopCap = 0,
		BottomCap = 1,
		iPlane = 2,	// side opposite the right angle
		jPlane = 3,
		kPlane = 4,
		PlaneTypesCount = 5
	};
	typedef std::tuple<void*, float, Vector3d, PlaneType> PlaneIntersectionInfo;	
	#define IntersectionInfo_Plane 0
	#define IntersectionInfo_Distance 1
	#define IntersectionInfo_Point 2
	#define IntersectionInfo_PlaneType 3
	

	PlaneIntersectionInfo intersections[] = {
		PlaneIntersectionInfo(&topCap, topIntersectionDistance, topIntersectionPoint, TopCap),
		PlaneIntersectionInfo(&bottomCap, bottomIntersectionDistance, bottomIntersectionPoint, BottomCap),
		PlaneIntersectionInfo(&hypotenuseSidePlane, hypotenuseIntersectionDistance, hypotenuseIntersectionPoint, iPlane),
		PlaneIntersectionInfo(&otherSidePlane1, otherSide1IntersectionDistance, otherSide1IntersectionPoint, jPlane),
		PlaneIntersectionInfo(&otherSidePlane2, otherSide2IntersectionDistance, otherSide2IntersectionPoint, kPlane),
	};

	std::sort(intersections, intersections+PlaneTypesCount-1,
		[](const PlaneIntersectionInfo& a, const PlaneIntersectionInfo& b) -> bool
		{
			return std::get<IntersectionInfo_Distance>(a) < std::get<IntersectionInfo_Distance>(b);
		}
	);

	// Choose the closest plane and based on it's intersection point calculate the point placed on the triangle to be displaced.	
	auto castPointOnTriangle = [normal, thisTriangle](const PlaneIntersectionInfo& intersectionInfo, Vector3d& out_castedPoint)
	{
		Vector3d testPoint1, testPoint2;
		const Vector3d& intersectionPoint = std::get<IntersectionInfo_Point>(intersectionInfo);
		float distanceToTriangle = thisTriangle.distance(intersectionPoint);

		Vector3d samplePoint;
		if (thisTriangle.intersectLine(Vector3d(), normal, samplePoint) < 0)
			thisTriangle.intersectLine(Vector3d(), normal, samplePoint);

		testPoint1.set(samplePoint + normal);
		testPoint1.set(samplePoint - normal);
				
		float testDistance1 = (intersectionPoint - testPoint1).length();
		float testDistance2 = (intersectionPoint - testPoint2).length();
		float normalMultiplier = distanceToTriangle * (testDistance1 < testDistance2 ? 1 : -1);
		out_castedPoint.set(intersectionPoint - normal * normalMultiplier);
	};
	
	// Calculate i,j and k
	Vector3d castedPoint;
	int closestIntersectionIndex = 0;

	while (closestIntersectionIndex < PlaneTypesCount)
	{
		if (std::get<IntersectionInfo_Distance>(intersections[closestIntersectionIndex]) < 0)
			closestIntersectionIndex++;
		else
			break;
	}

	if (closestIntersectionIndex == PlaneTypesCount)
	{
		// No intersection was found.
		// Ray omits the triangle and it's displacement volume box.
		return -1;
	}
	else if (closestIntersectionIndex == PlaneTypesCount-1)
	{
		// Ray enters the volume box and doesn't exit. Something is terribly wrong!
		_asm int 3h;
	}

	castPointOnTriangle(intersections[closestIntersectionIndex], castedPoint);

	int N = 3;
	auto calculate_ijk = [N, p1, p2, p3, castedPoint](int& i, int& j, int& k)
	{
		float u, v;
		Triangle::getUV(p1, p2, p3, castedPoint, u, v);

		// max distance from the right-angle point to the hypotenuse line
		float maxDist = Vector3d::pointLineDistance(p1, p2, p3);
		float curDist = Vector3d(p2, p3).pointLineDistance(castedPoint);

		float alpha = curDist / maxDist;	
		float beta = u;
		float gamma = v;

		i = alpha * static_cast<float>(N);
		j = beta * static_cast<float>(N);
		k = gamma * static_cast<float>(N);
	};
		
	
	// Now determine the ending indices i,j,k (ie,je,ke)
	int i, j, k;
	calculate_ijk(i, j, k);

	int ie, je, ke;
	castPointOnTriangle(intersections[4], castedPoint);
	calculate_ijk(ie, je, ke);
	

	// TODO Calculate vertices for first micro-triangle (don't displace a and b; displace only c)
	Vector3d a;
	Vector3d b;
	Vector3d c;
	Vector3d currentNormal = (p2 - p1).crossProduct(p3 - p1, true);

	typedef enum LastChange
	{
		iplus = 0,
		jminus,
		kplus,
		iminus,
		jplus,
		kminus
	};

	LastChange change = (i == 0 ? iplus : (j == 0 ? jplus : kplus));
	float u = static_cast<float>(j) / static_cast<float>(N);
	float v = static_cast<float>(k) / static_cast<float>(N);

	// Trace ray through the big triangle
	while (true)
	{
		float intersectionDist = Triangle::intersection(a, b, c, rayOrigin, rayDirection);
		if (intersectionDist > 0)
		{
			out_Triangle = Triangle(a, b, c, -1);
			return intersectionDist;
		}

		if (i == ie && j == je && k == ke)
		{
			// Exit, there was no intersection with any micro-triangle
			break;
		}

		// Determine on which side of point c the ray is traversing
		float tmp = (currentNormal.crossProduct(rayOrigin - c, true)).dotProduct(rayDirection);
		bool rightOfC = (tmp > - std::numeric_limits<float>::epsilon());
		
		if (rightOfC)
		{
			b = c;
			
			// Lower micro-triangle
			if (change == iplus)
			{
				--j;
				change = jminus;
			}
			else if (change == jplus)
			{
				--k;
				change = kminus;
			}
			else if (change == kplus)
			{
				--i;
				change = iminus;
			}

			// Upper micro-triangle
			else if (change == iminus)
			{
				++j;
				change = jplus;
			}
			else if (change == jminus)
			{
				++k;
				change = kplus;
			}
			else if (change == kminus)
			{
				++i;
				change = iplus;
			}
		}
		else
		{
			a = c;

			// Lower micro-triangle
			if (change == iplus)
			{
				--k;
				change = kminus;
			}
			else if (change == jplus)
			{
				--i;
				change = iminus;
			}
			else if (change == kplus)
			{
				--j;
				change = jminus;
			}

			// Upper micro-triangle
			else if (change == iminus)
			{
				++k;
				change = kplus;
			}
			else if (change == jminus)
			{
				++i;
				change = iplus;
			}
			else if (change == kminus)
			{
				++j;
				change = jplus;
			}
		}

		// Calculate new point c with the displacement from the base triangle
		// TODO based on ijk
		u = static_cast<float>(j) / static_cast<float>(N);
		v = static_cast<float>(k) / static_cast<float>(N);
		float height = displacementHeight(texture, u, v);

	}

	return -1;
}

// Gets triangle from flyweight or creates a new one and stores it, then returns it
float Triangle::displacedIntersection(const Vector3d& rayOrigin, const Vector3d& rayDirection, Triangle& out_Triangle) const
{
	float distance;
	Vector3d largestAnglePoint, otherPoint1, otherPoint2;

	if (isRightTriangle(p1, p2, p3, largestAnglePoint, otherPoint1, otherPoint2))
	{
		distance = ::displacedIntersection(rayOrigin, rayDirection, largestAnglePoint, otherPoint1, otherPoint2, norm, texture, out_Triangle);

		if (distance > 0)
			return distance;
	}
	else
	{
		// Cut triangle by casting a ray from the vertex having the largest angle		
		const Vector3d& V1 = otherPoint1;
		const Vector3d& V2 = otherPoint2;
		const Vector3d& V3 = largestAnglePoint;
				
		Vector3d V1V2 = V1 - V2;
		float u = (V3 - V1).dotProduct(V2 - V1) / (V1V2).dotProduct(V1V2);
		Vector3d cutPoint = V1 - V1V2*u;

		Vector3d normal = (V1 - cutPoint).crossProduct(V1 - V3, true);
		distance = ::displacedIntersection(rayOrigin, rayDirection, V1, V3, cutPoint, normal, texture, out_Triangle);

		if (distance > 0)
			return distance;
		else
		{
			normal = (V3 - cutPoint).crossProduct(V3 - V2, true);
			return ::displacedIntersection(rayOrigin, rayDirection, V3, V2, cutPoint, normal, texture, out_Triangle);
		}
	}
	
	return -1;
}