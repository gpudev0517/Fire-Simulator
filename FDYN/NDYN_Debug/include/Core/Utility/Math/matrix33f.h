#pragma once

class QVariant;
class vec3f;
class matrix33d;

class matrix33f
{
public:
	inline matrix33f() { setToIdentity(); }
    NEFEXP_CORE explicit matrix33f(const float *values);
	inline matrix33f(float m11, float m12, float m13, 
				     float m21, float m22, float m23, 
					 float m31, float m32, float m33);
    NEFEXP_CORE matrix33f(const float *values, int cols, int rows);
	inline matrix33f(const matrix33f& matrix);
    NEFEXP_CORE matrix33f(const matrix33d& matrix);

    static inline matrix33f tensorProduct(const vec3f& vec1, const vec3f& vec2)
    {
        return matrix33f(vec1.x()*vec2.x(), vec1.x()*vec2.y(), vec1.x()*vec2.z(),
                         vec1.y()*vec2.x(), vec1.y()*vec2.y(), vec1.y()*vec2.z(),
                         vec1.z()*vec2.x(), vec1.z()*vec2.y(), vec1.z()*vec2.z());
    }

	inline const float& operator()(int row, int column) const;
	inline float& operator()(int row, int column);

    NEFEXP_CORE vec3f column(int index) const;
    NEFEXP_CORE void setColumn(int index, const vec3f& value);

    NEFEXP_CORE vec3f row(int index) const;
    NEFEXP_CORE void setRow(int index, const vec3f& value);

	inline bool isIdentity() const;
	inline void setToIdentity();

	inline bool isZero() const;
	inline void setToZero();

	inline void fill(float value);

    NEFEXP_CORE matrix33f transposed() const;
    NEFEXP_CORE matrix33f adjoint() const;
    NEFEXP_CORE matrix33f power(int i) const;
    NEFEXP_CORE float determinant() const;
    NEFEXP_CORE float trace() const;
    NEFEXP_CORE vec3f diagonal() const;

    NEFEXP_CORE matrix33f inverse(bool *invertible = 0) const;
    NEFEXP_CORE void invert(bool *invertible = 0);

	inline matrix33f& operator+=(const matrix33f& other);
	inline matrix33f& operator-=(const matrix33f& other);
	inline matrix33f& operator*=(const matrix33f& other);
	inline matrix33f& operator*=(float factor);
    NEFEXP_CORE matrix33f& operator/=(float divisor);
	inline bool operator==(const matrix33f& other) const;
	inline bool operator!=(const matrix33f& other) const;

	inline matrix33f& operator=(const matrix33f& A);
    NEFEXP_CORE matrix33f& operator=(const matrix33d& A);

    NEFEXP_CORE friend matrix33f operator+(const matrix33f& m1, const matrix33f& m2);
    NEFEXP_CORE friend matrix33f operator-(const matrix33f& m1, const matrix33f& m2);
    NEFEXP_CORE friend matrix33f operator*(const matrix33f& m1, const matrix33f& m2);
    NEFEXP_CORE friend vec3f operator*(const matrix33f& matrix, const vec3f& vector);
    NEFEXP_CORE friend vec3f operator*(const vec3f& vector, const matrix33f& matrix);
    NEFEXP_CORE friend matrix33f operator-(const matrix33f& matrix);
    NEFEXP_CORE friend matrix33f operator*(float factor, const matrix33f& matrix);
    NEFEXP_CORE friend matrix33f operator*(const matrix33f& matrix, float factor);
    friend NEFEXP_CORE matrix33f operator/(const matrix33f& matrix, float divisor);

	friend inline bool qFuzzyCompare(const matrix33f& m1, const matrix33f& m2);

    NEFEXP_CORE void rotate(float angle, const vec3f& vector);
    NEFEXP_CORE void rotate(float angle, float x, float y, float z = 0.0f);

    NEFEXP_CORE void computeMaxEigenValue(float& valMax);
    //NEFEXP_CORE void SVDCovarianceMatrix(matrix33f& rotMatrix, matrix33f& diagMatrix);
    //MI added
    inline void eigenDecomposition(matrix33f& R);

    NEFEXP_CORE void eigenDecomposition2(matrix33f& eigenVectors, vec3f& eigenvalues);

