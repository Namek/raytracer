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
	hasDisplacement = false;
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

	hasDisplacement = false;
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

	hasDisplacement = false;
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
	if (hasDisplacement)
		return displacedIntersection(origin, dir);


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

float Triangle::intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& origin, const Vector3d& dir)
{
	// Check ray-plane intersection
	Vector3d p1p2 = p2 - p1;
	Vector3d p1p3 = p3 - p1;
	Vector3d norm = p1p2.crossProduct(p1p3, true);
	float d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	float dist = -(origin.dotProduct(norm) + d) / (dir.dotProduct(norm));

	if(dist > Triangle::MaxDistance)
	{
		return -1;
	}
	
	// Calculate the intersection point
	Vector3d intersectionPt = origin + dir * dist;

	// Check if the point is inside the triangle (barycentric method)
	float u, v;
	getUV(p1, p2, p3, intersectionPt, u, v);

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

inline void Triangle::getUV(const Point3d& pointInTriangle, float& out_u, float& out_v) const
{
	getUV(p1, p2, p3, pointInTriangle, out_u, out_v);
}

void Triangle::getUV(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Point3d& pointInTriangle, float& out_u, float& out_v)
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

void Triangle::SetTexcoords(float u1, float v1, float u2, float v2, float u3, float v3)
{
	this->u1 = u1;
	this->v1 = v1;
	this->u2 = u2;
	this->v2 = v2;
	this->u3 = u3;
	this->v3 = v3;
}


/******************************************************************************/
/*    Ray-triangle intersection where the triangle is displacement mapped.    */
/*                                                                            */
/*    Original algorithm comes from the paper by Smits et al. titled          */
/*         "Direct Ray Tracing of Displacement Mapped Triangles"              */
/*     available here: http://www.cs.utah.edu/~shirley/papers/disp.pdf        */
/*                                                                            */
/*   Paper was partly implemented in RISE project, which can be foudn here:   */
/* http://www.koders.com/cpp/fid52A652AEE2337998D1866267C44996B287014234.aspx */
/******************************************************************************/
float Triangle::displacedIntersection(const Point3d& rayOrigin, const Vector3d& rayDirection) const
{
	float M = 0.5f;
	float m = 0.1f;

	Vector3d normals[3];
	normals[0] = normals[1] = normals[2] = norm;

	// TODO
	// Initialization phase:
	// First check to see if the ray intersects the volume of the displaced triangle
	// To do this, we need to intersect the ray by the three bilinear patches and the two
	// end-cap triangles


	Point3d top[3];			// The top end-cap
	top[0] = p1 + normals[0] * M;
	top[1] = p2 + normals[1] * M ;
	top[2] = p3 + normals[2] * M;

	Point3d bottom[3];		// The bottom end-cap
	bottom[0] = p1 + normals[0] * (-m);
	bottom[1] = p2 + normals[1] * (-m);
	bottom[2] = p3 + normals[2] * (-m);

	// Then there are the three bilinear patches
	Point3d patch0_pts[4];
	patch0_pts[0] = top[1];
	patch0_pts[1] = bottom[1];
	patch0_pts[2] = top[0];
	patch0_pts[3] = bottom[0];

	Point3d patch1_pts[4];
	patch1_pts[0] = top[2];
	patch1_pts[1] = bottom[2];
	patch1_pts[2] = top[1];
	patch1_pts[3] = bottom[1];

	Point3d patch2_pts[4];
	patch2_pts[0] = top[0];
	patch2_pts[1] = bottom[0];
	patch2_pts[2] = top[2];
	patch2_pts[3] = bottom[2];

	// Now we need to basically intersect against all of these elements to figure which 
	// side the ray enters and which side it leaves
	struct HIT
	{
		bool		bHit;
		float		dRange;
		float		dRange2;

		HIT() : bHit(false), dRange(std::numeric_limits<float>::max()), dRange2(std::numeric_limits<float>::max()) {}
	};
	struct TRIANGLE_HIT : public HIT
	{
		float		alpha;
		float		beta;

		TRIANGLE_HIT() : alpha(0), beta(0) {}
	};

	TRIANGLE_HIT topcapHit, bottomcapHit;
/*
	Triangle::intersection(
	RayTriangleIntersection( ray, topcapHit, top[0], 
		top[0] - top[1], top[0] - top[2]);

	RayTriangleIntersection( ray, bottomcapHit, bottom[0], 
		Vector3Ops::mkVector3(bottom[1],bottom[0]), Vector3Ops::mkVector3(bottom[2],bottom[0]) );

	BILINEAR_HIT bh0, bh1, bh2;

	RayBilinearPatchIntersection( ray, bh0, patch0_pts );
	RayBilinearPatchIntersection( ray, bh1, patch1_pts );
	RayBilinearPatchIntersection( ray, bh2, patch2_pts );

	// Now we have to figure out which the closest hits were and which the farthest hits were
	// Fortunately we can do this seperately

	// See if either of the end-caps are an entry point,
	char in=-1, out=-1;							// describes which side the ray comes in and which side it goes out
	Scalar inDist=INFINITY, outDist=0;			// in and out distances

	if( topcap.bHit ) {
		in = 0; 
		out = 0;
		inDist = topcap.dRange;
		outDist = topcap.dRange;
	}

	if( bottomcap.bHit ) {
		if( bottomcap.dRange < inDist ) {
			inDist = bottomcap.dRange;
			in = 1;
		}

		if( bottomcap.dRange > outDist ) {
			outDist = bottomcap.dRange;
			out = 1;
		}
	}

	// Check the three bi-linear patches
#define CHECK_BILIN_PATCHES( p, val )	\
	if( ##p.bHit ) {					\
		if( ##p.dRange < inDist ) {		\
			inDist = ##p.dRange;		\
			in = val;					\
		}								\
		if( ##p.dRange > outDist ) {	\
			outDist = ##p.dRange;		\
			out = val;					\
		}								\
	}

	CHECK_BILIN_PATCHES( bh0, 2 );
	CHECK_BILIN_PATCHES( bh1, 3 );
	CHECK_BILIN_PATCHES( bh2, 4 );

#undef CHECK_BILIN_PATCHES

	// Quick sanity check
	if( in==-1 && out !=-1 ||
		in!=-1 && out == -1 )
	{
		// Something is terribly wrong!
		_asm int 3h;
	}

	// Now we check our bailing case
	if( in==-1 && out==-1 ) {
		return;	// the volume is never intersected
	}

	// Another sanity check
	if( in==out ) {
		// You can't enter and leave the volume on the same element
		// Well actually you can, but until I add support for checking twice intersected
		// bilinear patches, lets keep this here
		_asm int 3h;
	}

	// Now we compute i,j,k which is the start co-ordinates and
	// ie,je,ke which are the end co-ordinates based on in and out
	float alpha, beta, gamma;




	typedef enum LastChange
	{
		iplus = 0,
		jminus,
		kplus,
		iminus,
		jplus,
		kminus
	};

	Vector3d a, b, c;
	Vector3d uva, uvb, uvc;
	Vector3d cNormal;
	int i, j, k, i_end, j_end, k_end;
	bool rightOfC;
	LastChange change;
	const int N = texture->GetWidth() * 3;
	const float delta = 1.0f / static_cast<float>(N);
	Vector3d intersectionNormal;

	while (true)
	{
		float testIntersection = Triangle::intersection(a, b, c, rayOrigin, rayDirection);
		if (testIntersection > 0)
		{
			intersectionNormal = (b-a).crossProduct(c-a, true);			// is it required?
			return testIntersection;
		}

		if (i == i_end && j == j_end && k == k_end)
		{
			break;
		}

		rightOfC = ((cNormal.crossProduct(rayOrigin - c, false)).dotProduct(rayDirection) > 0);

		if (rightOfC)
		{
			a = c;
			uva = uvc;
		}
		else
		{
			b = c;
			uvb = uvc;
		}

		change = static_cast<LastChange>((change + (rightOfC ? 1:5)) % 6);
		if (change == iminus)
		{
			if (--i < 0) break;
			uvc = Vector3d((j+1)*delta, (k+1)*delta, 0);
		}
		else if (change == iplus)
		{
			if (++j >= N) break;
			uvc = Vector3d(j*delta, k*delta, 0);
		}
		else if (change == jminus)
		{
			if (--j < 0) break;
			uvc = Vector3d(j*delta, (k+1)*delta, 0);
		}
		else if (change == jplus)
		{
			if (++j >= N) break;
			uvc = Vector3d((j+1)*delta, k*delta, 0);
		}
		else if (change == kminus)
		{
			if (--k < 0) break;
			uvc = Vector3d((j+1)*delta, k*delta, 0);
		}
		else if (change == kplus)
		{
			if (++k >= N) break;
			uvc = Vector3d(j*delta, (k+1)*delta, 0);
		}
		//(c,cNormal) = GetPoint(uvc)
	}*/

	return -1;
}