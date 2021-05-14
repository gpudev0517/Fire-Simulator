inline vec2f::vec2f() : m_X(0.0f), m_Y(0.0f) {}
inline vec2f::vec2f(float xpos, float ypos, int) : m_X(xpos), m_Y(ypos) {}
inline vec2f::vec2f(float xpos, float ypos) : m_X(xpos), m_Y(ypos) {}
inline vec2f::vec2f(const vec2f& vec) :m_X(vec.m_X), m_Y(vec.m_Y) {}

inline float vec2f::x() const { return m_X; }
inline float vec2f::y() const { return m_Y; }

inline float& vec2f::x() { return m_X; }
inline float& vec2f::y() { return m_Y; }

inline void vec2f::setX(float aX) { m_X = aX; }
inline void vec2f::setY(float aY) { m_Y = aY; }
inline void vec2f::set(float x, float y) {m_X = x; m_Y = y;}
inline void vec2f::set(const vec2f& vec) {m_X = vec.m_X; m_Y = vec.m_Y;}

inline vec2f &vec2f::operator+=(const vec2f &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	return *this;
}

inline vec2f &vec2f::operator-=(const vec2f &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	return *this;
}

inline vec2f &vec2f::operator*=(float factor)
{
	m_X *= factor;
	m_Y *= factor;
	return *this;
}

inline vec2f &vec2f::operator*=(const vec2f &vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	return *this;
}

inline vec2f &vec2f::operator/=(float divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	return *this;
}

inline float vec2f::operator * (const vec2f& m) const
{
	return m_X*m.m_X + m_Y*m.m_Y;

}

inline vec3f::vec3f() : m_X(0.0f), m_Y(0.0f), m_Z(0.0f) {}

inline vec3f::vec3f(float xpos, float ypos, float zpos) : m_X(xpos), m_Y(ypos), m_Z(zpos) {}

inline vec3f::vec3f(float xpos, float ypos, float zpos, int) : m_X(xpos), m_Y(ypos), m_Z(zpos) {}


inline float vec3f::x() const { return float(m_X); }
inline float vec3f::y() const { return float(m_Y); }
inline float vec3f::z() const { return float(m_Z); }

inline float& vec3f::x() { return m_X; }
inline float& vec3f::y() { return m_Y; }
inline float& vec3f::z() { return m_Z; }

inline void vec3f::setX(float aX) { m_X = aX; }
inline void vec3f::setY(float aY) { m_Y = aY; }
inline void vec3f::setZ(float aZ) { m_Z = aZ; }
inline void vec3f::set(float x, float y, float z) {m_X = x; m_Y = y; m_Z = z;}
inline void vec3f::set(const vec3f& vec) {m_X = vec.m_X; m_Y = vec.m_Y; m_Z = vec.m_Z;}

inline float  vec3f::length() const
{
	return sqrt(m_X * m_X + m_Y * m_Y + m_Z * m_Z);
}

inline float vec3f::lengthSquaredInline() const
{
	return m_X * m_X + m_Y * m_Y + m_Z * m_Z;
}

inline vec3f &vec3f::operator+=(const vec3f &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	m_Z += vector.m_Z;
	return *this;
}

inline vec3f &vec3f::operator-=(const vec3f &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	m_Z -= vector.m_Z;
	return *this;
}

inline vec3f &vec3f::operator*=(float factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	return *this;
}

inline vec3f &vec3f::operator*=(const vec3f& vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	m_Z *= vector.m_Z;
	return *this;
}

inline vec3f &vec3f::operator/=(float divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	return *this;
}

inline vec4f::vec4f() : m_X(0.0f), m_Y(0.0f), m_Z(0.0f), m_W(0.0f) {}

inline vec4f::vec4f(float xpos, float ypos, float zpos, float wpos) : m_X(xpos), m_Y(ypos), m_Z(zpos), m_W(wpos) {}

inline vec4f::vec4f(float xpos, float ypos, float zpos, float wpos, int) : m_X(xpos), m_Y(ypos), m_Z(zpos), m_W(wpos) {}

inline float vec4f::x() const { return float(m_X); }
inline float vec4f::y() const { return float(m_Y); }
inline float vec4f::z() const { return float(m_Z); }
inline float vec4f::w() const { return float(m_W); }

inline float& vec4f::x() { return m_X; }
inline float& vec4f::y() { return m_Y; }
inline float& vec4f::z() { return m_Z; }
inline float& vec4f::w() { return m_W; }

inline void vec4f::setX(float aX) { m_X = aX; }
inline void vec4f::setY(float aY) { m_Y = aY; }
inline void vec4f::setZ(float aZ) { m_Z = aZ; }
inline void vec4f::setW(float aW) { m_W = aW; }
inline void vec4f::set(float x, float y, float z, float w)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_W = w;
}
inline void vec4f::set(const vec4f& vec)
{
	m_X = vec.m_X;
	m_Y = vec.m_Y;
	m_Z = vec.m_Z;
	m_W = vec.m_W;
}

inline vec4f &vec4f::operator+=(const vec4f &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	m_Z += vector.m_Z;
	m_W += vector.m_W;
	return *this;
}

inline vec4f &vec4f::operator-=(const vec4f &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	m_Z -= vector.m_Z;
	m_W -= vector.m_W;
	return *this;
}

inline vec4f &vec4f::operator*=(float factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	m_W *= factor;
	return *this;
}

inline vec4f &vec4f::operator*=(const vec4f &vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	m_Z *= vector.m_Z;
	m_W *= vector.m_W;
	return *this;
}

inline vec4f &vec4f::operator/=(float divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	m_W /= divisor;
	return *this;
}