	//MI added
	inline void jacobiRotate(matrix33f& R, int p, int q);
	inline matrix33f& dyad_product(const vec3f &a,const vec3f &b);
	//Computes the entrywise norm of the matrix
	inline float norm(float p=2.f) const;

    NEFEXP_CORE void copyDataTo(float *values) const;

    NEFEXP_CORE vec3f mapVector(const vec3f& vector) const;

	inline float *data();
	inline const float *data() const { return m[0]; }
	inline const float *constData() const { return m[0]; }

    NEFEXP_CORE operator QVariant() const;
    NEFEXP_CORE QString toUtf8() const;

private:
	float m[3][3];          // Column-major order to match OpenGL.

    friend inline QDebug operator<<(QDebug dbg, const matrix33f &vector);
    friend inline QDataStream &operator<<(QDataStream &, const matrix33f &);
    friend inline QDataStream &operator>>(QDataStream &, matrix33f &);

    NEFEXP_CORE float hypot2(float x, float y);
    NEFEXP_CORE void tred2(matrix33f& v, vec3f& d, vec3f& e);
    NEFEXP_CORE void tql2(matrix33f& v, vec3f& d, vec3f& e);
};

Q_DECLARE_TYPEINFO(matrix33f, Q_MOVABLE_TYPE);

NEFEXP_CORE matrix33f operator/(const matrix33f& matrix, float divisor);

inline matrix33f operator+(const matrix33f& m1, const matrix33f& m2)
{
	matrix33f m;
	m.m[0][0] = m1.m[0][0] + m2.m[0][0];
	m.m[0][1] = m1.m[0][1] + m2.m[0][1];
	m.m[0][2] = m1.m[0][2] + m2.m[0][2];
	
	m.m[1][0] = m1.m[1][0] + m2.m[1][0];
	m.m[1][1] = m1.m[1][1] + m2.m[1][1];
	m.m[1][2] = m1.m[1][2] + m2.m[1][2];
	
	m.m[2][0] = m1.m[2][0] + m2.m[2][0];
	m.m[2][1] = m1.m[2][1] + m2.m[2][1];
	m.m[2][2] = m1.m[2][2] + m2.m[2][2];
	
	return m;
}

inline matrix33f operator-(const matrix33f& m1, const matrix33f& m2)
{
	matrix33f m;
	m.m[0][0] = m1.m[0][0] - m2.m[0][0];
	m.m[0][1] = m1.m[0][1] - m2.m[0][1];
	m.m[0][2] = m1.m[0][2] - m2.m[0][2];
	
	m.m[1][0] = m1.m[1][0] - m2.m[1][0];
	m.m[1][1] = m1.m[1][1] - m2.m[1][1];
	m.m[1][2] = m1.m[1][2] - m2.m[1][2];

	m.m[2][0] = m1.m[2][0] - m2.m[2][0];
	m.m[2][1] = m1.m[2][1] - m2.m[2][1];
	m.m[2][2] = m1.m[2][2] - m2.m[2][2];
	
	return m;
}

inline matrix33f operator*(const matrix33f& m1, const matrix33f& m2)
{
	
	matrix33f m;
	m.m[0][0] = m1.m[0][0] * m2.m[0][0] +
		m1.m[1][0] * m2.m[0][1] +
		m1.m[2][0] * m2.m[0][2];
	m.m[0][1] = m1.m[0][1] * m2.m[0][0] +
		m1.m[1][1] * m2.m[0][1] +
		m1.m[2][1] * m2.m[0][2];
	m.m[0][2] = m1.m[0][2] * m2.m[0][0] +
		m1.m[1][2] * m2.m[0][1] +
		m1.m[2][2] * m2.m[0][2];
	
	m.m[1][0] = m1.m[0][0] * m2.m[1][0] +
		m1.m[1][0] * m2.m[1][1] +
		m1.m[2][0] * m2.m[1][2];
	m.m[1][1] = m1.m[0][1] * m2.m[1][0] +
		m1.m[1][1] * m2.m[1][1] +
		m1.m[2][1] * m2.m[1][2];
	m.m[1][2] = m1.m[0][2] * m2.m[1][0] +
		m1.m[1][2] * m2.m[1][1] +
		m1.m[2][2] * m2.m[1][2];
	
	m.m[2][0] = m1.m[0][0] * m2.m[2][0] +
		m1.m[1][0] * m2.m[2][1] +
		m1.m[2][0] * m2.m[2][2];
	m.m[2][1] = m1.m[0][1] * m2.m[2][0] +
		m1.m[1][1] * m2.m[2][1] +
		m1.m[2][1] * m2.m[2][2];
	m.m[2][2] = m1.m[0][2] * m2.m[2][0] +
		m1.m[1][2] * m2.m[2][1] +
		m1.m[2][2] * m2.m[2][2];
	
	return m;
}


