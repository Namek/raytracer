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
		Point3d pointOnAxisX, pointOnAxisY, pointOnAxisZ;

	public:
		Plane();
		Plane(float A, float B, float C, float D);
		Plane(const Point3d& p1, const Point3d& p2, const Point3d& p3);
		Plane(const Point3d& point, const Vector3d& normal);

		void setGeneral(float A, float B, float C, float D);
		float distance(const Point3d& point) const;
		Point3d intersectLine(const Point3d& origin, const Point3d& direction) const;
	};
}