#include "Triangle.h"

#include <algorithm>

using namespace nprt;


float Triangle::MaxDistance = 100000.0f;


Triangle::Triangle() {
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

Triangle::Triangle(Point3d p1, Point3d p2, Point3d p3, int ind) {
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->ind = ind;
	materialIndex = -1;

	Vector3d p1p2(p1, p2, false);
	Vector3d p1p3(p1, p3, false);
	norm = p1p2.crossProduct(p1p3, true);
	d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	preprocess();
}

Triangle::Triangle(Point3d p1, Point3d p2, Point3d p3, int index, int ind) {
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	this->ind = ind;
	materialIndex = index;

	Vector3d p1p2(p1, p2, false);
	Vector3d p1p3(p1, p3, false);
	norm = p1p2.crossProduct(p1p3, true);
	d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	preprocess();
}

void Triangle::setMaterialIndex(int index) 
{
	materialIndex = index;
}

//Wyznaczam warto�� a1, b1, d1, a2, b2, d2, a3, b3, d3, max1, max2, min1, min2
void Triangle::preprocess() 
{
	if ((abs(norm.x)>abs(norm.y)) && (abs(norm.x)>abs(norm.z)))
	{
		//Rzutowanie na p�aszczyzn� YZ
		plane = 0;

		max1 = (p1.y>p2.y && p1.y>p3.y)?p1.y:(p2.y>p3.y?p2.y:p3.y);
		max2 = (p1.z>p2.z && p1.z>p3.z)?p1.z:(p2.z>p3.z?p2.z:p3.z);
		min1 = (p1.y<p2.y && p1.y<p3.y)?p1.y:(p2.y<p3.y?p2.y:p3.y);
		min2 = (p1.z<p2.z && p1.z<p3.z)?p1.z:(p2.z<p3.z?p2.z:p3.z);

		//Prosta zawieraj�ca punkty P1 P2

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

		//Prosta zawieraj�ca punkty P1 P3
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

		//Prosta zawieraj�ca punkty P2 P3
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
		//Rzutujemy na p�aszczyzn� XZ
		plane = 1;

		max1 = (p1.x > p2.x && p1.x > p3.x) ? p1.x : (p2.x > p3.x ? p2.x : p3.x);
		max2 = (p1.z > p2.z && p1.z > p3.z) ? p1.z : (p2.z > p3.z ? p2.z : p3.z);
		min1 = (p1.x < p2.x && p1.x < p3.x) ? p1.x : (p2.x < p3.x ? p2.x : p3.x);
		min2 = (p1.z < p2.z && p1.z < p3.z) ? p1.z : (p2.z < p3.z ? p2.z : p3.z);

		//Prosta zawieraj�ca punkty P1 P2
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

		//Prosta zawieraj�ca punkty P1 P3
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

		//Prosta zawieraj�ca punkty P2 P3
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
		//Rzutujemy na p�aszczyzn� XY
		plane = 2;

		max1 = (p1.x>p2.x && p1.x>p3.x)?p1.x:(p2.x>p3.x?p2.x:p3.x);
		max2 = (p1.y>p2.y && p1.y>p3.y)?p1.y:(p2.y>p3.y?p2.y:p3.y);
		min1 = (p1.x<p2.x && p1.x<p3.x)?p1.x:(p2.x<p3.x?p2.x:p3.x);
		min2 = (p1.y<p2.y && p1.y<p3.y)?p1.y:(p2.y<p3.y?p2.y:p3.y);

		//Prosta zawieraj�ca punkty P1 P2

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

		//Prosta zawieraj�ca punkty P1 P3
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

		//Prosta zawieraj�ca punkty P2 P3
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
	// Compute vectors        
	Vector3d v0 = p3 - p1;
	Vector3d v1 = p2 - p1;
	Vector3d v2 = intersectionPt - p1;

	// Compute dot products
	float dot00 = v0.dotProduct(v0);
	float dot01 = v0.dotProduct(v1);
	float dot02 = v0.dotProduct(v2);
	float dot11 = v1.dotProduct(v1);
	float dot12 = v1.dotProduct(v2);

	// Compute barycentric coordinates
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	if((u >= 0) && (v >= 0) && (u + v < 1))
	{
		return dist;
	}
	else
	{
		return -1;
	}
}

