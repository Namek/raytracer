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
	Point3d tmp = (p2-p1).crossProduct(p3-p1, false);
	float D = -(tmp.x * p1.x + tmp.y * p1.y + tmp.z * p1.z);

	setGeneral(tmp.x, tmp.y, tmp.z, D);
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