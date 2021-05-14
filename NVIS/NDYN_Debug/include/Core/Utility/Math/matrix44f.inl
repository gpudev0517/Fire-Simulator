inline matrix44f::matrix44f (float m11, float m12, float m13, float m14,
							 float m21, float m22, float m23, float m24,
							 float m31, float m32, float m33, float m34,
							 float m41, float m42, float m43, float m44)
{
	m[0][0] = m11; m[0][1] = m21; m[0][2] = m31; m[0][3] = m41;
	m[1][0] = m12; m[1][1] = m22; m[1][2] = m32; m[1][3] = m42;
	m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; m[2][3] = m43;
	m[3][0] = m14; m[3][1] = m24; m[3][2] = m34; m[3][3] = m44;
	flagBits = General;
}

inline matrix44f::matrix44f( const matrix44f& matrix )
{
	m[0][0] = matrix.m[0][0]; m[0][1] = matrix.m[0][1]; m[0][2] = matrix.m[0][2]; m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0]; m[1][1] = matrix.m[1][1]; m[1][2] = matrix.m[1][2]; m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0]; m[2][1] = matrix.m[2][1]; m[2][2] = matrix.m[2][2]; m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0]; m[3][1] = matrix.m[3][1]; m[3][2] = matrix.m[3][2]; m[3][3] = matrix.m[3][3];

	flagBits = General;
}

inline const float& matrix44f::operator()(int aRow, int aColumn) const
{
	Q_ASSERT(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
	return m[aColumn][aRow];
}

inline float& matrix44f::operator()(int aRow, int aColumn)
{
	Q_ASSERT(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
	flagBits = General;
	return m[aColumn][aRow];
}

inline bool matrix44f::isIdentity() const
{
	if (flagBits == Identity)
		return true;
	if (m[0][0] != 1.0f || m[0][1] != 0.0f || m[0][2] != 0.0f)
		return false;
	if (m[0][3] != 0.0f || m[1][0] != 0.0f || m[1][1] != 1.0f)
		return false;
	if (m[1][2] != 0.0f || m[1][3] != 0.0f || m[2][0] != 0.0f)
		return false;
	if (m[2][1] != 0.0f || m[2][2] != 1.0f || m[2][3] != 0.0f)
		return false;
	if (m[3][0] != 0.0f || m[3][1] != 0.0f || m[3][2] != 0.0f)
		return false;
	return (m[3][3] == 1.0f);
}

inline void matrix44f::setToIdentity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
	flagBits = Identity;
}

inline bool matrix44f::isZero() const
{
	for(uint i=0; i < 4; i++)
		for(uint j=0; j < 4; j++)
			if(!FIsZero(m[i][j]))
				return false;

	return true;
}

inline void matrix44f::setToZero()
{
	m[0][0] = 0.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 0.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 0.0f;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 0.0f;
	flagBits = General;
}

inline void matrix44f::fill(float value)
{
	m[0][0] = value;
	m[0][1] = value;
	m[0][2] = value;
	m[0][3] = value;
	m[1][0] = value;
	m[1][1] = value;
	m[1][2] = value;
	m[1][3] = value;
	m[2][0] = value;
	m[2][1] = value;
	m[2][2] = value;
	m[2][3] = value;
	m[3][0] = value;
	m[3][1] = value;
	m[3][2] = value;
	m[3][3] = value;
	flagBits = General;
}

inline matrix44f& matrix44f::operator+=(const matrix44f& other)
{
	m[0][0] += other.m[0][0];
	m[0][1] += other.m[0][1];
	m[0][2] += other.m[0][2];
	m[0][3] += other.m[0][3];
	m[1][0] += other.m[1][0];
	m[1][1] += other.m[1][1];
	m[1][2] += other.m[1][2];
	m[1][3] += other.m[1][3];
	m[2][0] += other.m[2][0];
	m[2][1] += other.m[2][1];
	m[2][2] += other.m[2][2];
	m[2][3] += other.m[2][3];
	m[3][0] += other.m[3][0];
	m[3][1] += other.m[3][1];
	m[3][2] += other.m[3][2];
	m[3][3] += other.m[3][3];
	flagBits = General;
	return *this;
}

inline matrix44f& matrix44f::operator-=(const matrix44f& other)
{
	m[0][0] -= other.m[0][0];
	m[0][1] -= other.m[0][1];
	m[0][2] -= other.m[0][2];
	m[0][3] -= other.m[0][3];
	m[1][0] -= other.m[1][0];
	m[1][1] -= other.m[1][1];
	m[1][2] -= other.m[1][2];
	m[1][3] -= other.m[1][3];
	m[2][0] -= other.m[2][0];
	m[2][1] -= other.m[2][1];
	m[2][2] -= other.m[2][2];
	m[2][3] -= other.m[2][3];
	m[3][0] -= other.m[3][0];
	m[3][1] -= other.m[3][1];
	m[3][2] -= other.m[3][2];
	m[3][3] -= other.m[3][3];
	flagBits = General;
	return *this;
}

inline matrix44f& matrix44f::operator*=(const matrix44f& other)
{
	if (flagBits == Identity) {
		*this = other;
		return *this;
	} else if (other.flagBits == Identity) {
		return *this;
	} else {
		*this = *this * other;
		return *this;
	}
}

inline matrix44f& matrix44f::operator*=(float factor)
{
	m[0][0] *= factor;
	m[0][1] *= factor;
	m[0][2] *= factor;
	m[0][3] *= factor;
	m[1][0] *= factor;
	m[1][1] *= factor;
	m[1][2] *= factor;
	m[1][3] *= factor;
	m[2][0] *= factor;
	m[2][1] *= factor;
	m[2][2] *= factor;
	m[2][3] *= factor;
	m[3][0] *= factor;
	m[3][1] *= factor;
	m[3][2] *= factor;
	m[3][3] *= factor;
	flagBits = General;
	return *this;
}

inline bool matrix44f::operator==(const matrix44f& other) const
{
	return m[0][0] == other.m[0][0] &&
		m[0][1] == other.m[0][1] &&
		m[0][2] == other.m[0][2] &&
		m[0][3] == other.m[0][3] &&
		m[1][0] == other.m[1][0] &&
		m[1][1] == other.m[1][1] &&
		m[1][2] == other.m[1][2] &&
		m[1][3] == other.m[1][3] &&
		m[2][0] == other.m[2][0] &&
		m[2][1] == other.m[2][1] &&
		m[2][2] == other.m[2][2] &&
		m[2][3] == other.m[2][3] &&
		m[3][0] == other.m[3][0] &&
		m[3][1] == other.m[3][1] &&
		m[3][2] == other.m[3][2] &&
		m[3][3] == other.m[3][3];
}

inline bool matrix44f::operator!=(const matrix44f& other) const
{
	return m[0][0] != other.m[0][0] ||
		m[0][1] != other.m[0][1] ||
		m[0][2] != other.m[0][2] ||
		m[0][3] != other.m[0][3] ||
		m[1][0] != other.m[1][0] ||
		m[1][1] != other.m[1][1] ||
		m[1][2] != other.m[1][2] ||
		m[1][3] != other.m[1][3] ||
		m[2][0] != other.m[2][0] ||
		m[2][1] != other.m[2][1] ||
		m[2][2] != other.m[2][2] ||
		m[2][3] != other.m[2][3] ||
		m[3][0] != other.m[3][0] ||
		m[3][1] != other.m[3][1] ||
		m[3][2] != other.m[3][2] ||
		m[3][3] != other.m[3][3];
}

inline matrix44f& matrix44f::operator=( const matrix44f& matrix )
{
	m[0][0] = matrix.m[0][0]; m[0][1] = matrix.m[0][1]; m[0][2] = matrix.m[0][2]; m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0]; m[1][1] = matrix.m[1][1]; m[1][2] = matrix.m[1][2]; m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0]; m[2][1] = matrix.m[2][1]; m[2][2] = matrix.m[2][2]; m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0]; m[3][1] = matrix.m[3][1]; m[3][2] = matrix.m[3][2]; m[3][3] = matrix.m[3][3];

	flagBits = General;

	return *this;

}

