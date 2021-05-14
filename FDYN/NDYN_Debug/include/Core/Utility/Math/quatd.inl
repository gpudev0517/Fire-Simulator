inline quatd::quatd() : m_W(1.0), m_X(0.0), m_Y(0.0), m_Z(0.0) {}

inline quatd::quatd(double s, double x, double y, double z) : m_W(s), m_X(x), m_Y(y), m_Z(z) {}

inline bool quatd::isNull() const
{
	return DIsExactlyZero(m_X) && DIsExactlyZero(m_Y) && DIsExactlyZero(m_Z) && DIsExactlyZero(m_W);
}

inline bool quatd::isIdentity() const
{
	return DIsExactlyZero(m_X) && DIsExactlyZero(m_Y) && DIsExactlyZero(m_Z) && m_W == 1.0;
}

inline double quatd::x() const { return double(m_X); }
inline double quatd::y() const { return double(m_Y); }
inline double quatd::z() const { return double(m_Z); }
inline double quatd::scalar() const { return double(m_W); }

inline void quatd::setX(double x) { m_X = x; }
inline void quatd::setY(double y) { m_Y = y; }
inline void quatd::setZ(double z) { m_Z = z; }
inline void quatd::setScalar(double s) { m_W = s; }

inline quatd quatd::conjugate() const
{
	return quatd(m_W, -m_X, -m_Y, -m_Z);
}

inline quatd& quatd::operator+=(const quatd& quat)
{
	m_X += quat.m_X;
	m_Y += quat.m_Y;
	m_Z += quat.m_Z;
	m_W += quat.m_W;
	return *this;
}

inline quatd& quatd::operator-=(const quatd& quat)
{
	m_X -= quat.m_X;
	m_Y -= quat.m_Y;
	m_Z -= quat.m_Z;
	m_W -= quat.m_W;
	return *this;
}

inline quatd& quatd::operator*=(double factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	m_W *= factor;
	return *this;
}


inline quatd& quatd::operator*=(const quatd& quat)
{
	*this = *this * quat;
	return *this;
}

inline quatd& quatd::operator/=(double divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	m_W /= divisor;
	return *this;
}

inline void quatd::setVector(double x, double y, double z)
{
	m_X = x;
	m_Y = y;
	m_Z = z;
}