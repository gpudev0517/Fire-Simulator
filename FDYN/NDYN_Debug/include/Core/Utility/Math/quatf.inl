inline quatf::quatf() : m_W(1.0f), m_X(0.0f), m_Y(0.0f), m_Z(0.0f) {}

inline quatf::quatf(float aScalar, float xpos, float ypos, float zpos) : m_W(aScalar), m_X(xpos), m_Y(ypos), m_Z(zpos) {}

inline bool quatf::isNull() const
{
	return m_X == 0.0f && m_Y == 0.0f && m_Z == 0.0f && m_W == 0.0f;
}

inline bool quatf::isIdentity() const
{
	return m_X == 0.0f && m_Y == 0.0f && m_Z == 0.0f && m_W == 1.0f;
}

inline float quatf::x() const { return float(m_X); }
inline float quatf::y() const { return float(m_Y); }
inline float quatf::z() const { return float(m_Z); }
inline float quatf::scalar() const { return float(m_W); }

inline void quatf::setX(float aX) { m_X = aX; }
inline void quatf::setY(float aY) { m_Y = aY; }
inline void quatf::setZ(float aZ) { m_Z = aZ; }
inline void quatf::setScalar(float aScalar) { m_W = aScalar; }

inline quatf quatf::conjugate() const
{
	return quatf(m_W, -m_X, -m_Y, -m_Z);
}

inline quatf& quatf::operator+=(const quatf& quat)
{
	m_X += quat.m_X;
	m_Y += quat.m_Y;
	m_Z += quat.m_Z;
	m_W += quat.m_W;
	return *this;
}

inline quatf& quatf::operator-=(const quatf& quat)
{
	m_X -= quat.m_X;
	m_Y -= quat.m_Y;
	m_Z -= quat.m_Z;
	m_W -= quat.m_W;
	return *this;
}

inline quatf& quatf::operator*=(float factor)
{
	m_X *= factor;
	m_Y *= factor;
	m_Z *= factor;
	m_W *= factor;
	return *this;
}

inline quatf& quatf::operator*=(const quatf& quat)
{
	*this = *this * quat;
	return *this;
}

inline quatf& quatf::operator/=(float divisor)
{
	m_X /= divisor;
	m_Y /= divisor;
	m_Z /= divisor;
	m_W /= divisor;
	return *this;
}

inline void quatf::setVector(float aX, float aY, float aZ)
{
	m_X = aX;
	m_Y = aY;
	m_Z = aZ;
}