inline vec3f matrix44f::map(const vec3f& point) const
{
	return *this * point;
}

inline vec3f matrix44f::mapVector(const vec3f& vector) const
{
	if (flagBits == Identity || flagBits == Translation) {
		return vector;
	} else if (flagBits == Scale || flagBits == (Translation | Scale)) {
		return vec3f(vector.x() * m[0][0],
			vector.y() * m[1][1],
			vector.z() * m[2][2]);
	} else {
		return vec3f(vector.x() * m[0][0] +
			vector.y() * m[1][0] +
			vector.z() * m[2][0],
			vector.x() * m[0][1] +
			vector.y() * m[1][1] +
			vector.z() * m[2][1],
			vector.x() * m[0][2] +
			vector.y() * m[1][2] +
			vector.z() * m[2][2]);
	}
}

inline vec4f matrix44f::map(const vec4f& point) const
{
	return *this * point;
}

inline float *matrix44f::data()
{
	// We have to assume that the caller will modify the matrix elements,
	// so we flip it over to "General" mode.
	flagBits = General;
	return m[0];
}

inline QString matrix44f::toUtf8() const 
{		
	return QString("[%1,%2,%3 %4],[%5,%6,%7,%8], [%9,%10,%11,%12],[%13,%14,%15,%16]\n").arg(m[0][0],6,'f',6).arg(m[0][1],6,'f',6).arg(m[0][2],6,'f',6).arg(m[0][3],6,'f',6).
		arg(m[1][0],6,'f',6).arg(m[1][1],6,'f',6).arg(m[1][2],6,'f',6).arg(m[1][3],6,'f',6).
		arg(m[2][0],6,'f',6).arg(m[2][1],6,'f',6).arg(m[2][2],6,'f',6).arg(m[2][3],6,'f',6).
		arg(m[3][0],6,'f',6).arg(m[3][1],6,'f',6).arg(m[3][2],6,'f',6).arg(m[3][3],6,'f',6);
}