inline matrix33f operator-(const matrix33f& matrix)
{
	matrix33f m;
	m.m[0][0] = -matrix.m[0][0];
	m.m[0][1] = -matrix.m[0][1];
	m.m[0][2] = -matrix.m[0][2];
	
	m.m[1][0] = -matrix.m[1][0];
	m.m[1][1] = -matrix.m[1][1];
	m.m[1][2] = -matrix.m[1][2];
	
	m.m[2][0] = -matrix.m[2][0];
	m.m[2][1] = -matrix.m[2][1];
	m.m[2][2] = -matrix.m[2][2];
	
	return m;
}

inline matrix33f operator*(float factor, const matrix33f& matrix)
{
	matrix33f m;
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	m.m[0][2] = matrix.m[0][2] * factor;
	
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;
	m.m[1][2] = matrix.m[1][2] * factor;
	
	m.m[2][0] = matrix.m[2][0] * factor;
	m.m[2][1] = matrix.m[2][1] * factor;
	m.m[2][2] = matrix.m[2][2] * factor;
	
	return m;
}

inline matrix33f operator*(const matrix33f& matrix, float factor)
{
	matrix33f m;
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	m.m[0][2] = matrix.m[0][2] * factor;
	
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;
	m.m[1][2] = matrix.m[1][2] * factor;

	m.m[2][0] = matrix.m[2][0] * factor;
	m.m[2][1] = matrix.m[2][1] * factor;
	m.m[2][2] = matrix.m[2][2] * factor;

	return m;
}

inline bool qFuzzyCompare(const matrix33f& m1, const matrix33f& m2)
{
	return qFuzzyCompare(m1.m[0][0], m2.m[0][0]) &&
		qFuzzyCompare(m1.m[0][1], m2.m[0][1]) &&
		qFuzzyCompare(m1.m[0][2], m2.m[0][2]) &&
		qFuzzyCompare(m1.m[1][0], m2.m[1][0]) &&
		qFuzzyCompare(m1.m[1][1], m2.m[1][1]) &&
		qFuzzyCompare(m1.m[1][2], m2.m[1][2]) &&
		qFuzzyCompare(m1.m[2][0], m2.m[2][0]) &&
		qFuzzyCompare(m1.m[2][1], m2.m[2][1]) &&
		qFuzzyCompare(m1.m[2][2], m2.m[2][2]);
}

QDebug operator<<(QDebug dbg, const matrix33f &matrix)
{
    dbg.nospace() << "matrix33f(" << matrix.m[0][0] << ", " << matrix.m[1][0] << ", " << matrix.m[2][0] << endl
                                  << matrix.m[0][1] << ", " << matrix.m[1][1] << ", " << matrix.m[2][2] << endl
                                  << matrix.m[0][2] << ", " << matrix.m[1][2] << ", " << matrix.m[2][2] <<  ')';

    return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const matrix33f &matrix)
{
    stream << float(matrix.m[0][0]) << float(matrix.m[0][1]) << float(matrix.m[0][2])
           << float(matrix.m[1][0]) << float(matrix.m[1][1]) << float(matrix.m[1][2])
           << float(matrix.m[2][0]) << float(matrix.m[2][1]) << float(matrix.m[2][2]);
    return stream;
}

QDataStream& operator>>(QDataStream& stream, matrix33f &matrix)
{
    stream >> matrix.m[0][0] >> matrix.m[0][1] >> matrix.m[0][2]
           >> matrix.m[1][0] >> matrix.m[1][1] >> matrix.m[1][2]
           >> matrix.m[2][0] >> matrix.m[2][1] >> matrix.m[2][2];

    return stream;
}

#include "Utility/Math/matrix33f.inl"




