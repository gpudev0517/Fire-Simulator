#pragma once

//! Implementation of the 2D vector class (integer precision)
struct vec2i
{
public:
	vec2i() {}
	vec2i(int x, int y):m_X(x), m_Y(y){}

	// get functions
	int x() const {return m_X;}
	int y() const {return m_Y;}

	// set functions
	void setX(const int x) {m_X = x;}
	void setY(const int y) {m_Y = y;}
	void set(const int x, const int y) {m_X = x; m_Y = y;}

	vec2i operator+(const vec2i& v2) const {return vec2i(m_X + v2.m_X, m_Y + v2.m_Y);}

private:
	int m_X, m_Y;
};

//! Implementation of the 3D vector class (integer precision)
struct vec3i
{
public:
	vec3i() {}
	vec3i(int x, int y, int z):m_X(x), m_Y(y), m_Z(z){}

	// get functions
	int x() const {return m_X;}
	int y() const {return m_Y;}
	int z() const {return m_Z;}

	// set functions
	void setX(const int x) {m_X = x;}
	void setY(const int y) {m_Y = y;}
	void setZ(const int z) {m_Z = z;}
	void set(const int x, const int y, const int z) {m_X = x; m_Y = y; m_Z = z;}

	vec3i operator+(const vec3i& v2) const {return vec3i(m_X+ v2.m_X, m_Y + v2.m_Y, m_Z + v2.m_Z);}

	inline int& operator [] (uint i) { return (&m_X)[i]; }
	inline int operator [] (uint i) const { return (&m_X)[i]; }
    inline bool operator==(const vec3i &rhs){ return m_X == rhs.m_X && m_Y == rhs.m_Y && m_Z == rhs.m_Z; }

    NEFEXP_CORE operator QVariant() const
    {
        return QVariant(getNEVarIds().vec3iId, this);
    }

    NEFEXP_CORE QString toUtf8() const
    {
        return QString("(%1,%2,%3)").arg(x()).arg(y()).arg(z());
    }

private:
	int m_X, m_Y, m_Z;
};

//! Implementation of the 4D vector class (integer precision)
struct vec4i
{
public:
	vec4i() {}
	vec4i(int x, int y, int z, int w):m_X(x), m_Y(y), m_Z(z), m_W(w){}

	// get functions
	int x() const {return m_X;}
	int y() const {return m_Y;}
	int z() const {return m_Z;}
	int w() const {return m_W;}

	// set functions
	void setX(const int x) {m_X = x;}
	void setY(const int y) {m_Y = y;}
	void setZ(const int z) {m_Z = z;}
	void setW(const int w) {m_W = w;}
	void set(const int x, const int y, const int z, const int w) {m_X = x; m_Y = y; m_Z = z; m_W = w;}

	vec4i operator+(const vec4i& v2) const {return vec4i(m_X+ v2.m_X, m_Y + v2.m_Y, m_Z + v2.m_Z, m_W + v2.m_W);}

private:
	int m_X, m_Y, m_Z, m_W;
};

Q_DECLARE_METATYPE(vec3i)
