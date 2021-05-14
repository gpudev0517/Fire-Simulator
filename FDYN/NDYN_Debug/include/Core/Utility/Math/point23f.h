#pragma once

#include "Utility/Math/vec234f.h"
struct point2d;
struct point3d;

//! Implementation of the 2D point class (float percision)
struct point2f
{
private:
	float m_X, m_Y;
public:
	inline point2f() {}
	inline point2f(float nx, float ny) : m_X(nx), m_Y(ny) {}
	inline point2f(const point2f& p) : m_X(p.m_X), m_Y(p.m_Y) {}
    NEFEXP_CORE point2f(const point2d& p);

	//! Get methods
	inline float x() const { return m_X; }
	inline float& x() { return m_X; }
	inline float y() const { return m_Y; }
	inline float& y() { return m_Y; }
	inline point2f xy() const { return *this; }
	inline point2f& xy() { return *this; }

	//! Set methods
	inline void set(const float x, const float y){ m_X = x; m_Y = y; }
	inline void set(const point2f& p){ m_X = p.m_X; m_Y = p.m_Y; }
	inline void setX(const float x){ m_X = x; }
	inline void setY(const float y){ m_Y = y; }
    NEFEXP_CORE void set(const point2d& p);
	inline void makeZero() { m_X = m_Y = 0.0f; }

	//! Operators
	inline point2f operator - () const { return point2f(-m_X,-m_Y); }
	// Point + vector -> point
	inline point2f operator + (const vec2f& vec) const { return point2f(m_X+vec.x(), m_Y+vec.y()); }
	// Point - vector -> point
	inline point2f operator - (const vec2f& vec) const { return point2f(m_X-vec.x(), m_Y-vec.y()); }
	// Point - point -> vector
	inline vec2f operator - (const point2f& p) const { return vec2f(m_X-p.m_X, m_Y-p.m_Y); }
	inline point2f operator * (float a) const { return point2f(a*m_X, a*m_Y); }
	inline point2f& operator = (const point2f& p)
	{
		m_X = p.m_X; 
		m_Y = p.m_Y;
		return *this;
	}
	inline bool operator == (const point2f& p) const
	{
		return (FAreEqual(m_X, p.m_X) && FAreEqual(m_Y, p.m_Y));
	}
	inline bool operator != (const point2f& p) const
	{
		return (!FAreEqual(m_X, p.m_X) || !FAreEqual(m_Y, p.m_Y));
	}
	inline point2f operator / (float a) const
	{
		float oneOverA = 1.0f / a;
		return point2f(m_X*oneOverA, m_Y*oneOverA);
	}
	inline point2f& operator += (const vec2f& vec)
	{
		m_X += vec.x(); m_Y += vec.y();
		return *this;
	}
	inline point2f& operator -= (const vec2f& vec) 
	{ 
		m_X -= vec.x(); m_Y -= vec.y();
		return *this;
	}
	inline point2f& operator *= (float a) 
	{ 
		m_X *= a; m_Y *= a;
		return *this;
	}
	inline point2f& operator /= (float a) 
	{
		float oneOverA = 1.0f / a;
		m_X *= oneOverA; m_Y *= oneOverA;
		return *this;
	}

	//Other point related methods
	friend inline point2f operator * (float k, const point2f& p);	
	friend inline QDataStream& operator>> (QDataStream& stream, point2f& p);
	friend inline QDataStream& operator<< (QDataStream& stream, const point2f& p);
};

inline point2f operator * (float k, const point2f& p)
{
	return point2f(p.m_X*k, p.m_Y*k);
}

QDataStream& operator>> (QDataStream& stream, point2f& p)
{
	float x, y;
	stream >> x;
	stream >> y;
	p.set(x,y);
	return stream;
}

QDataStream& operator<< (QDataStream& stream, const point2f& p)
{
	stream << p.m_X << p.m_Y;
	return stream;
}

//! Implementation of the 3D point class (float precision)
struct point3f
{
private:
	float m_X, m_Y, m_Z;
public:
	inline point3f() {}
	inline point3f(float nx, float ny, float nz) : m_X(nx), m_Y(ny),m_Z(nz) {}
	inline point3f(const point3f& p) : m_X(p.m_X), m_Y(p.m_Y), m_Z(p.m_Z) {}
    NEFEXP_CORE point3f(const point3d& p);

	//! Get methods
	inline float x() const { return m_X; }
	inline float& x() { return m_X; }
	inline float y() const { return m_Y; }
	inline float& y() { return m_Y; }
	inline float z() const { return m_Z; }
	inline float& z() { return m_Z; }
	inline const point3f xyz() const { return *this; }
	inline point3f& xyz() { return *this; }

