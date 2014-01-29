#include "Plane.h"
#include "Triangle.h"

using namespace nprt;

Plane::Plane()
{
	a = b = c = A = B = C = D = 0;
	alfa = beta = gamma = delta = 0;
	N = 0;
}

Plane::Plane(float A, float B, float C, float D)
{
	setGeneral(A, B, C, D);
}

Plane::Plane(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3)
{
	Vector3d N = (p2-p1).crossProduct(p3-p1, true);
	float D = -(N.x * p1.x + N.y * p1.y + N.z * p1.z);

	setGeneral(N.x, N.y, N.z, D);
}

Plane::Plane(const Vector3d& point, const Vector3d& normal)
{
	const Vector3d& N = normal;
	float D = -(N.x * point.x + N.y * point.y + N.z * point.z);
	setGeneral(N.x, N.y, N.z, D);
}

void Plane::setGeneral(float A, float B, float C, float D)
{
	this->A = A;
	this->B = B;
	this->C = C;
	this->D = D;
	N = sqrt(A*A + B*B + C*C);
	alfa = A/N;
	beta = B/N;
	gamma = C/N;
	delta = D/N;
	a = -D/A;
	b = -D/B;
	c = -D/C;
	normal.set(A, B, C);
	normal.normalize();
	pointOnAxisX = Vector3d(a, 0, 0);
	pointOnAxisY = Vector3d(0, b, 0);
	pointOnAxisZ = Vector3d(0, 0, c);
}

float Plane::distance(const Vector3d& point) const
{
	return abs(alfa*point.x + beta*point.y + gamma*point.z + delta);
}

float Plane::intersectLine(const Vector3d& origin, const Vector3d& direction, Vector3d& out_intersectionPoint) const
{
	float u = A * origin.x + B * origin.y + C * origin.z + D;
	float den = -A * direction.x - B * direction.y - C * direction.z;

	if (den != 0)
	{
		u /= den;
		out_intersectionPoint = origin + direction*u;
		return u;
	}

	return -1;
}

// Note: Rectangle points should be given in clockwise or counter-clockwise order.
float Plane::intersectLineInSegment(const Vector3d& lineOrigin, const Vector3d& lineDirection,
	const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& p4, Vector3d& out_intersectionPoint) const
{
	/*
	float distance = intersectLine(lineOrigin, lineDirection, out_intersectionPoint);

	if (distance < 0)
		return -1;
		
	const Vector3d& p = out_intersectionPoint;

	// define four lines defining the segment of a plane
	Vector3d k1(p1, p3);
	Vector3d k2(p2, p4);
	Vector3d l1(p1, p2);
	Vector3d l2(p3, p4);

	float k1k2Distance = k1.lineLineDistance(p2, p4);
	float l1l2Distance = l1.lineLineDistance(p3, p4);

	if (k1.pointLineDistance(p) < k1k2Distance && k2.pointLineDistance(p) < k1k2Distance
		&& l1.pointLineDistance(p) < l1l2Distance && l2.pointLineDistance(p) < l1l2Distance)
	{
		return distance;
	}	

	return -1;*/

	out_intersectionPoint;
	float distance = Triangle::intersection(p1, p2, p3, lineOrigin, lineDirection, out_intersectionPoint);

	if (distance < 0)
	{
		distance = Triangle::intersection(p1, p3, p4, lineOrigin, lineDirection, out_intersectionPoint);

		if (distance < 0)
			return -1;
	}

	return distance;
}