#pragma once

#include "Utility/Math/vec234d.h"
struct point2f;
struct point3f;

//! Implementation of the 2D point class (double percision)
struct point2d
{
private:
	double m_X, m_Y;
public:
	inline point2d() {}
	inline point2d(double nx, double ny) : m_X(nx), m_Y(ny) {}
	inline point2d(const point2d& p) : m_X(p.m_X), m_Y(p.m_Y) {}
    NEFEXP_CORE point2d(const point2f& p);

	//! Get methods
	inline double x() const { return m_X; }
	inline double& x() { return m_X; }
	inline double y() const { return m_Y; }
	inline double& y() { return m_Y; }
	inline point2d xy() const { return *this; }
	inline point2d& xy() { return *this; }

	//! Set methods
	inline void set(const double x, const double y){ m_X = x; m_Y = y; }
	inline void set(const point2d& p){ m_X = p.m_X; m_Y = p.m_Y; }
	inline void setX(const double x){ m_X = x; }
	inline void setY(const double y){ m_Y = y; }
    NEFEXP_CORE void set(const point2f& p);
	inline void makeZero() { m_X = m_Y = 0.0; }

	//! Operators
	inline point2d operator - () const { return point2d(-m_X,-m_Y); }
	// Point + vector -> point
	inline point2d operator + (const vec2d& vec) const { return point2d(m_X+vec.x(), m_Y+vec.y()); }
	// Point - vector -> point
	inline point2d operator - (const vec2d& vec) const { return point2d(m_X-vec.x(), m_Y-vec.y()); }
	// Point - point -> vector
	inline vec2d operator - (const point2d& p) const { return vec2d(m_X-p.m_X, m_Y-p.m_Y); }
	inline point2d operator * (double a) const { return point2d(a*m_X, a*m_Y); }
	inline point2d& operator = (const point2d& p)
	{
		m_X = p.m_X; 
		m_Y = p.m_Y;
		return *this;
	}
	inline bool operator == (const point2d& p) const
	{
		return (DAreEqual(m_X, p.m_X) && DAreEqual(m_Y, p.m_Y));
	}
	inline bool operator != (const point2d& p) const
	{
		return (!DAreEqual(m_X, p.m_X) || !DAreEqual(m_Y, p.m_Y));
	}
	inline point2d operator / (double a) const
	{
		double oneOverA = 1.0 / a;
		return point2d(m_X*oneOverA, m_Y*oneOverA);
	}
	inline point2d& operator += (const vec2d& vec)
	{
		m_X += vec.x(); m_Y += vec.y();
		return *this;
	}
	inline point2d& operator -= (const vec2d& vec) 
	{ 
		m_X -= vec.x(); m_Y -= vec.y();
		return *this;
	}
	inline point2d& operator *= (double a) 
	{ 
		m_X *= a; m_Y *= a;
		return *this;
	}
	inline point2d& operator /= (double a) 
	{
		double oneOverA = 1.0 / a;
		m_X *= oneOverA; m_Y *= oneOverA;
		return *this;
	}

	//Other point related methods
	friend inline point2d operator * (double k, const point2d& p);	
	friend inline QDataStream& operator>> (QDataStream& stream, point2d& p);
	friend inline QDataStream& operator<< (QDataStream& stream, const point2d& p);
};

inline point2d operator * (double k, const point2d& p)
{
	return point2d(p.m_X*k, p.m_Y*k);
}

QDataStream& operator>> (QDataStream& stream, point2d& p)
{
	double x, y;
	stream >> x;
	stream >> y;
	p.set(x,y);
	return stream;
}

QDataStream& operator<< (QDataStream& stream, const point2d& p)
{
	stream << p.m_X << p.m_Y;
	return stream;
}

//! Implementation of the 3D point class (double percision)
struct point3d
{
private:
	double m_X, m_Y, m_Z;
public:
	inline point3d() {}
	inline point3d(double nx, double ny, double nz) : m_X(nx), m_Y(ny),m_Z(nz) {}
	inline point3d(const point3d& p) : m_X(p.m_X), m_Y(p.m_Y), m_Z(p.m_Z) {}
    NEFEXP_CORE point3d(const point3f& p);

	//! Get methods
	inline double x() const { return m_X; }
	inline double& x() { return m_X; }
	inline double y() const { return m_Y; }
	inline double& y() { return m_Y; }
	inline double z() const { return m_Z; }
	inline double& z() { return m_Z; }
	inline const point3d xyz() const { return *this; }
	inline point3d& xyz() { return *this; }

