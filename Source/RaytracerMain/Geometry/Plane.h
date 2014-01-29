#pragma once

#include "Vector3d.h"

namespace nprt
{
	class Plane
	{
	public:
		float A, B, C, D;					//równanie ogólne
		float alfa, beta, gamma, delta, N;	//równanie normalne
		float a, b, c;						//równanie odcinkowe
		Vector3d normal;
		Vector3d pointOnAxisX, pointOnAxisY, pointOnAxisZ;

	public:
		Plane();
		Plane(float A, float B, float C, float D);
		Plane(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3);
		Plane(const Vector3d& point, const Vector3d& normal);

		void setGeneral(float A, float B, float C, float D);
		float distance(const Vector3d& point) const;
		float intersectLine(const Vector3d& origin, const Vector3d& direction, Vector3d& out_intersectionPoint) const;
		float intersectLineInSegment(const Vector3d& lineOrigin, const Vector3d& lineDirection,
			const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& p4, Vector3d& out_intersectionPoint) const;
	};
}