#pragma once

//#include <math.h>

class vec2f;
class vec3f;
class matrix33f;
class quatf;
class eulerf;

#define FPi 3.1415926535897932384626433832795f
#define FPiHalf 1.5707963267948966192313216916398f
#define FPiOver180 0.01745329251994329576923690768489f
#define FEps 1.0e-6f
#define FTwoPi 2.0f*FPi
#define DEps 2.2204460492503131e-015
#define DPi 3.1415926535897932384626433832795
#define DHalfPi 1.5707963267948966192313216916398
#define DTwoPi 2.0*DPi

inline bool FIsZero(float a)
{
	return (fabsf(a) < FEps);
}

inline bool FIsExactlyZero(float a)
{
	return (fabsf(a) == 0.0f);
}

inline bool FAreEqual(float a, float b)
{
	return FIsZero(a - b);
}

inline bool FAreExactlyEqual(float a, float b)
{
	return FIsExactlyZero(a - b);
}

inline void FSinCos(float a, float& s, float& c)
{
	s = sinf(a);
	c = cosf(a);
}

inline float FDegToRad(float deg)
{
	return (FPi / 180.0f) * deg;
}

inline int  FSignum(float a)
{
	return (0.0f < a) - (a < 0.0f);
}

inline bool DIsZero(double a)
{
	return (fabs(a) < DEps);
}

inline bool DIsExactlyZero(double a)
{
	return (fabs(a) == 0.0);
}

inline bool DAreEqual(double a, double b)
{
	return DIsZero(a - b);
}

inline bool DAreExactlyEqual(double a, double b)
{
	return DIsExactlyZero(a - b);
}

inline void DSinCos(double a, double& s, double& c)
{
	s = sin(a);
	c = cos(a);
}

NEFEXP_CORE matrix33f matrixFromQuaternions(const quatf& q);
int mapAnglesToAvoidSingularities(eulerf& e1, eulerf& e2);
eulerf mapEulerAngleToPositiveValue(const eulerf& e);
NEFEXP_CORE eulerf eulerAnglesFromMatrix(const matrix33f& m);
NEFEXP_CORE eulerf eulerAnglesFromQuaternion(const quatf& q);
NEFEXP_CORE eulerf eulerAnglesFromDirection(const vec3f& ds);
NEFEXP_CORE vec3f directionFromEulerAngles(const eulerf& ds);
NEFEXP_CORE quatf quaternionsFromEulerAngles(const eulerf& e);

NEFEXP_CORE vec2f linearInterpolation(double frameA, vec2f positionA, double frameB, vec2f positionB, double currentFrame);
NEFEXP_CORE vec3f linearInterpolation(double frameA, vec3f positionA, double frameB, vec3f positionB, double currentFrame);
NEFEXP_CORE quatf linearInterpolation(double frameA, quatf rotationA, double frameB, quatf rotationB, double currentFrame);
NEFEXP_CORE bool linearInterpolation(double frameA, bool flagA, double frameB, bool flagB, double currentFrame);
NEFEXP_CORE QColor linearInterpolation(double frameA, QColor colorA, double frameB, QColor colorB, double currentFrame);
NEFEXP_CORE int linearInterpolation(double frameA, int valA, double frameB, int valB, double currentFrame);
NEFEXP_CORE double linearInterpolation(double frameA, double valA, double frameB, double valB, double currentFrame);
NEFEXP_CORE eulerf linearInterpolation(double frameA, eulerf valA, double frameB, eulerf valB, double currentFrame);

template <class T>
inline T interpolate(const T val, const T val1, const T val2)
{
	T delta = val2 - val1;

	if(fabs(val - val1) < FEps)
		return 0.0f;
	else if(fabs(val - val2) < FEps)
		return 1.0f;
	else if(fabs(delta) < FEps)
		return 0.5f; //we shouldnt get this case <- might remove this line
	else
		return (val - val1) / delta;
}


//!
// Calculates the intersection point between two lines, assuming that there is such a point.
// PRECONDITION: Intersection point must exist
//
// originD: The origin of the first line
// directionD: The direction of the first line.
// originE: The origin of the second line.
// directionE: The direction of the second line.
//
// Returns the point at which the two lines intersect.
NEFEXP_CORE vec3f lineLineIntersection(vec3f originD, vec3f directionD, vec3f originE, vec3f directionE);
NEFEXP_CORE float pointLineDistance(vec3f point, vec3f linepoint, vec3f linedirection);

// Returns that point lying on the triangle formed by t1, t2 and t3 whose distane to p is minimized
vec3f closestPointOnTriangle(vec3f p, vec3f t1, vec3f t2, vec3f t3);

// Returns that point on the plane formed by p and n whose distance to q is minimized
vec3f closestPointOnPlane(vec3f p, vec3f n, vec3f q);

// Returns the intersection point of a line and a plane
// Returns NaN values if the line and plana are parallel
NEFEXP_CORE vec3f linePlaneInterPoint(const vec3f& linePoint, const vec3f& lineVector, const vec3f& planePoint, const vec3f& planeNormal);

// SVD related
NEFEXP_CORE float hypotSVD(const float& a, const float& b);

//extern NEVariantIds NEVarIds;
NEFEXP_CORE struct NEVariantIds& getNEVarIds();

//Mapping functions for mapping a value to interval [newMin..newMax] (sort of interpolated clamping) used e.g. in foam
double mapValueToInterval(double value, const double& min, const double& max, const double newMin = 0.f, const double newMax = 1.f, const uint method = NE::kMapLinear);

class NEIndexedTriMesh;
class matrix33d;

NEFEXP_CORE int computePolyhedralProperties(NEIndexedTriMesh& triMesh, double density, double &volume, vec3f& com, matrix33d& moi);
