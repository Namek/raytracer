#include "Vector3d.h"


using namespace nprt;


Vector3d::Vector3d(void)
{
	x = y = z = 0;
}

Vector3d::Vector3d(float x1, float y1, float z1, bool nor)
{
	x = x1;
	y = y1;
	z = z1;	

	if (nor)
	{
		float len = this->length();
		x /= len;
		y /= len;
		z /= len;
	}
}

Vector3d::Vector3d(Point3d& p1, Point3d& p2, bool nor)
{
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

Vector3d::Vector3d(float yaw, float pitch)
{
	set(yaw, pitch);
}

void Vector3d::set(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Vector3d::set(float yaw, float pitch)
{
	this->x = -cos(pitch) * sin(yaw);
	this->y = -sin(pitch);
	this->z =  cos(pitch) * cos(yaw);
	normalize();
}

float Vector3d::dotProduct(const Vector3d& vector) const
{
	return x*vector.x + y*vector.y + z*vector.z;
}

Vector3d Vector3d::crossProduct(const Vector3d& vector, bool nor) const
{
	float xC = y*vector.z - z*vector.y;
	float yC = z*vector.x - x*vector.z;
	float zC = x*vector.y - y*vector.x;

	return Vector3d(xC, yC, zC, nor);
}

void Vector3d::rotateX(float pitch)
{
	float Y = y * cos(pitch) - z * sin(pitch);
	this->z = y * sin(pitch) + z * cos(pitch);
	this->y = Y;
}

void Vector3d::rotateY(float yaw)
{
	float X = x * cos(yaw) + z * sin(yaw);
	this->z =-y * sin(yaw) + z * cos(yaw);
	this->x = X;
}

void Vector3d::rotateZ(float roll)
{
	float X = x * cos(roll) - y * sin(roll);
	this->y = x * sin(roll) + y * cos(roll);
	this->x = X;
}

bool Vector3d::isInAABB(const Point3d& minDomain, const Point3d& maxDomain, float epsilon) const
{
	return x >= minDomain.x-epsilon && x <= maxDomain.x+epsilon
		&& y >= minDomain.y-epsilon && x <= maxDomain.y+epsilon
		&& z >= minDomain.z-epsilon && x <= maxDomain.z+epsilon;
}

float Vector3d::length()
{
	return (float)sqrt(x*x + y*y + z*z);
}

void Vector3d::normalize()
{
	float l = this->length();
	this->x /= l;
	this->y /= l;
	this->z /= l;
}


