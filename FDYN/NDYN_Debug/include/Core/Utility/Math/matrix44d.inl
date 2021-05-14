inline matrix44d::matrix44d(double m11, double m12, double m13, double m14,
							double m21, double m22, double m23, double m24,
							double m31, double m32, double m33, double m34,
							double m41, double m42, double m43, double m44)
{
	m[0][0] = m11; m[0][1] = m21; m[0][2] = m31; m[0][3] = m41;
	m[1][0] = m12; m[1][1] = m22; m[1][2] = m32; m[1][3] = m42;
	m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; m[2][3] = m43;
	m[3][0] = m14; m[3][1] = m24; m[3][2] = m34; m[3][3] = m44;
	flagBits = General;
}

inline matrix44d::matrix44d( const matrix44d& matrix )
{
	m[0][0] = matrix.m[0][0]; m[0][1] = matrix.m[0][1]; m[0][2] = matrix.m[0][2]; m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0]; m[1][1] = matrix.m[1][1]; m[1][2] = matrix.m[1][2]; m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0]; m[2][1] = matrix.m[2][1]; m[2][2] = matrix.m[2][2]; m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0]; m[3][1] = matrix.m[3][1]; m[3][2] = matrix.m[3][2]; m[3][3] = matrix.m[3][3];

	flagBits = General;
}

inline const double& matrix44d::operator()(int aRow, int aColumn) const
{
	Q_ASSERT(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
	return m[aColumn][aRow];
}

inline double& matrix44d::operator()(int aRow, int aColumn)
{
	Q_ASSERT(aRow >= 0 && aRow < 4 && aColumn >= 0 && aColumn < 4);
	flagBits = General;
	return m[aColumn][aRow];
}

inline bool matrix44d::isIdentity() const
{
	if (flagBits == Identity)
		return true;
	if (m[0][0] != 1.0 || m[0][1] != 0.0 || m[0][2] != 0.0)
		return false;
	if (m[0][3] != 0.0 || m[1][0] != 0.0 || m[1][1] != 1.0)
		return false;
	if (m[1][2] != 0.0 || m[1][3] != 0.0 || m[2][0] != 0.0)
		return false;
	if (m[2][1] != 0.0 || m[2][2] != 1.0 || m[2][3] != 0.0)
		return false;
	if (m[3][0] != 0.0 || m[3][1] != 0.0 || m[3][2] != 0.0)
		return false;
	return (m[3][3] == 1.0);
}

inline void matrix44d::setToIdentity()
{
	m[0][0] = 1.0;
	m[0][1] = 0.0;
	m[0][2] = 0.0;
	m[0][3] = 0.0;
	m[1][0] = 0.0;
	m[1][1] = 1.0;
	m[1][2] = 0.0;
	m[1][3] = 0.0;
	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 1.0;
	m[2][3] = 0.0;
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 1.0;
	flagBits = Identity;
}

inline bool matrix44d::isZero() const
{
	for(uint i=0; i < 4; i++)
		for(uint j=0; j < 4; j++)
			if(!DIsZero(m[i][j]))
				return false;

	return true;
}

inline void matrix44d::setToZero()
{
	m[0][0] = 0.0;
	m[0][1] = 0.0;
	m[0][2] = 0.0;
	m[0][3] = 0.0;
	m[1][0] = 0.0;
	m[1][1] = 0.0;
	m[1][2] = 0.0;
	m[1][3] = 0.0;
	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 0.0;
	m[2][3] = 0.0;
	m[3][0] = 0.0;
	m[3][1] = 0.0;
	m[3][2] = 0.0;
	m[3][3] = 0.0;
	flagBits = General;
}

inline void matrix44d::fill(double value)
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

inline matrix44d& matrix44d::operator+=(const matrix44d& other)
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

inline matrix44d& matrix44d::operator-=(const matrix44d& other)
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

inline matrix44d& matrix44d::operator*=(const matrix44d& other)
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

inline matrix44d& matrix44d::operator*=(double factor)
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

inline bool matrix44d::operator==(const matrix44d& other) const
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

inline bool matrix44d::operator!=(const matrix44d& other) const
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

inline matrix44d& matrix44d::operator=( const matrix44d& matrix )
{
	m[0][0] = matrix.m[0][0]; m[0][1] = matrix.m[0][1]; m[0][2] = matrix.m[0][2]; m[0][3] = matrix.m[0][3];
	m[1][0] = matrix.m[1][0]; m[1][1] = matrix.m[1][1]; m[1][2] = matrix.m[1][2]; m[1][3] = matrix.m[1][3];
	m[2][0] = matrix.m[2][0]; m[2][1] = matrix.m[2][1]; m[2][2] = matrix.m[2][2]; m[2][3] = matrix.m[2][3];
	m[3][0] = matrix.m[3][0]; m[3][1] = matrix.m[3][1]; m[3][2] = matrix.m[3][2]; m[3][3] = matrix.m[3][3];

	flagBits = General;

	return *this;
}

inline vec3d matrix44d::map(const vec3d& point) const
{
	return *this * point;
}

inline vec3d matrix44d::mapVector(const vec3d& vector) const
{
	if (flagBits == Identity || flagBits == Translation) {
		return vector;
	} else if (flagBits == Scale || flagBits == (Translation | Scale)) {
		return vec3d(vector.x() * m[0][0],
			vector.y() * m[1][1],
			vector.z() * m[2][2]);
	} else {
		return vec3d(vector.x() * m[0][0] +
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

inline vec4d matrix44d::map(const vec4d& point) const
{
	return *this * point;
}

inline double *matrix44d::data()
{
	// We have to assume that the caller will modify the matrix elements,
	// so we flip it over to "General" mode.
	flagBits = General;
	return m[0];
}