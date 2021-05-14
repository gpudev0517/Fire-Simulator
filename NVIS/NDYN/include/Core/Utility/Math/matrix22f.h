#pragma once

class QVariant;
class vec2f;

class matrix22f
{
public:
    inline matrix22f() { setToIdentity(); }
    NEFEXP_CORE explicit matrix22f(const float *values);
    inline matrix22f(float m11, float m12,
                     float m21, float m22);
    NEFEXP_CORE matrix22f(const float *values, int cols, int rows);
    inline matrix22f(const matrix22f& matrix);

    static inline matrix22f tensorProduct(const vec2f& vec1, const vec2f& vec2)
    {
        return matrix22f(vec1.x()*vec2.x(),vec1.x()*vec2.y(),
                         vec1.y()*vec2.x(),vec1.y()*vec2.y());
    }

	inline const float& operator()(int row, int column) const;
	inline float& operator()(int row, int column);

    NEFEXP_CORE vec2f column(int index) const;
    NEFEXP_CORE void setColumn(int index, const vec2f& value);

    NEFEXP_CORE vec2f row(int index) const;
    NEFEXP_CORE void setRow(int index, const vec2f& value);

	inline bool isIdentity() const;
	inline void setToIdentity();

	inline bool isZero() const;
	inline void setToZero();

	inline void fill(float value);

    NEFEXP_CORE matrix22f transposed() const;
    NEFEXP_CORE matrix22f adjoint() const;
    NEFEXP_CORE matrix22f power(int i) const;
    NEFEXP_CORE float determinant() const;
    NEFEXP_CORE float trace() const;
    NEFEXP_CORE vec2f diagonal() const;

    NEFEXP_CORE matrix22f inverse() const;
    NEFEXP_CORE void invert();

    inline matrix22f& operator+=(const matrix22f& other);
    inline matrix22f& operator-=(const matrix22f& other);
    inline matrix22f& operator*=(const matrix22f& other);
    inline matrix22f& operator*=(float factor);
    NEFEXP_CORE matrix22f& operator/=(float divisor);
    inline bool operator==(const matrix22f& other) const;
    inline bool operator!=(const matrix22f& other) const;

    inline matrix22f& operator=(const matrix22f& A);

    NEFEXP_CORE friend matrix22f operator+(const matrix22f& m1, const matrix22f& m2);
    NEFEXP_CORE friend matrix22f operator-(const matrix22f& m1, const matrix22f& m2);
    NEFEXP_CORE friend matrix22f operator*(const matrix22f& m1, const matrix22f& m2);
    NEFEXP_CORE friend vec2f operator*(const matrix22f& matrix, const vec2f& vector);
    NEFEXP_CORE friend vec2f operator*(const vec2f& vector, const matrix22f& matrix);
    NEFEXP_CORE friend matrix22f operator-(const matrix22f& matrix);
    NEFEXP_CORE friend matrix22f operator*(float factor, const matrix22f& matrix);
    NEFEXP_CORE friend matrix22f operator*(const matrix22f& matrix, float factor);
    friend NEFEXP_CORE matrix22f operator/(const matrix22f& matrix, float divisor);

private:
    float m[2][2];          // Column-major order to match OpenGL.

    friend inline QDebug operator<<(QDebug dbg, const matrix22f &vector);
    friend inline QDataStream &operator<<(QDataStream &, const matrix22f &);
    friend inline QDataStream &operator>>(QDataStream &, matrix22f &);
};

Q_DECLARE_TYPEINFO(matrix22f, Q_MOVABLE_TYPE);

NEFEXP_CORE matrix22f operator/(const matrix22f& matrix, float divisor);

inline matrix22f operator+(const matrix22f& m1, const matrix22f& m2)
{
    matrix22f m;
	m.m[0][0] = m1.m[0][0] + m2.m[0][0];
	m.m[0][1] = m1.m[0][1] + m2.m[0][1];
	
	m.m[1][0] = m1.m[1][0] + m2.m[1][0];
	m.m[1][1] = m1.m[1][1] + m2.m[1][1];
	
	return m;
}

inline matrix22f operator-(const matrix22f& m1, const matrix22f& m2)
{
    matrix22f m;
	m.m[0][0] = m1.m[0][0] - m2.m[0][0];
	m.m[0][1] = m1.m[0][1] - m2.m[0][1];
	
	m.m[1][0] = m1.m[1][0] - m2.m[1][0];
	m.m[1][1] = m1.m[1][1] - m2.m[1][1];

	return m;
}

inline matrix22f operator*(const matrix22f& m1, const matrix22f& m2)
{
	
    matrix22f m;
	m.m[0][0] = m1.m[0][0] * m2.m[0][0] +
        m1.m[1][0] * m2.m[0][1];
	m.m[0][1] = m1.m[0][1] * m2.m[0][0] +
        m1.m[1][1] * m2.m[0][1];
	
	m.m[1][0] = m1.m[0][0] * m2.m[1][0] +
        m1.m[1][0] * m2.m[1][1];
	m.m[1][1] = m1.m[0][1] * m2.m[1][0] +
        m1.m[1][1] * m2.m[1][1];
//	m.m[1][2] = m1.m[0][2] * m2.m[1][0] +
//        m1.m[1][2] * m2.m[1][1];
	
	return m;
}


inline matrix22f operator-(const matrix22f& matrix)
{
    matrix22f m;
	m.m[0][0] = -matrix.m[0][0];
	m.m[0][1] = -matrix.m[0][1];
	
	m.m[1][0] = -matrix.m[1][0];
	m.m[1][1] = -matrix.m[1][1];
	
	return m;
}

inline matrix22f operator*(float factor, const matrix22f& matrix)
{
    matrix22f m;
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;
	
	return m;
}

inline matrix22f operator*(const matrix22f& matrix, float factor)
{
    matrix22f m;
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;

	return m;
}

QDebug operator<<(QDebug dbg, const matrix22f &matrix)
{
    dbg.nospace() << "matrix22f(" << matrix.m[0][0] << ", " << matrix.m[1][0] << endl
                                  << matrix.m[0][1] << ", " << matrix.m[1][1] << ')';

    return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const matrix22f &matrix)
{
    stream << float(matrix.m[0][0]) << float(matrix.m[0][1])
           << float(matrix.m[1][0]) << float(matrix.m[1][1]);
    return stream;
}

QDataStream& operator>>(QDataStream& stream, matrix22f &matrix)
{
    stream >> matrix.m[0][0] >> matrix.m[0][1]
           >> matrix.m[1][0] >> matrix.m[1][1];

    return stream;
}

#include "Utility/Math/matrix22f.inl"




