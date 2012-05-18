#include "Triangle.h"

#include <algorithm>
#include <limits>

using namespace nprt;


float Triangle::MaxDistance = 100000.0f;


Triangle::Triangle()
{
	p1 = Point3d();
	p2 = Point3d();
	p3 = Point3d();	
	materialIndex = -1;
	d = -1;
	norm = Vector3d();
	a1 = a2 = a3 = b1 = b2 = b3 = 0.f;
	max1 = max2 = min1 = min2 = -666.f;
	d1 = d2 = d3 = 0.f;
	plane = -1;
}

Triangle::Triangle(Point3d p1, Point3d p2, Point3d p3, int ind)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->ind = ind;
	materialIndex = -1;

	Vector3d p1p2(p1, p2, false);
	Vector3d p1p3(p1, p3, false);
	norm = p1p2.crossProduct(p1p3, true);
	norm.normalize();
	d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	preprocess();
}

Triangle::Triangle(Point3d p1, Point3d p2, Point3d p3, int index, int ind)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->ind = ind;
	materialIndex = index;

	Vector3d p1p2(p1, p2, false);
	Vector3d p1p3(p1, p3, false);
	norm = p1p2.crossProduct(p1p3, true);
	norm.normalize();
	d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	preprocess();
}

void Triangle::setMaterialIndex(int index) 
{
	materialIndex = index;
}