	//! Set methods
	inline void set(const float x, const float y, const float z){ m_X = x; m_Y = y; m_Z = z;}
	inline void set(const point3f& p){ m_X = p.m_X; m_Y = p.m_Y; m_Z = p.m_Z;}
	inline void setX(const float x){ m_X = x; }
	inline void setY(const float y){ m_Y = y; }
	inline void setZ(const float z){ m_Z = z; }
    NEFEXP_CORE void set(const point3d& p);

	//! Operators
	inline point3f operator - () const { return point3f(-m_X,-m_Y,-m_Z); }
	// Point + vector -> point
	inline point3f operator + (const vec3f& vec) const { return point3f(m_X+vec.x(), m_Y+vec.y(), m_Z+vec.z()); }
	// Point - vector -> point
	inline point3f operator - (const vec3f& vec) const { return point3f(m_X-vec.x(), m_Y-vec.y(), m_Z-vec.z()); }
	// Point - point -> vector
	inline vec3f operator - (const point3f& p) const { return vec3f(m_X-p.m_X, m_Y-p.m_Y, m_Z-p.m_Z); }
	inline point3f operator * (float a) const { return point3f(a*m_X, a*m_Y, a*m_Z); }
	inline point3f& operator = (const point3f& p)
	{
		m_X = p.m_X; 
		m_Y = p.m_Y;
		m_Z = p.m_Z;
		return *this;
	}
	inline bool operator == (const point3f& p) const
	{
		return (FAreEqual(m_X, p.m_X) && FAreEqual(m_Y, p.m_Y) && FAreEqual(m_Z, p.m_Z));
	}
	inline bool operator != (const point3f& p) const
	{
		return (!FAreEqual(m_X, p.m_X) || !FAreEqual(m_Y, p.m_Y) || !FAreEqual(m_Z, p.m_Z));
	}
	inline point3f operator / (float a) const
	{
		float oneOverA = 1.0f / a;
		return point3f(m_X*oneOverA, m_Y*oneOverA, m_Z*oneOverA);
	}
	inline point3f& operator += (const vec3f& vec)
	{
		m_X += vec.x(); m_Y += vec.y(); m_Z  += vec.z();
		return *this;
	}
	inline point3f& operator -= (const vec3f& vec) 
	{ 
		m_X -= vec.x(); m_Y -= vec.y(); m_Z -= vec.z();
		return *this;
	}
	inline point3f& operator *= (float a) 
	{ 
		m_X *= a; m_Y *= a; m_Z *= a;
		return *this;
	}
	inline point3f& operator /= (float a) 
	{
		float oneOverA = 1.0f / a;
		m_X *= oneOverA; m_Y *= oneOverA; m_Z *= oneOverA;
		return *this;
	}

	//Other point related methods
	inline void makeZero() { m_X = m_Y = m_Z = 0.0f; }
	//! Computes the distance to a plane which is defined by its normal and a vertex. Given normal is assumed to be normalized.
	// Result -> 0: on the plane, -: below the plane, +: above the plane. 
	inline float distanceToPlane(const point3f& pOnPlane, const vec3f& planeNormal) const
	{
		vec3f v = *this - pOnPlane;
		return v*planeNormal;	
	}
	//! Computes the distance to a plane which is defined by its 3 vertices. 
	inline float distanceToPlane(const point3f& pOnPlane1, const point3f& pOnPlane2, const point3f& pOnPlane3) const
	{
		vec3f v1 = pOnPlane2 - pOnPlane1;
		vec3f v2 = pOnPlane3 - pOnPlane1;
		vec3f normal = v1%v2;
		vec3f v = *this - pOnPlane1;
		return v*normal;	
	}
	//! Computes the distance to a line which is defined by its initial point and direction. Given direction is assumed to be normalized.
	// Line eqn: x = x0 + at, y = y0 + bt, z = z0 + ct. (x0,y0,z0) initial points, (a,b,c) define the slope.
	inline float distanceToLine(const point3f& p, const vec3f& direction) const
	{
		vec3f v1 = *this - p;
		float f = v1 * direction;
		point3f p1 = p + (f * direction);
		vec3f v = *this - p1;
		return v.length();	
	}
	friend inline point3f operator * (float k, const point3f& p);	
	friend inline QDataStream& operator>> (QDataStream& stream, point3f& p);
	friend inline QDataStream& operator<< (QDataStream& stream, const point3f& p);
};

inline point3f operator * (float k, const point3f& p)
{
	return point3f(p.m_X*k, p.m_Y*k, p.m_Z*k);
}

QDataStream& operator>> (QDataStream& stream, point3f& p)
{
	float x, y, z;
	stream >> x;
	stream >> y;
	stream >> z;
	p.set(x,y,z);
	return stream;
}

QDataStream& operator<< (QDataStream& stream, const point3f& p)
{
	stream << p.m_X << p.m_Y << p.m_Z;
	return stream;
}


