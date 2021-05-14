inline double matrix33d::m11() const { return m[0][0]; }
inline double matrix33d::m12() const { return  m[0][1]; }
inline double matrix33d::m13() const { return m[0][2]; }
inline double matrix33d::m21() const { return  m[1][0]; }
inline double matrix33d::m22() const { return  m[1][1]; }
inline double matrix33d::m23() const { return m[1][2]; }
inline double matrix33d::m31() const { return  m[2][0]; }
inline double matrix33d::m32() const { return  m[2][1]; }
inline double matrix33d::m33() const { return m[2][2]; }

inline double& matrix33d::m11() { return m[0][0]; }
inline double& matrix33d::m12() { return m[0][1]; }
inline double& matrix33d::m13() { return m[0][2]; }
inline double& matrix33d::m21() { return m[1][0]; }
inline double& matrix33d::m22() { return m[1][1]; }
inline double& matrix33d::m23() { return m[1][2]; }
inline double& matrix33d::m31() { return m[2][0]; }
inline double& matrix33d::m32() { return m[2][1]; }
inline double& matrix33d::m33() { return m[2][2]; }

inline matrix33d::matrix33d (double m11, double m12, double m13, 
							 double m21, double m22, double m23, 
							 double m31, double m32, double m33)
{
	m[0][0] = m11; m[0][1] = m21; m[0][2] = m31;
	m[1][0] = m12; m[1][1] = m22; m[1][2] = m32;
	m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; 
}

inline void matrix33d::setM11(double m11) { m[0][0] = m11; }
inline void matrix33d::setM12(double m12) { m[0][1] = m12; }
inline void matrix33d::setM13(double m13) { m[0][2] = m13; }
inline void matrix33d::setM21(double m21) { m[1][0] = m21; }
inline void matrix33d::setM22(double m22) { m[1][1] = m22; }
inline void matrix33d::setM23(double m23) { m[1][2] = m23; }
inline void matrix33d::setM31(double m31) { m[2][0] = m31; }
inline void matrix33d::setM32(double m32) { m[2][1] = m32; }
inline void matrix33d::setM33(double m33) { m[2][2] = m33; }

inline void matrix33d::set (double m11, double m12, double m13,
                       double m21, double m22, double m23,
                       double m31, double m32, double m33)
{
    m[0][0] = m11; m[0][1] = m21; m[0][2] = m31;
    m[1][0] = m12; m[1][1] = m22; m[1][2] = m32;
    m[2][0] = m13; m[2][1] = m23; m[2][2] = m33;
}



inline matrix33d::matrix33d( const matrix33d& matrix )
{
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			m[i][j] = matrix.m[i][j];
}

inline void matrix33d::set (const matrix33d& matrix )
{
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            m[i][j] = matrix.m[i][j];
}


inline const double& matrix33d::operator()(int aRow, int aColumn) const
{
	Q_ASSERT(aRow >= 0 && aRow < 3 && aColumn >= 0 && aColumn < 3);
	return m[aColumn][aRow];
}

inline double& matrix33d::operator()(int aRow, int aColumn)
{
	Q_ASSERT(aRow >= 0 && aRow < 3 && aColumn >= 0 && aColumn < 3);
	return m[aColumn][aRow];
}

inline bool matrix33d::isIdentity() const
{
	if (m[0][0] != 1.0 || m[0][1] != 0.0 || m[0][2] != 0.0)
		return false;
	if (m[1][0] != 0.0 || m[1][1] != 1.0 || m[1][2] != 0.0)
		return false;
	if (m[2][0] != 0.0 || m[2][1] != 0.0 || m[2][2] != 1.0)
		return false;
	return true;
}

inline void matrix33d::setToIdentity()
{
	m[0][0] = 1.0;
	m[0][1] = 0.0;
	m[0][2] = 0.0;

	m[1][0] = 0.0;
	m[1][1] = 1.0;
	m[1][2] = 0.0;

	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 1.0;
}

inline bool matrix33d::isZero() const
{
	for(uint i=0; i < 3; i++)
		for(uint j=0; j < 3; j++)
			if(!DIsZero(m[i][j]))
				return false;

	return true;
}

inline void matrix33d::setToZero()
{
	m[0][0] = 0.0;
	m[0][1] = 0.0;
	m[0][2] = 0.0;

	m[1][0] = 0.0;
	m[1][1] = 0.0;
	m[1][2] = 0.0;

	m[2][0] = 0.0;
	m[2][1] = 0.0;
	m[2][2] = 0.0;
}

inline void matrix33d::fill(double value)
{
	m[0][0] = value;
	m[0][1] = value;
	m[0][2] = value;

	m[1][0] = value;
	m[1][1] = value;
	m[1][2] = value;

	m[2][0] = value;
	m[2][1] = value;
	m[2][2] = value;
}

inline matrix33d& matrix33d::operator+=(const matrix33d& other)
{
	m[0][0] += other.m[0][0];
	m[0][1] += other.m[0][1];
	m[0][2] += other.m[0][2];

	m[1][0] += other.m[1][0];
	m[1][1] += other.m[1][1];
	m[1][2] += other.m[1][2];

	m[2][0] += other.m[2][0];
	m[2][1] += other.m[2][1];
	m[2][2] += other.m[2][2];

	return *this;
}

inline matrix33d& matrix33d::operator-=(const matrix33d& other)
{
	m[0][0] -= other.m[0][0];
	m[0][1] -= other.m[0][1];
	m[0][2] -= other.m[0][2];

	m[1][0] -= other.m[1][0];
	m[1][1] -= other.m[1][1];
	m[1][2] -= other.m[1][2];

	m[2][0] -= other.m[2][0];
	m[2][1] -= other.m[2][1];
	m[2][2] -= other.m[2][2];

	return *this;
}

inline matrix33d& matrix33d::operator*=(const matrix33d& other)
{
	*this = *this * other;
	return *this;
}

inline matrix33d& matrix33d::operator*=(double factor)
{
	m[0][0] *= factor;
	m[0][1] *= factor;
	m[0][2] *= factor;

	m[1][0] *= factor;
	m[1][1] *= factor;
	m[1][2] *= factor;

	m[2][0] *= factor;
	m[2][1] *= factor;
	m[2][2] *= factor;

	return *this;
}

inline bool matrix33d::operator==(const matrix33d& other) const
{
	return m[0][0] == other.m[0][0] &&
		m[0][1] == other.m[0][1] &&
		m[0][2] == other.m[0][2] &&
		m[1][0] == other.m[1][0] &&
		m[1][1] == other.m[1][1] &&
		m[1][2] == other.m[1][2] &&
		m[2][0] == other.m[2][0] &&
		m[2][1] == other.m[2][1] &&
		m[2][2] == other.m[2][2] ;

}

inline bool matrix33d::operator!=(const matrix33d& other) const
{
	return m[0][0] != other.m[0][0] ||
		m[0][1] != other.m[0][1] ||
		m[0][2] != other.m[0][2] ||
		m[1][0] != other.m[1][0] ||
		m[1][1] != other.m[1][1] ||
		m[1][2] != other.m[1][2] ||
		m[2][0] != other.m[2][0] ||
		m[2][1] != other.m[2][1] ||
		m[2][2] != other.m[2][2] ;
}

inline double *matrix33d::data()
{

    return m[0];
}