	//! Set methods
	inline void set(const double x, const double y, const double z){ m_X = x; m_Y = y; m_Z = z;}
	inline void set(const point3d& p){ m_X = p.m_X; m_Y = p.m_Y; m_Z = p.m_Z;}
	inline void setX(const double x){ m_X = x; }
	inline void setY(const double y){ m_Y = y; }
	inline void setZ(const double z){ m_Z = z; }
    NEFEXP_CORE void set(const point3f& p);

	//! Operators
	inline point3d operator - () const { return point3d(-m_X,-m_Y,-m_Z); }
	// Point + vector -> point
	inline point3d operator + (const vec3d& vec) const { return point3d(m_X+vec.x(), m_Y+vec.y(), m_Z+vec.z()); }
	// Point - vector -> point
	inline point3d operator - (const vec3d& vec) const { return point3d(m_X-vec.x(), m_Y-vec.y(), m_Z-vec.z()); }
	// Point - point -> vector
	inline vec3d operator - (const point3d& p) const { return vec3d(m_X-p.m_X, m_Y-p.m_Y, m_Z-p.m_Z); }
	inline point3d operator * (double a) const { return point3d(a*m_X, a*m_Y, a*m_Z); }
	inline point3d& operator = (const point3d& p)
	{
		m_X = p.m_X; 
		m_Y = p.m_Y;
		m_Z = p.m_Z;
		return *this;
	}
	inline bool operator == (const point3d& p) const
	{
		return (DAreEqual(m_X, p.m_X) && DAreEqual(m_Y, p.m_Y) && DAreEqual(m_Z, p.m_Z));
	}
	inline bool operator != (const point3d& p) const
	{
		return (!DAreEqual(m_X, p.m_X) || !DAreEqual(m_Y, p.m_Y) || !DAreEqual(m_Z, p.m_Z));
	}
	inline point3d operator / (double a) const
	{
		double oneOverA = 1.0 / a;
		return point3d(m_X*oneOverA, m_Y*oneOverA, m_Z*oneOverA);
	}
	inline point3d& operator += (const vec3d& vec)
	{
		m_X += vec.x(); m_Y += vec.y(); m_Z  += vec.z();
		return *this;
	}
	inline point3d& operator -= (const vec3d& vec) 
	{ 
		m_X -= vec.x(); m_Y -= vec.y(); m_Z -= vec.z();
		return *this;
	}
	inline point3d& operator *= (double a) 
	{ 
		m_X *= a; m_Y *= a; m_Z *= a;
		return *this;
	}
	inline point3d& operator /= (double a) 
	{
		double oneOverA = 1.0 / a;
		m_X *= oneOverA; m_Y *= oneOverA; m_Z *= oneOverA;
		return *this;
	}

	//Other point related methods
	inline void makeZero() { m_X = m_Y = m_Z = 0.0; }
	//! Computes the distance to a plane which is defined by its normal and a vertex. Given normal is assumed to be normalized.
	// Result -> 0: on the plane, -: below the plane, +: above the plane. 
	inline double distanceToPlane(const point3d& pOnPlane, const vec3d& planeUnitNormal) const
	{
		vec3d v = *this - pOnPlane;
		return v*planeUnitNormal;	
	}
	//! Computes the distance to a plane which is defined by its 3 vertices. 
	inline double distanceToPlane(const point3d& pOnPlane1, const point3d& pOnPlane2, const point3d& pOnPlane3) const
	{
		vec3d v1 = pOnPlane2 - pOnPlane1;
		vec3d v2 = pOnPlane3 - pOnPlane1;
		vec3d normal = v1%v2;
		vec3d v = *this - pOnPlane1;
		return v*normal;	
	}
	//! Computes the distance to a line which is defined by its initial point and direction. Given dierction is assumed to be normalized.
	// Line eqn: x = x0 + at, y = y0 + bt, z = z0 + ct. (x0,y0,z0) initial points, (a,b,c) define the slope.
	inline double distanceToLine(const point3d& p, const vec3d& direction) const
	{
		vec3d v1 = *this - p;
		double f = v1 * direction;
		point3d p1 = p + (f * direction);
		vec3d v = *this - p1;
		return v.length();	
	}
	friend inline point3d operator * (double k, const point3d& p);	
	friend inline QDataStream& operator>> (QDataStream& stream, point3d& p);
	friend inline QDataStream& operator<< (QDataStream& stream, const point3d& p);
};

inline point3d operator * (double k, const point3d& p)
{
	return point3d(p.m_X*k, p.m_Y*k, p.m_Z*k);
}

QDataStream& operator>> (QDataStream& stream, point3d& p)
{
	double x, y, z;
	stream >> x;
	stream >> y;
	stream >> z;
	p.set(x,y,z);
	return stream;
}

QDataStream& operator<< (QDataStream& stream, const point3d& p)
{
	stream << p.m_X << p.m_Y << p.m_Z;
	return stream;
}



