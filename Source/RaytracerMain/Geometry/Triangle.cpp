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
	max1 = max2 = min1 = min2 = -666.f;
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

	hasDisplacement = false;
}

void Triangle::setMaterialIndex(int index) 
{
	materialIndex = index;
}

float Triangle::intersection(const Vector3d& origin, const Vector3d& dir) const
{
	if (hasDisplacement && texture->currentX == 313 && ind == 3270)
	{
		//return displacedIntersection(origin, dir);
	}


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

float Triangle::intersection(const Vector3d& p1, const Vector3d& p2, const Vector3d& p3, const Vector3d& rayOrigin, const Vector3d& rayDirection, float& out_u, float& out_v)
{
	// Check ray-plane intersection
	Vector3d p1p2 = p2 - p1;
	Vector3d p1p3 = p3 - p1;
	Vector3d norm = p1p2.crossProduct(p1p3, true);
	float d = -(norm.x*p1.x + norm.y*p1.y + norm.z*p1.z);
	float dist = -(rayOrigin.dotProduct(norm) + d) / (rayDirection.dotProduct(norm));

	if(dist > Triangle::MaxDistance)
	{
		return -1;
	}
	
	// Calculate the intersection point
	Vector3d intersectionPt = rayOrigin + rayDirection * dist;

	// Check if the point is inside the triangle (barycentric method)
	float u, v;
	getUV(p1, p2, p3, intersectionPt, u, v);
	out_u = u;
	out_v = v;

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
#define NEARZERO float(1e-12)
#define INFINITY float(1.7976931348623158e+308)

struct HIT
{
	bool		bHit;
	float		dRange;
	float		dRange2;

	HIT() : bHit(false), dRange(INFINITY), dRange2(INFINITY) {}
};

struct TRIANGLE_HIT : public HIT
{
	float		alpha;
	float		beta;

	TRIANGLE_HIT() : alpha(0), beta(0) {}
};

struct BILINEAR_HIT : public HIT
{
	float		u;
	float		v;

	BILINEAR_HIT( ) : u( 0 ), v( 0 ) {}
};

struct BilinearPatch
{
	Point3d pts[4];
};

#define FIND_BARY( x )												\
	switch( ##x )													\
	{																\
	case 0:															\
		/* top cap	*/												\
		alpha = topcapHit.alpha;										\
		beta = topcapHit.beta;											\
		gamma = 1.0f - (topcapHit.alpha + topcapHit.beta);					\
		/* we set one to zero so that we go right to the end */		\
		if( alpha < beta && alpha < gamma ) alpha = 0;				\
		else if( beta < gamma ) beta = 0;							\
		else gamma = 0;												\
		break;														\
	case 1:															\
		/* bottom cap	*/											\
		alpha = bottomcapHit.alpha;									\
		beta = bottomcapHit.beta;										\
		gamma = 1.0f - (bottomcapHit.alpha + bottomcapHit.beta);				\
		/* we set one to zero so that we go right to the end */		\
		if( alpha < beta && alpha < gamma ) alpha = 0;				\
		else if( beta < gamma ) beta = 0;							\
		else gamma = 0;												\
		break;														\
	case 2:															\
		/* bilinear 0 - gamma is 0	*/								\
		gamma = 0;													\
		alpha = bh0.u;												\
		beta = 1.0f-bh0.u;											\
		break;														\
	case 3:															\
		/* bilinear 1 - alpha is 0	*/								\
		alpha = 0;													\
		beta = bh1.u;												\
		gamma = 1.0f-bh0.u;											\
		break;														\
	case 4:															\
		/* bilinear 2 - beta is 0	*/								\
		beta = 0;													\
		gamma = bh2.u;												\
		alpha = 1.0f-bh2.u;											\
		break;														\
	};


// Choose between the best denominator to avoid singularities
// and to get the most accurate root possible
inline float getu( 
	const float v,
	const float M1,
	const float M2, 
	const float J1,
	const float J2,
	const float K1, 
	const float K2,
	const float R1, 
	const float R2
	)
{
	const float denom = (v*(M1-M2)+J1-J2);
	const float d2 = (v*M1+J1);
	if(fabs(denom) > fabs(d2)) { // which denominator is bigger
		return (v*(K2-K1)+R2-R1)/denom;
	}

	return -(v*K1+R1)/d2;
}

// Compute t with the best accuracy by using the component
// of the direction that is largest
float computet(
	const Vector3d& rayOrigin,
	const Vector3d& rayDirection,
	const Point3d& srfpos
	)
{
	if(fabs(rayDirection.x) >= fabs(rayDirection.y) && fabs(rayDirection.x) >= fabs(rayDirection.z)) {
		return (srfpos.x - rayOrigin.x) / rayDirection.x;
	} else if(fabs(rayDirection.y) >= fabs(rayDirection.z)) {
		return (srfpos.y - rayOrigin.y) / rayDirection.y;
	} else {
		return (srfpos.z - rayOrigin.z) / rayDirection.z;
	}
}

// Solves a quadratic function within the given range
// Returns number of solutions
int SolveQuadricWithinRange( 
	const float (&coeff)[3],				///< [in] Coefficients
	float (&sol)[2],						///< [out] Solutions
	const float min,						///< [in] Minimum value
	const float max						///< [in] Maximum value
	)
{
	const float& a = coeff[0];
	const float& b = coeff[1];
	const float& c = coeff[2];

	sol[0] = sol[1] = min-min;
	if( a == 0.0 ) {
		if(b != 0.0) {
			sol[0] = -c/b;
			if( sol[0] > min && sol[0] < max ) {
				return 1;
			} else {
				return 0;
			}
		} else {
			return 0;
		}
	}

	const float d = b*b - 4*a*c; //discriminant

	if(d <= 0.0) {
		if(d == 0.0) {
			sol[0] = -b/a;
			if(sol[0] > min && sol[0] < max) {
				return 1;
			} else {
			return 0;
			}
		} else {
			return 0;
		}
	}

#ifdef _WIN32
	// Bloody MSVC crt
	const float q = -0.5  * (b + _copysign(sqrt(d),b));
#else
	const float q = -0.5  * (b + copysign(sqrt(d),b));
#endif

	sol[0] = c/q;
	sol[1] = q/a;

	if(
		(sol[0] > min && sol[0] < max ) &&
		(sol[1] > min && sol[1] < max)) {
		return 2;
	} else if(sol[0] > min && sol[0] < max) {
		return 1;
	} else if(sol[1] > min && sol[1] < max) {
		// Swap them around
		const float temp = sol[0];
		sol[0] = sol[1];
		sol[1] = temp;
		return 1;
	}

	return 0;
}

Point3d EvaluateBilinearPatchAt( 
	const BilinearPatch& patch,
	const float u, 
	const float v 
	)
{
	Point3d ret;
	ret.x = ( ( (1.0 - u) * (1.0 - v) * patch.pts[0].x +
			(1.0 - u) *        v  * patch.pts[1].x + 
			u  * (1.0 - v) * patch.pts[2].x +
			u  *        v  * patch.pts[3].x));
	ret.y = (  ( (1.0 - u) * (1.0 - v) * patch.pts[0].y +
			(1.0 - u) *        v  * patch.pts[1].y + 
			u  * (1.0 - v) * patch.pts[2].y +
			u  *        v  * patch.pts[3].y));
	ret.z = (  ( (1.0 - u) * (1.0 - v) * patch.pts[0].z +
			(1.0 - u) *        v  * patch.pts[1].z + 
			u  * (1.0 - v) * patch.pts[2].z +
			u  *        v  * patch.pts[3].z));

	return ret;
}

void RayBilinearPatchIntersection( 
		const Vector3d& rayOrigin,
		const Vector3d& rayDirection,
		BILINEAR_HIT& hit,
		const BilinearPatch& patch
		)
{
	hit.bHit = false;
	hit.dRange = INFINITY;
	hit.dRange2 = INFINITY;

	//
	// Equation of the patch
	// 
	// P(u,v) = (1-u)(1-v)*patch.pts[0] + (1-u)v*patch.pts[1] + u(1-v)*patch.pts[2] + uv*patch.pts[3]
	//

	// Variables for substitution
	// a = pts[3] - pts[2] - pts[1] + pts[0]
	// b = pts[2] - pts[0]
	// c = pts[1] - pts[0]
	// d = pts[0]

	// Find a w.r.t. x, y, z
	const float ax = patch.pts[3].x - patch.pts[2].x - patch.pts[1].x + patch.pts[0].x;
	const float ay = patch.pts[3].y - patch.pts[2].y - patch.pts[1].y + patch.pts[0].y;
	const float az = patch.pts[3].z - patch.pts[2].z - patch.pts[1].z + patch.pts[0].z;


	// Find b w.r.t. x, y, z
	const float bx = patch.pts[2].x - patch.pts[0].x;
	const float by = patch.pts[2].y - patch.pts[0].y;
	const float bz = patch.pts[2].z - patch.pts[0].z;

	// Find c w.r.t. x, y, z
	const float cx = patch.pts[1].x - patch.pts[0].x;
	const float cy = patch.pts[1].y - patch.pts[0].y;
	const float cz = patch.pts[1].z - patch.pts[0].z;


	const float rx = rayOrigin.x;
	const float ry = rayOrigin.y;
	const float rz = rayOrigin.z;

	// Retrieve the xyz of the q part of ray
	const float qx = rayDirection.x;
	const float qy = rayDirection.y;
	const float qz = rayDirection.z;

	// Find d w.r.t. x, y, z - subtracting r just after  
	const float dx = patch.pts[0].x - rx;
	const float dy = patch.pts[0].y - ry;
	const float dz = patch.pts[0].z - rz;

	// Find A1 and A2
	const float A1 = ax*qz - az*qx;
	const float A2 = ay*qz - az*qy;

	// Find B1 and B2
	const float B1 = bx*qz - bz*qx;
	const float B2 = by*qz - bz*qy;

	// Find C1 and C2
	const float C1 = cx*qz - cz*qx;
	const float C2 = cy*qz - cz*qy;

	// Find D1 and D2
	const float D1 = dx*qz - dz*qx;
	const float D2 = dy*qz - dz*qy;

	float coeff[3] = {0};
	coeff[0] = A2*C1 - A1*C2;
	coeff[1] = A2*D1 - A1*D2 + B2*C1 -B1*C2;
	coeff[2] = B2*D1 - B1*D2;

	hit.u = hit.v = hit.dRange = -2;
		
	float sol[2] = {0};
	const int numSol = SolveQuadricWithinRange( coeff, sol, -NEARZERO, 1.0+NEARZERO ); 

	switch( numSol )
	{
	case 0:
		break;			 // no solutions found
	case 1:
		{
			hit.u = getu(sol[0],A2,A1,B2,B1,C2,C1,D2,D1);
			hit.v = sol[0];
				
			const Point3d pos1 = EvaluateBilinearPatchAt( patch, hit.u, hit.v );
			hit.dRange = computet(rayOrigin, rayDirection, pos1);

			if( hit.u < 1+NEARZERO && hit.u > -NEARZERO && hit.dRange > 0 ) {
				hit.bHit = true;
			}
		}
		break;
	case 2: // two solutions found
		{
			hit.v = sol[0];
			hit.u = getu(sol[0],A2,A1,B2,B1,C2,C1,D2,D1);
				
			const Point3d pos1 = EvaluateBilinearPatchAt( patch, hit.u, hit.v );
			hit.dRange = computet(rayOrigin, rayDirection, pos1); 

			if( hit.u < 1+NEARZERO && hit.u > -NEARZERO && hit.dRange > 0 ) {
				hit.bHit = true;

				const float u = getu(sol[1],A2,A1,B2,B1,C2,C1,D2,D1);
				if( u < 1+NEARZERO && u > NEARZERO ) {
					const Point3d pos2 = EvaluateBilinearPatchAt( patch, u, sol[1] );
					const float t2 = computet(rayOrigin, rayDirection, pos2);
					if(t2 < 0 || hit.dRange < t2) { // t2 is bad or t1 is better
						return;
					}
					// other wise both t2 > 0 and t2 < t1
					hit.v = sol[1];
					hit.u = u;
					hit.dRange = t2;					
				}
			}
			else // doesn't fit in the root - try other one
			{
				hit.u = getu(sol[1],A2,A1,B2,B1,C2,C1,D2,D1);
				hit.v = sol[1];
				const Point3d pos1 = EvaluateBilinearPatchAt( patch, hit.u, hit.v );
				hit.dRange = computet(rayOrigin, rayDirection, pos1);

				if( hit.u < 1+NEARZERO && hit.u > -NEARZERO && hit.dRange > 0 ) {
					hit.bHit = true;
				}
			}
		}
		break;
	};
}

#define MATRIX_DETERMINANT(M)										\
	  (M ## _00 * (M ## _11 * M ## _22 - M ## _12 * M ## _21))	\
	- (M ## _10 * (M ## _01 * M ## _22 - M ## _02 * M ## _21))	\
	+ (M ## _20 * (M ## _01 * M ## _12 - M ## _02 * M ## _11))

// m1 = m2
#define COPY_MATRIX(M1, M2)				\
	M1 ## _00 = M2 ## _00;			\
	M1 ## _01 = M2 ## _01;			\
	M1 ## _02 = M2 ## _02;			\
	M1 ## _10 = M2 ## _10;			\
	M1 ## _11 = M2 ## _11;			\
	M1 ## _12 = M2 ## _12;			\
	M1 ## _20 = M2 ## _20;			\
	M1 ## _21 = M2 ## _21;			\
	M1 ## _22 = M2 ## _22;
	


void RayTriangleIntersection( 
	const Vector3d& rayOrigin,
	const Vector3d& rayDirection,
	TRIANGLE_HIT& hit,
	const Point3d& vPt1,
	const Vector3d& vEdgeA,
	const Vector3d& vEdgeB
	)
{
	hit.bHit = false;
	hit.dRange = INFINITY;

	// Here is my intersection test.  
	// You can find this in Real-time Rendering 10.5.2
		
	// Equation of the ray
	// X = raybegin + nRange * raydir
		
	// Equation of the triangle
	// X = vPoint1 + X * vPoint2 + Y * vPoint3;
		
	float m_00 = rayDirection.x;
	float m_01 = rayDirection.y;
	float m_02 = rayDirection.z;

	float m_10 = vEdgeA.x;
	float m_11 = vEdgeA.y;
	float m_12 = vEdgeA.z;

	float m_20 = vEdgeB.x;
	float m_21 = vEdgeB.y;
	float m_22 = vEdgeB.z;

	const float determinant = MATRIX_DETERMINANT(m);

	if( determinant > -NEARZERO && determinant < NEARZERO )
	{
		return;
	}

	float oodet = 1.0f / determinant;
		
	float m2_00;
	float m2_01;
	float m2_02;
			  
	float m2_10;
	float m2_11;
	float m2_12;
			  
	float m2_20;
	float m2_21;
	float m2_22;

	// The vector v gives the vector from the ray's begining to the first point on the triangle
	const Vector3d v = vPt1 - rayOrigin;

	COPY_MATRIX(m2, m);
	m2_10 = v.x;
	m2_11 = v.y;
	m2_12 = v.z;

	const float a = -MATRIX_DETERMINANT(m2) * oodet;

	if( (a < 0.0-NEARZERO) || (a > 1.0+NEARZERO) )
	{
		return;
	}

	COPY_MATRIX(m2, m);
	m2_20 = v.x;
	m2_21 = v.y;
	m2_22 = v.z;
	const float b = -MATRIX_DETERMINANT(m2) * oodet;

	if( (b < 0.0-NEARZERO) || ((a+b) > 1.0+NEARZERO) )
	{
		return;
	}

	COPY_MATRIX(m2, m);
	m2_00 = v.x;
	m2_01 = v.y;
	m2_02 = v.z;
	hit.dRange = MATRIX_DETERMINANT(m2) * oodet;

	if( hit.dRange >= NEARZERO )
	{
		hit.dRange2 = hit.dRange;
		hit.bHit = true;
		hit.alpha = a;
		hit.beta = b;
	}
}

float Triangle::displacedIntersection(const Point3d& rayOrigin, const Vector3d& rayDirection) const
{
	float M = 0.5f;
	float m = 0.01f;

	Vector3d p1 = this->p1;
	Vector3d p2 = this->p2;
	Vector3d p3 = this->p3;
	
	Vector3d normals[3];
	normals[0] = normals[1] = normals[2] = (p2 - p1).crossProduct(p3 - p1, false);
	normals[0].normalize();
	normals[1].normalize();
	normals[2].normalize();


	// Initialization phase:
	// First check to see if the ray intersects the volume of the displaced triangle
	// To do this, we need to intersect the ray by the three bilinear patches and the two
	// end-cap triangles


	Point3d top[3];			// The top end-cap
	top[0] = p1 + normals[0] * M;
	top[1] = p2 + normals[1] * M;
	top[2] = p3 + normals[2] * M;

	Point3d bottom[3];		// The bottom end-cap
	bottom[0] = p1 + normals[0] * (-m);
	bottom[1] = p2 + normals[1] * (-m);
	bottom[2] = p3 + normals[2] * (-m);

	// Then there are the three bilinear patches
	BilinearPatch patch0;
	patch0.pts[0] = top[1];
	patch0.pts[1] = bottom[1];
	patch0.pts[2] = top[0];
	patch0.pts[3] = bottom[0];

	BilinearPatch patch1;
	patch1.pts[0] = top[2];
	patch1.pts[1] = bottom[2];
	patch1.pts[2] = top[1];
	patch1.pts[3] = bottom[1];

	BilinearPatch patch2;
	patch2.pts[0] = top[0];
	patch2.pts[1] = bottom[0];
	patch2.pts[2] = top[2];
	patch2.pts[3] = bottom[2];

	// Now we need to basically intersect against all of these elements to figure which 
	// side the ray enters and which side it leaves
	TRIANGLE_HIT topcapHit, bottomcapHit;
	float u, v;

	//RayTriangleIntersection(rayOrigin, rayDirection, topcapHit, top[0], top[1] - top[0], top[2] - top[0]);
	//RayTriangleIntersection(rayOrigin, rayDirection, bottomcapHit, bottom[0], bottom[1] - bottom[0], bottom[2] - bottom[0]);


	topcapHit.dRange = Triangle::intersection(top[0], top[1], top[2], rayOrigin, rayDirection, u, v);
	if (topcapHit.dRange >= 0)
	{
		topcapHit.dRange2 = topcapHit.dRange;
		topcapHit.bHit = true;
		topcapHit.alpha = u;
		topcapHit.beta = 1.0f - u - v;
	}

	bottomcapHit.dRange = Triangle::intersection(bottom[0], bottom[1], bottom[2], rayOrigin, rayDirection, u, v);
	if (bottomcapHit.dRange >= NEARZERO)
	{
		bottomcapHit.dRange2 = bottomcapHit.dRange;
		bottomcapHit.bHit = true;
		bottomcapHit.alpha = v;
		bottomcapHit.beta = 1.0f - u - v;
	}


	BILINEAR_HIT bh0, bh1, bh2;

	RayBilinearPatchIntersection(rayOrigin, rayDirection, bh0, patch0);
	RayBilinearPatchIntersection(rayOrigin, rayDirection, bh1, patch1);
	RayBilinearPatchIntersection(rayOrigin, rayDirection, bh2, patch2);

	// Now we have to figure out which the closest hits were and which the farthest hits were
	// Fortunately we can do this seperately

	// See if either of the end-caps are an entry point,
	char in=-1, out=-1;							// describes which side the ray comes in and which side it goes out
	float inDist=INFINITY, outDist=0;			// in and out distances

	if( topcapHit.bHit )
	{
		in = 0; 
		out = 0;
		inDist = topcapHit.dRange;
		outDist = topcapHit.dRange;
	}

	if( bottomcapHit.bHit )
	{
		if( bottomcapHit.dRange < inDist )
		{
			inDist = bottomcapHit.dRange;
			in = 1;
		}

		if( bottomcapHit.dRange > outDist )
		{
			outDist = bottomcapHit.dRange;
			out = 1;
		}
	}

	if (inDist < 0 || outDist < 0)
		return -1;

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
		return -1;	// the volume is never intersected
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
	const int N = texture->GetWidth();
	const float delta = 1.0f / static_cast<float>(N);
	float alpha, beta, gamma;

	FIND_BARY( out );
	int ie = int(floor(alpha*N));
	int je = int(floor(beta*N));
	int ke = (int)(floor(gamma*N));

	FIND_BARY( in );
	int i = int(floor(alpha*N));
	int j = int(floor(beta*N));
	int k = int(floor(gamma*N));


	typedef enum LastChange
	{
		iplus = 0,
		jminus,
		kplus,
		iminus,
		jplus,
		kminus
	};

	Vector3d cNormal;
	Vector3d intersectionNormal;

	Vector3d color; float grayColor; float displacementHeight;

#define GET_DISPLACEMENT_HEIGHT(U,V) \
	static_cast<float>(1.0f - ((color = texture->GetTexel(U, V)).x + color.y + color.z) / 3);

	
	Vector3d uva;
	Vector3d uvb;
	Vector3d uvc;
	Vector3d a;
	Vector3d b;
	Vector3d c, basicC;
	LastChange change;

	if (i == 0)
	{
		uva.set(beta, gamma, 0);
		uvb.set(beta+delta, gamma-delta, 0);
		uvc.set(beta, gamma-delta, 0);

		change = iplus;
	}
	else if (j == 0)
	{
		uva.set(0, gamma, 0);
		uvb.set(0, gamma-delta, 0);
		uvc.set(delta, gamma-delta, 0);

		change = jplus;
	}
	else if (k == 0)
	{
		uva.set(beta, 0, 0);
		uvb.set(beta+delta, 0, 0);
		uvc.set(beta, delta, 0);

		change = kplus;
	}

	Vector3d dirDown = p2-p3;
	Vector3d dirRight = p1-p2;
	a = p3 + dirDown*(1.0f-uva.y) + dirRight*(uva.x);
	b = p3 + dirDown*(1.0f-uvb.y) + dirRight*(uvb.x);
	c = p3 + dirDown*(1.0f-uvc.y) + dirRight*(uvc.x);
	basicC = c;

	// Displace
	//a += normals[0] * M; //GET_DISPLACEMENT_HEIGHT(uva.x, uva.y);
	//b += normals[0] * M;
	//c += normals[0] * M;
	cNormal = normals[0];

	float testIntersection = Triangle::intersection(p1, p2, p3, rayOrigin, rayDirection);
	Point3d intersectionPt = rayOrigin + rayDirection*testIntersection;

	int ni = 0;
	float eps = delta;
	Vector3d vTmp;
	while (ni++ < 100)
	{
		testIntersection = Triangle::intersection(a, b, c, rayOrigin, rayDirection, u, v);
		//testIntersection = Triangle::intersection(a+(b-a)*eps+(c-a)*eps, b+(a-b)*eps+(c-b)*eps, c+(a-c)*eps+(b-c)*eps, rayOrigin, rayDirection);
		if (testIntersection > -std::numeric_limits<float>::epsilon())
		{
			intersectionNormal = (b-a).crossProduct(c-a, true);
			return testIntersection;
		}

		if ((i == ie && j == je && k == ke) || i >= N || j >= N || k >= N)
		{
			return -1;
		}

		float tmp = (cNormal.crossProduct(rayOrigin - c, true)).dotProduct(rayDirection);
		bool rightOfC = (tmp > - std::numeric_limits<float>::epsilon());

		if (!rightOfC)
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
			if (++i >= N) break;
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
		//uvc = uvc*(1+std::numeric_limits<float>::epsilon());

		// Get color and convert it to a gray scale
		Vector3d color = texture->GetTexel(uvc.x, 1.0f-uvc.y);
		float grayColor = M;//static_cast<float>(1.0f - (color.x+color.y+color.z) / 3);

		// (c,cNormal) = GetPoint(uvc)
		//c = basicC + GET_DISPLACEMENT_HEIGHT(uvc.x, uvc.y);

		c = p3 + (p3-p2)*(1.0f-uvc.y) + (p1-p2)*uvc.x;
		
		Vector3d ab(a, b, false);
		Vector3d ac(a, c, false);
		Vector3d microTriangleNormal = ab.crossProduct(ac, true);
		//cNormal = microTriangleNormal;
		//c += cNormal*M;
	}

	return -1;
}
/************************************ CUT *************************************/
/******************************************************************************/
/*    Ray-triangle intersection where the triangle is displacement mapped.    */
/******************************************************************************/