//Wyznaczam wartoœæ a1, b1, d1, a2, b2, d2, a3, b3, d3, max1, max2, min1, min2
void Triangle::preprocess() 
{
	if ((abs(norm.x)>abs(norm.y)) && (abs(norm.x)>abs(norm.z)))
	{
		//Rzutowanie na p³aszczyznê YZ
		plane = 0;

		max1 = (p1.y>p2.y && p1.y>p3.y)?p1.y:(p2.y>p3.y?p2.y:p3.y);
		max2 = (p1.z>p2.z && p1.z>p3.z)?p1.z:(p2.z>p3.z?p2.z:p3.z);
		min1 = (p1.y<p2.y && p1.y<p3.y)?p1.y:(p2.y<p3.y?p2.y:p3.y);
		min2 = (p1.z<p2.z && p1.z<p3.z)?p1.z:(p2.z<p3.z?p2.z:p3.z);

		//Prosta zawieraj¹ca punkty P1 P2

		if (p1.y == p2.y)
		{
			a1 = 1;
			b1 = 0;
			d1 = -p1.y;
		}
		else if (p1.z == p2.z) 
		{
			a1 = 0;
			b1 = 1;
			d1 = -p1.z;
		}
		else 
		{
			a1 = (p2.z - p1.z)/(p2.y - p1.y);
			b1 = 1;
			d1 = p1.z - a1*p1.y;
			a1 *= -1;
			d1 *= -1;
		}

		if (a1*p3.y + b1*p3.z + d1 < 0) 
		{
			a1 *= -1;
			b1 *= -1;
			d1 *= -1;
		}

		//Prosta zawieraj¹ca punkty P1 P3
		if (p1.y == p3.y)
		{
			a2 = 1;
			b2 = 0;
			d2 = -p1.y;
		} 
		else if (p1.z == p3.z) 
		{
			a2 = 0;
			b2 = 1;
			d2 = -p1.z;
		} 
		else 
		{
			a2 = (p3.z - p1.z)/(p3.y - p1.y);
			b2 = 1;
			d2 = p1.z - a2*p1.y;
			a2 *= -1;
			d2 *= -1;
		}

		if (a2*p2.y + b2*p2.z + d2 < 0) 
		{
			a2 *= -1;
			b2 *= -1;
			d2 *= -1;
		}

		//Prosta zawieraj¹ca punkty P2 P3
		if (p2.y == p3.y)
		{
			a3 = 1;
			b3 = 0;
			d3 = -p2.y;
		} 
		else if (p2.z == p3.z) 
		{
			a3 = 0;
			b3 = 1;
			d3 = -p2.z;
		} 
		else 
		{
			a3 = (p3.z - p2.z)/(p3.y - p2.y);
			b3 = 1;
			d3 = p2.z - a3*p2.y;
			a3 *= -1;
			d3 *= -1;
		}

		if (a3*p1.y + b3*p1.z + d3 < 0) 
		{
			a3 *= -1;
			b3 *= -1;
			d3 *= -1;
		}
	}
	else if (abs(norm.y)>abs(norm.z))
	{
		//Rzutujemy na p³aszczyznê XZ
		plane = 1;

		max1 = (p1.x > p2.x && p1.x > p3.x) ? p1.x : (p2.x > p3.x ? p2.x : p3.x);
		max2 = (p1.z > p2.z && p1.z > p3.z) ? p1.z : (p2.z > p3.z ? p2.z : p3.z);
		min1 = (p1.x < p2.x && p1.x < p3.x) ? p1.x : (p2.x < p3.x ? p2.x : p3.x);
		min2 = (p1.z < p2.z && p1.z < p3.z) ? p1.z : (p2.z < p3.z ? p2.z : p3.z);

		//Prosta zawieraj¹ca punkty P1 P2
		if (p1.x == p2.x)
		{
			a1 = 1;
			b1 = 0;
			d1 = -p1.x;
		} 
		else if (p1.z == p2.z) 
		{
			a1 = 0;
			b1 = 1;
			d1 = -p1.z;
		} 
		else 
		{
			a1 = (p2.z - p1.z)/(p2.x - p1.x);
			b1 = 1;
			d1 = p1.z - a1*p1.x;
			a1 *= -1;
			d1 *= -1;
		}

		if (a1*p3.x + b1*p3.z + d1 < 0) 
		{
			a1 *= -1;
			b1 *= -1;
			d1 *= -1;
		}

		//Prosta zawieraj¹ca punkty P1 P3
		if (p1.x == p3.x)
		{
			a2 = 1;
			b2 = 0;
			d2 = -p1.x;
		} 
		else if (p1.z == p3.z) 
		{
			a2 = 0;
			b2 = 1;
			d2 = -p1.z;
		} 
		else 
		{
			a2 = (p3.z - p1.z)/(p3.x - p1.x);
			b2 = 1;
			d2 = p1.z - a2*p1.x;
			a2 *= -1;
			d2 *= -1;
		}

		if (a2*p2.x + b2*p2.z + d2 < 0) 
		{
			a2 *= -1;
			b2 *= -1;
			d2 *= -1;
		}

		//Prosta zawieraj¹ca punkty P2 P3
		if (p2.x == p3.x)
		{
			a3 = 1;
			b3 = 0;
			d3 = -p2.x;
		}
		else if (p2.z == p3.z) 
		{
			a3 = 0;
			b3 = 1;
			d3 = -p2.z;
		}
		else 
		{
			a3 = (p3.z - p2.z)/(p3.x - p2.x);
			b3 = 1;
			d3 = p2.z - a3*p2.x;
			a3 *= -1;
			d3 *= -1;
		}

		if (a3*p1.x + b3*p1.z + d3 < 0) 
		{
			a3 *= -1;
			b3 *= -1;
			d3 *= -1;
		}
	}
	else
	{
		//Rzutujemy na p³aszczyznê XY
		plane = 2;

		max1 = (p1.x>p2.x && p1.x>p3.x)?p1.x:(p2.x>p3.x?p2.x:p3.x);
		max2 = (p1.y>p2.y && p1.y>p3.y)?p1.y:(p2.y>p3.y?p2.y:p3.y);
		min1 = (p1.x<p2.x && p1.x<p3.x)?p1.x:(p2.x<p3.x?p2.x:p3.x);
		min2 = (p1.y<p2.y && p1.y<p3.y)?p1.y:(p2.y<p3.y?p2.y:p3.y);

		//Prosta zawieraj¹ca punkty P1 P2

		if (p1.x == p2.x)
		{
			a1 = 1;
			b1 = 0;
			d1 = -p1.x;
		}
		else if (p1.y == p2.y) 
		{
			a1 = 0;
			b1 = 1;
			d1 = -p1.y;
		}
		else 
		{
			a1 = (p2.y - p1.y)/(p2.x - p1.x);
			b1 = 1;
			d1 = p1.y - a1*p1.x;
			a1 *= -1;
			d1 *= -1;
		}

		if (a1*p3.x + b1*p3.y + d1 < 0) 
		{
			a1 *= -1;
			b1 *= -1;
			d1 *= -1;
		}

		//Prosta zawieraj¹ca punkty P1 P3
		if (p1.x == p3.x)
		{
			a2 = 1;
			b2 = 0;
			d2 = -p1.x;
		}
		else if (p1.y == p3.y) 
		{
			a2 = 0;
			b2 = 1;
			d2 = -p1.y;
		}
		else
		{
			a2 = (p3.y - p1.y)/(p3.x - p1.x);
			b2 = 1;
			d2 = p1.y - a2*p1.x;
			a2 *= -1;
			d2 *= -1;
		}

		if (a2*p2.x + b2*p2.y + d2 < 0)
		{
			a2 *= -1;
			b2 *= -1;
			d2 *= -1;
		}

		//Prosta zawieraj¹ca punkty P2 P3
		if (p2.x == p3.x)
		{
			a3 = 1;
			b3 = 0;
			d3 = -p2.x;
		} 
		else if (p2.y == p3.y)
		{
			a3 = 0;
			b3 = 1;
			d3 = -p2.y;
		}
		else
		{
			a3 = (p3.y - p2.y)/(p3.x - p2.x);
			b3 = 1;
			d3 = p2.y - a3*p2.x;
			a3 *= -1;
			d3 *= -1;
		}

		if (a3*p1.x + b3*p1.y + d3 < 0)
		{
			a3 *= -1;
			b3 *= -1;
			d3 *= -1;
		}
	}
}


