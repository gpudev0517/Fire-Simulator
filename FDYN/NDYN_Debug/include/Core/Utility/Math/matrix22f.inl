inline matrix22f::matrix22f (float m11, float m12,
                             float m21, float m22)
{
        m[0][0] = m11; m[0][1] = m21;
        m[1][0] = m12; m[1][1] = m22;
}

inline matrix22f::matrix22f( const matrix22f& matrix )
{
        for(int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
			m[i][j] = matrix.m[i][j];
}

inline const float& matrix22f::operator()(int aRow, int aColumn) const
{
        Q_ASSERT(aRow >= 0 && aRow < 2 && aColumn >= 0 && aColumn < 2);
	return m[aColumn][aRow];
}

inline float& matrix22f::operator()(int aRow, int aColumn)
{
        Q_ASSERT(aRow >= 0 && aRow < 2 && aColumn >= 0 && aColumn < 2);
	return m[aColumn][aRow];
}

inline bool matrix22f::isIdentity() const
{
        if (m[0][0] != 1.0f || m[0][1] != 0.0f)
		return false;
        if (m[1][0] != 0.0f || m[1][1] != 1.0f)
		return false;
	return true;
}

inline void matrix22f::setToIdentity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
}

inline bool matrix22f::isZero() const
{
        for(uint i=0; i < 2; i++)
                for(uint j=0; j < 2; j++)
			if(!FIsZero(m[i][j]))
				return false;

	return true;
}

inline void matrix22f::setToZero()
{
	m[0][0] = 0.0f;
	m[0][1] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 0.0f;
}

inline void matrix22f::fill(float value)
{
	m[0][0] = value;
	m[0][1] = value;

	m[1][0] = value;
	m[1][1] = value;
}

inline matrix22f& matrix22f::operator+=(const matrix22f& other)
{
	m[0][0] += other.m[0][0];
	m[0][1] += other.m[0][1];

	m[1][0] += other.m[1][0];
	m[1][1] += other.m[1][1];

	return *this;
}

inline matrix22f& matrix22f::operator-=(const matrix22f& other)
{
	m[0][0] -= other.m[0][0];
	m[0][1] -= other.m[0][1];

	m[1][0] -= other.m[1][0];
	m[1][1] -= other.m[1][1];

	return *this;
}

inline matrix22f& matrix22f::operator*=(const matrix22f& other)
{

	*this = *this * other;
	return *this;
}

inline matrix22f& matrix22f::operator*=(float factor)
{
	m[0][0] *= factor;
	m[0][1] *= factor;

	m[1][0] *= factor;
	m[1][1] *= factor;

	return *this;
}

inline bool matrix22f::operator==(const matrix22f& other) const
{
	return m[0][0] == other.m[0][0] &&
		m[0][1] == other.m[0][1] &&
		m[1][0] == other.m[1][0] &&
                m[1][1] == other.m[1][1];

}

inline bool matrix22f::operator!=(const matrix22f& other) const
{
	return m[0][0] != other.m[0][0] ||
		m[0][1] != other.m[0][1] ||
		m[1][0] != other.m[1][0] ||
                m[1][1] != other.m[1][1];
}

inline matrix22f& matrix22f::operator=( const matrix22f& A )
{
        for(int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
			m[i][j] = A.m[i][j];

	return *this;
}
