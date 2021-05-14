inline vec2d::vec2d() : m_X(0.0), m_Y(0.0) {}
inline vec2d::vec2d(double xpos, double ypos, int) : m_X(xpos), m_Y(ypos) {}
inline vec2d::vec2d(double xpos, double ypos) : m_X(xpos), m_Y(ypos) {}
inline vec2d::vec2d(const vec2d& vec) :m_X(vec.m_X), m_Y(vec.m_Y) {}

inline double vec2d::x() const { return m_X; }
inline double vec2d::y() const { return m_Y; }

inline double& vec2d::x() { return m_X; }
inline double& vec2d::y() { return m_Y; }

inline void vec2d::setX(double aX) { m_X = aX; }
inline void vec2d::setY(double aY) { m_Y = aY; }
inline void vec2d::set(double x, double y) {m_X = x; m_Y = y;}
inline void vec2d::set(const vec2d& vec) {m_X = vec.m_X; m_Y = vec.m_Y;}

inline vec2d &vec2d::operator+=(const vec2d &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	return *this;
}

inline vec2d &vec2d::operator-=(const vec2d &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	return *this;
}

inline vec2d &vec2d::operator*=(double factor)
{
	m_X *= factor;
	m_Y *= factor;
	return *this;
}

inline vec2d &vec2d::operator*=(const vec2d &vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	return *this;
}

inline vec2d &vec2d::operator/=(double divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	return *this;
}

inline vec2d& vec2d::operator = (const vec2d& vec)
{
	m_X = vec.m_X; 
	m_Y = vec.m_Y;
	return *this;
}

inline double vec2d::operator * (const vec2d& m) const
{
	return m_X*m.m_X + m_Y*m.m_Y;

}

inline vec3d::vec3d() : m_X(0.0), m_Y(0.0), m_Z(0.0) {}

inline vec3d::vec3d(float xpos, float ypos, float zpos) : m_X(xpos), m_Y(ypos), m_Z(zpos) {}

inline vec3d::vec3d(double xpos, double ypos, double zpos) : m_X(xpos), m_Y(ypos), m_Z(zpos) {}

inline vec3d::vec3d(double xpos, double ypos, double zpos, int) : m_X(xpos), m_Y(ypos), m_Z(zpos) {}

inline double vec3d::x() const { return double(m_X); }
inline double vec3d::y() const { return double(m_Y); }
inline double vec3d::z() const { return double(m_Z); }

inline double& vec3d::x() { return m_X; }
inline double& vec3d::y() { return m_Y; }
inline double& vec3d::z() { return m_Z; }

inline void vec3d::setX(double aX) { m_X = aX; }
inline void vec3d::setY(double aY) { m_Y = aY; }
inline void vec3d::setZ(double aZ) { m_Z = aZ; }
inline void vec3d::set(double x, double y, double z) {m_X = x; m_Y = y; m_Z = z;}
inline void vec3d::set(const vec3d& vec) {m_X = vec.m_X; m_Y = vec.m_Y; m_Z = vec.m_Z;}

inline vec3d &vec3d::operator+=(const vec3d &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	m_Z += vector.m_Z;
	return *this;
}

inline vec3d &vec3d::operator+=(const vec3f &vector)
{
        m_X += static_cast<double>(vector.x());
        m_Y += static_cast<double>(vector.y());
        m_Z += static_cast<double>(vector.z());
        return *this;
}

inline vec3d &vec3d::operator-=(const vec3d &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	m_Z -= vector.m_Z;
	return *this;
}

inline vec3d &vec3d::operator-=(const vec3f &vector)
{
        m_X -= static_cast<double>(vector.x());
        m_Y -= static_cast<double>(vector.y());
        m_Z -= static_cast<double>(vector.z());
        return *this;
}

inline vec3d &vec3d::operator*=(double factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	return *this;
}

inline vec3d &vec3d::operator*=(const vec3d& vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	m_Z *= vector.m_Z;
	return *this;
}

inline vec3d &vec3d::operator*=(const vec3f& vector)
{
        m_X *= static_cast<double>(vector.x());
        m_Y *= static_cast<double>(vector.y());
        m_Z *= static_cast<double>(vector.z());
        return *this;
}

inline vec3d &vec3d::operator/=(double divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	return *this;
}

inline vec3d& vec3d::operator=(const vec3d& vec)
{
	m_X = vec.m_X; 
	m_Y = vec.m_Y; 
	m_Z = vec.m_Z; 
	return *this;
}

//inline vec3d& vec3d::operator=(const vec3f& vec)
//{
//        m_X = static_cast<double>(vec.x());
//        m_Y = static_cast<double>(vec.y());
//        m_Z = static_cast<double>(vec.z());
//        return *this;
//}

inline vec4d::vec4d() : m_X(0.0), m_Y(0.0), m_Z(0.0), m_W(0.0) {}

inline vec4d::vec4d(double xpos, double ypos, double zpos, double wpos) : m_X(xpos), m_Y(ypos), m_Z(zpos), m_W(wpos) {}

inline vec4d::vec4d(double xpos, double ypos, double zpos, double wpos, int) : m_X(xpos), m_Y(ypos), m_Z(zpos), m_W(wpos) {}

inline double vec4d::x() const { return double(m_X); }
inline double vec4d::y() const { return double(m_Y); }
inline double vec4d::z() const { return double(m_Z); }
inline double vec4d::w() const { return double(m_W); }

inline double& vec4d::x() { return m_X; }
inline double& vec4d::y() { return m_Y; }
inline double& vec4d::z() { return m_Z; }
inline double& vec4d::w() { return m_W; }

inline void vec4d::setX(double aX) { m_X = aX; }
inline void vec4d::setY(double aY) { m_Y = aY; }
inline void vec4d::setZ(double aZ) { m_Z = aZ; }
inline void vec4d::setW(double aW) { m_W = aW; }
inline void vec4d::set(double x, double y, double z, double w)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_W = w;
}
inline void vec4d::set(const vec4d& vec)
{
	m_X = vec.m_X;
	m_Y = vec.m_Y;
	m_Z = vec.m_Z;
	m_W = vec.m_W;
}

inline vec4d &vec4d::operator+=(const vec4d &vector)
{
	m_X += vector.m_X;
	m_Y += vector.m_Y;
	m_Z += vector.m_Z;
	m_W += vector.m_W;
	return *this;
}

inline vec4d &vec4d::operator-=(const vec4d &vector)
{
	m_X -= vector.m_X;
	m_Y -= vector.m_Y;
	m_Z -= vector.m_Z;
	m_W -= vector.m_W;
	return *this;
}

inline vec4d &vec4d::operator*=(double factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	m_W *= factor;
	return *this;
}

inline vec4d &vec4d::operator*=(const vec4d &vector)
{
	m_X *= vector.m_X;
	m_Y *= vector.m_Y;
	m_Z *= vector.m_Z;
	m_W *= vector.m_W;
	return *this;
}

inline vec4d &vec4d::operator/=(double divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	m_W /= divisor;
	return *this;
}

inline vec4d& vec4d::operator=(const vec4d& vec)
{
	m_X = vec.m_X; 
	m_Y = vec.m_Y; 
	m_Z = vec.m_Z; 
	m_W = vec.m_W;
	return *this;

}