float Triangle::intersection(const Vector3d& origin, const Vector3d& dir) const
{
	// Check ray-plane intersection
	float dist = -(origin.dotProduct(norm) + d) / (dir.dotProduct(norm));

	if(dist > Triangle::MaxDistance)
	{
		return -1;
	}
	
	// Calculate the intersection point
	Vector3d intersectionPt = origin + dir * dist;

	// Check if the point is inside the triangle (barycentric method)
	float u, v;
	getUV(intersectionPt, u, v);

	// Check if point is in triangle
	if((u >= std::numeric_limits<float>::epsilon()) && (v >= std::numeric_limits<float>::epsilon()) && (u + v < 1))
	{
		return dist;
	}
	else
	{
		return -1;
	}
}

void Triangle::getUV(const Point3d& pointInTriangle, float& out_u, float& out_v) const
{
	// Compute vectors        
	Vector3d v0 = p3 - p1;
	Vector3d v1 = p2 - p1;
	Vector3d v2 = pointInTriangle - p1;

	// Compute dot products
	float dot00 = v0.dotProduct(v0);
	float dot01 = v0.dotProduct(v1);
	float dot02 = v0.dotProduct(v2);
	float dot11 = v1.dotProduct(v1);
	float dot12 = v1.dotProduct(v2);

	// Compute barycentric coordinates
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	out_u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	out_v = (dot00 * dot12 - dot01 * dot02) * invDenom;
}

/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/********************************************************/

bool Triangle::overlapsWithAABB(const Point3d& minDomain, const Point3d& maxDomain) const
{
	Point3d boxHalfSize((maxDomain - minDomain)*0.5f);
	return overlapsWithAABB_byDomainCenter(minDomain + boxHalfSize, boxHalfSize);
}

#define FINDMINMAX(x0,x1,x2,min,max)					\
  min = max = x0;										\
  if(x1<min) min=x1;									\
  if(x1>max) max=x1;									\
  if(x2<min) min=x2;									\
  if(x2>max) max=x2;

#define AXISTEST_X01(a, b, fa, fb)						\
	p0 = a*v0.y - b*v0.z;								\
	p2 = a*v2.y - b*v2.z;								\
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}	\
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return false;

