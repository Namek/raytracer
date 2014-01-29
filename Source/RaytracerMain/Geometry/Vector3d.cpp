#include "Vector3d.h"
#include "../Math/Math.h"
#include "../Math/floats.h"

using namespace nprt;

Vector3d::Vector3d()
{
	x = y = z = 0;
}

Vector3d::Vector3d(float x, float y, float z, bool shouldNormalize)
{
	set(x, y, z);

	if (shouldNormalize)
	{
		normalize();
	}
}

Vector3d::Vector3d(const Vector3d& p1, const Vector3d& p2, bool shouldNormalize)
{
	x = p2.x - p1.x;
	y = p2.y - p1.y;
	z = p2.z - p1.z;

	if (shouldNormalize)
	{
		normalize();
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

void Vector3d::set(const Vector3d& v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

float Vector3d::dotProduct(const Vector3d& vector) const
{
//	nprt::math::dot_product(this.values, vector.values);
	return x*vector.x + y*vector.y + z*vector.z;
}

Vector3d Vector3d::crossProduct(const Vector3d& vector, bool normalize) const
{
	float xC = y*vector.z - z*vector.y;
	float yC = z*vector.x - x*vector.z;
	float zC = x*vector.y - y*vector.x;

	return Vector3d(xC, yC, zC, normalize);
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
	this->z =-x * sin(yaw) + z * cos(yaw);
	this->x = X;
}

void Vector3d::rotateZ(float roll)
{
	float X = x * cos(roll) - y * sin(roll);
	this->y = x * sin(roll) + y * cos(roll);
	this->x = X;
}

float Vector3d::pointLineDistance(const Vector3d& point, const Vector3d& linePoint1, const Vector3d& linePoint2)
{
	const Vector3d x1x2 = linePoint2 - linePoint1;
	return (x1x2).crossProduct(linePoint1 - point).length() / x1x2.length();
}

float Vector3d::pointLineDistance(const Vector3d& point) const
{
	// Calculating distance for a line given in points from (0,0,0) to (x,y,z).
	// The points doesn't really matter, they only must be placed on the line.

	return pointLineDistance(point, Vector3d(), Vector3d(x, y, z));
}

float Vector3d::lineLineDistance(const Vector3d& line1Point1, const Vector3d& line1Point2, const Vector3d& line2Point1, const Vector3d& line2Point2)
{
	const Vector3d a = line1Point2 - line1Point1;
	const Vector3d b = line2Point2 - line2Point1;
	const Vector3d c = line2Point1 - line1Point1;
	const Vector3d d = a.crossProduct(b);

	return fabs(c.dotProduct(d)) / d.length();
}

inline
float Vector3d::lineLineDistance(const Vector3d& otherLinePoint1, const Vector3d& otherLinePoint2) const
{
	return lineLineDistance(otherLinePoint1, otherLinePoint2, Vector3d(0, 0, 0), Vector3d(x, y, z));
}

bool Vector3d::isInAABB(const Vector3d& minDomain, const Vector3d& maxDomain, float epsilon) const
{
	return x >= minDomain.x-epsilon && x <= maxDomain.x+epsilon
		&& y >= minDomain.y-epsilon && x <= maxDomain.y+epsilon
		&& z >= minDomain.z-epsilon && x <= maxDomain.z+epsilon;
}

float Vector3d::length() const
{
	return nprt::math::fsqrt(x*x + y*y + z*z);
}

inline void Vector3d::normalize()
{	
	float l = this->length();
	this->x /= l;
	this->y /= l;
	this->z /= l;

	// TODO normalize3_accurate seems to be inaccurate... sometimes returns normal directed in opposite direction
	//nprt::math::normalize3_accurate(nprt::math::Load3Floats(this->x, this->y, this->z));
}
