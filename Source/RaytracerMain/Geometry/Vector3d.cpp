#include "Vector3d.h"


using namespace nprt;


Vector3d::Vector3d(void)
{
	x = y = z = 0;
}

Vector3d::Vector3d(float x1, float y1, float z1, bool nor) {
	x = x1;
	y = y1;
	z = z1;	
	if (nor)
	{
		float len = this->length();
		x/=len;
		y/=len;
		z/=len;
	}
}

Vector3d::Vector3d(Point3d& p1, Point3d& p2, bool nor) {
	x = p2.x - p1.x;
	y = p2.y - p1.y;
	z = p2.z - p1.z;
	if (nor)
	{
		float len = this->length();
		x/=len;
		y/=len;
		z/=len;
	}
}


float Vector3d::dotProduct(const Vector3d& vector) const {
	return x*vector.x + y*vector.y + z*vector.z;
}

Vector3d Vector3d::crossProduct(const Vector3d& vector, bool nor) const {
	float xC = y*vector.z - z*vector.y;
	float yC = z*vector.x - x*vector.z;
	float zC = x*vector.y - y*vector.x;

	return Vector3d(xC, yC, zC, nor);
}

float Vector3d::length() {
	return (float)sqrt(x*x + y*y + z*z);
}

void Vector3d::normalize() {
	float l = this->length();
	this->x /= l;
	this->y /= l;
	this->z /= l;
}