#define AXISTEST_X2(a, b, fa, fb)						\
	p0 = a*v0.y - b*v0.z;								\
	p1 = a*v1.y - b*v1.z;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return false;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)						\
	p0 = -a*v0.x + b*v0.z;								\
	p2 = -a*v2.x + b*v2.z;								\
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return false;

#define AXISTEST_Y1(a, b, fa, fb)						\
	p0 = -a*v0.x + b*v0.z;								\
	p1 = -a*v1.x + b*v1.z;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return false;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)						\
	p1 = a*v1.x - b*v1.y;								\
	p2 = a*v2.x - b*v2.y;								\
	if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;		\
	if(min>rad || max<-rad) return false;

#define AXISTEST_Z0(a, b, fa, fb)						\
	p0 = a*v0.x - b*v0.y;								\
	p1 = a*v1.x - b*v1.y;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;		\
	if(min>rad || max<-rad) return false;

bool planeBoxOverlap(const Vector3d& normal, const Vector3d& vert, const Vector3d& maxBox)
{
	Vector3d vMin, vMax;

	if (normal.x > 0)
	{
		vMin.x =-maxBox.x - vert.x;
		vMax.x = maxBox.x - vert.x;
	}
	else
	{
		vMin.x = maxBox.x - vert.x;
		vMax.x =-maxBox.x - vert.x;
	}

	if (normal.y > 0)
	{
		vMin.y =-maxBox.y - vert.y;
		vMax.y = maxBox.y - vert.y;
	}
	else
	{
		vMin.y = maxBox.y - vert.y;
		vMax.y =-maxBox.y - vert.y;
	}

	if (normal.z > 0)
	{
		vMin.z =-maxBox.z - vert.z;
		vMax.z = maxBox.z - vert.z;
	}
	else
	{
		vMin.z = maxBox.z - vert.z;
		vMax.z =-maxBox.z - vert.z;
	}
	
	if (normal.dotProduct(vMin) > 0)
		return false;

	if (normal.dotProduct(vMax) > 0)
		return true;
	
	return false;
}

bool Triangle::overlapsWithAABB_byDomainCenter(const Point3d& boxCenter, const Point3d& boxHalfSize) const
{
	// Move everything so that the boxCenter is in (0,0,0)
	Vector3d v0(this->p1 - boxCenter);
	Vector3d v1(this->p2 - boxCenter);
	Vector3d v2(this->p3 - boxCenter);

	// Compute triangle edges
	Vector3d e0(v1 - v0);
	Vector3d e1(v2 - v1);
	Vector3d e2(v0 - v2);

	// Test the 9 tests firsts
	float fex = fabsf(e0.x);
	float fey = fabsf(e0.y);
	float fez = fabsf(e0.z);

	float p0, p1, p2;
	float min, max, rad;

	AXISTEST_X01(e0.z, e0.y, fez, fey);
	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	AXISTEST_Z12(e0.y, e0.x, fey, fex);

	
	fex = fabsf(e1.x);
	fey = fabsf(e1.y);
	fez = fabsf(e1.z);

	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0(e1.y, e1.x, fey, fex);

	fex = fabsf(e2.x);
	fey = fabsf(e2.y);
	fez = fabsf(e2.z);

	AXISTEST_X2(e2.z, e2.y, fez, fey);
	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);

	// Test overlap in the {x,y,z}-directions.
	// Find min, max of the triangle each direction,
	// and test for overlap in that direction.
	// (This is equivalent to testing a minimal AABB
	//   around the triangle aganinst the AABB)
	
	/* test in X-direction */
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min > boxHalfSize.x || max < -boxHalfSize.x)
		return false;
	
	/* test in Y-direction */
	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min > boxHalfSize.y || max < -boxHalfSize.y)
		return false;
	
	/* test in Z-direction */
	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min > boxHalfSize.z || max < -boxHalfSize.z)
		return false;

	// Test if the box intersects the plane of the triangle.
	// Compute plane equation of triangle: normal*x+d=0
	Vector3d normal = e0.crossProduct(e1, false);

	if (!planeBoxOverlap(normal, v0, boxHalfSize))
		return false;

	return true;
}
/************************* CUT **************************/
/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/********************************************************/