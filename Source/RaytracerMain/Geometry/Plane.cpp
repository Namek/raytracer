#include "Plane.h"

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

Plane::Plane(const Point3d& p1, const Point3d& p2, const Point3d& p3)
{
	Point3d N = (p2-p1).crossProduct(p3-p1, true);
	float D = -(N.x * p1.x + N.y * p1.y + N.z * p1.z);

	setGeneral(N.x, N.y, N.z, D);
}

Plane::Plane(const Point3d& point, const Vector3d& normal)
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
	pointOnAxisX = Point3d(a, 0, 0);
	pointOnAxisY = Point3d(0, b, 0);
	pointOnAxisZ = Point3d(0, 0, c);
}

float Plane::distance(const Point3d& point) const
{
	return abs(alfa*point.x + beta*point.y + gamma*point.z + delta);
}

bool Plane::intersectLine(const Point3d& origin, const Point3d& direction, Point3d& out_intersectionPoint) const
{
	float u = A * origin.x + B * origin.y + C * origin.z + D;
	float den = -A * direction.x - B * direction.y - C * direction.z;

	if (den != 0)
	{
		u /= den;
		out_intersectionPoint = origin + direction*u;
		return true;
	}

	return false;
}
