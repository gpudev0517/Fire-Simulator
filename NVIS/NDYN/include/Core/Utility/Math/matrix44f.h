#pragma once

class QVariant;
class vec3f;
class vec4f;
class matrix33f;
class matrix44d;
class quatf;
struct AABB;

class matrix44f
{
public:
	inline matrix44f() { setToIdentity(); }
    NEFEXP_CORE explicit matrix44f(const float *values);
	inline matrix44f(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
    NEFEXP_CORE matrix44f(const float *values, int cols, int rows);
    NEFEXP_CORE matrix44f(const matrix33f& matrix);
	inline matrix44f(const matrix44f& matrix);
    NEFEXP_CORE matrix44f(const matrix44d& matrix);

    static inline matrix44f tensorProduct(const vec4f& vec1, const vec4f& vec2)
    {
        return matrix44f(vec1.x()*vec2.x(), vec1.x()*vec2.y(), vec1.x()*vec2.z(), vec1.x()*vec2.w(),
                         vec1.y()*vec2.x(), vec1.y()*vec2.y(), vec1.y()*vec2.z(), vec1.y()*vec2.w(),
                         vec1.z()*vec2.x(), vec1.z()*vec2.y(), vec1.z()*vec2.z(), vec1.z()*vec2.w(),
                         vec1.w()*vec2.x(), vec1.w()*vec2.y(), vec1.w()*vec2.z(), vec1.w()*vec2.w());
    }

	inline const float& operator()(int row, int column) const;
	inline float& operator()(int row, int column);

    NEFEXP_CORE vec4f column(int index) const;
    NEFEXP_CORE void setColumn(int index, const vec4f& value);

    NEFEXP_CORE vec4f row(int index) const;
    NEFEXP_CORE void setRow(int index, const vec4f& value);

	inline bool isIdentity() const;
	inline void setToIdentity();

	inline bool isZero() const;
	inline void setToZero();

	inline void fill(float value);

    NEFEXP_CORE float determinant() const;
    NEFEXP_CORE matrix44f inverted(bool *invertible = 0) const;
    NEFEXP_CORE void invert(bool *invertible = 0);
    NEFEXP_CORE matrix44f transposed() const;
    NEFEXP_CORE matrix33f normalMatrix() const;

    NEFEXP_CORE float trace() const;
    NEFEXP_CORE vec4f diagonal() const;
    NEFEXP_CORE matrix44f power(int i) const;

	// Gets one set of possible x-y-z fixed angles that will generate this matrix
	// Assumes that upper 3x3 is a rotation matrix
    NEFEXP_CORE void getFixedAngles( float& xRotation, float& yRotation, float& zRotation );
	// Gets one possible axis-angle pair that will generate this matrix
	// Assumes that upper 3x3 is a rotation matrix
    NEFEXP_CORE void getAxisAngle( vec3f& axis, float& angle );

	inline matrix44f& operator+=(const matrix44f& other);
	inline matrix44f& operator-=(const matrix44f& other);
	inline matrix44f& operator*=(const matrix44f& other);
	inline matrix44f& operator*=(float factor);
    NEFEXP_CORE matrix44f& operator/=(float divisor);
	inline bool operator==(const matrix44f& other) const;
	inline bool operator!=(const matrix44f& other) const;

	inline matrix44f& operator=(const matrix44f& A);
    NEFEXP_CORE matrix44f& operator=(const matrix44d& A);

    NEFEXP_CORE friend matrix44f operator+(const matrix44f& m1, const matrix44f& m2);
    NEFEXP_CORE friend matrix44f operator-(const matrix44f& m1, const matrix44f& m2);
    NEFEXP_CORE friend matrix44f operator*(const matrix44f& m1, const matrix44f& m2);
    NEFEXP_CORE friend vec3f operator*(const matrix44f& matrix, const vec3f& vector);
    NEFEXP_CORE friend vec3f operator*(const vec3f& vector, const matrix44f& matrix);
    NEFEXP_CORE friend vec4f operator*(const vec4f& vector, const matrix44f& matrix);
    NEFEXP_CORE friend vec4f operator*(const matrix44f& matrix, const vec4f& vector);
    NEFEXP_CORE friend AABB operator*(const matrix44f& matrix, const AABB& aabb);
    NEFEXP_CORE friend matrix44f operator-(const matrix44f& matrix);
    NEFEXP_CORE friend matrix44f operator*(float factor, const matrix44f& matrix);
    NEFEXP_CORE friend matrix44f operator*(const matrix44f& matrix, float factor);
    NEFEXP_CORE friend  matrix44f operator/(const matrix44f& matrix, float divisor);

	friend inline bool qFuzzyCompare(const matrix44f& m1, const matrix44f& m2);

    NEFEXP_CORE void scale(const vec3f& vector);
    NEFEXP_CORE void translate(const vec3f& vector);
    NEFEXP_CORE void rotate(float angle, const vec3f& vector);
    NEFEXP_CORE void scale(float x, float y);
    NEFEXP_CORE void scale(float x, float y, float z);
    NEFEXP_CORE void scale(float factor);
    NEFEXP_CORE void translate(float x, float y);
    NEFEXP_CORE void translate(float x, float y, float z);
    NEFEXP_CORE void rotate(float angle, float x, float y, float z = 0.0f);
    NEFEXP_CORE void rotate(const quatf& quaternion);

    NEFEXP_CORE void ortho(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    NEFEXP_CORE void frustum(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    NEFEXP_CORE void perspective(float angle, float aspect, float nearPlane, float farPlane);
    NEFEXP_CORE void lookAt(const vec3f& eye, const vec3f& center, const vec3f& up);
    NEFEXP_CORE void flipCoordinates();

    NEFEXP_CORE void copyDataTo(float *values) const;

    NEFEXP_CORE vec3f map(const vec3f& point) const;
    NEFEXP_CORE vec3f mapVector(const vec3f& vector) const;
    NEFEXP_CORE vec4f map(const vec4f& point) const;

	inline float *data();
	inline const float *data() const { return m[0]; }
	inline const float *constData() const { return m[0]; }

    NEFEXP_CORE void optimize();

    NEFEXP_CORE operator QVariant() const;

    NEFEXP_CORE void decompose(vec3f &position, quatf &rotation, vec3f &scale);

	inline QString toUtf8() const;

	inline friend QDebug operator<<(QDebug dbg, const matrix44f &m);
	inline friend QDataStream &operator<<(QDataStream &, const matrix44f &);
	inline friend QDataStream &operator>>(QDataStream &, matrix44f &);

private:
	float m[4][4];          // Column-major order to match OpenGL.
	int flagBits;           // Flag bits from the enum below.

	enum {
		Identity        = 0x0001,   // Identity matrix
		General         = 0x0002,   // General matrix, unknown contents
		Translation     = 0x0004,   // Contains a simple translation
		Scale           = 0x0008,   // Contains a simple scale
		Rotation        = 0x0010    // Contains a simple rotation
	};

	// Construct without initializing identity matrix.
	matrix44f(int) { flagBits = General; }

	matrix44f orthonormalInverse() const;

	void projectedRotate(float angle, float x, float y, float z);
};

Q_DECLARE_TYPEINFO(matrix44f, Q_MOVABLE_TYPE);

NEFEXP_CORE matrix44f operator/(const matrix44f& matrix, float divisor);

inline matrix44f operator+(const matrix44f& m1, const matrix44f& m2)
{
	matrix44f m(1);
	m.m[0][0] = m1.m[0][0] + m2.m[0][0];
	m.m[0][1] = m1.m[0][1] + m2.m[0][1];
	m.m[0][2] = m1.m[0][2] + m2.m[0][2];
	m.m[0][3] = m1.m[0][3] + m2.m[0][3];
	m.m[1][0] = m1.m[1][0] + m2.m[1][0];
	m.m[1][1] = m1.m[1][1] + m2.m[1][1];
	m.m[1][2] = m1.m[1][2] + m2.m[1][2];
	m.m[1][3] = m1.m[1][3] + m2.m[1][3];
	m.m[2][0] = m1.m[2][0] + m2.m[2][0];
	m.m[2][1] = m1.m[2][1] + m2.m[2][1];
	m.m[2][2] = m1.m[2][2] + m2.m[2][2];
	m.m[2][3] = m1.m[2][3] + m2.m[2][3];
	m.m[3][0] = m1.m[3][0] + m2.m[3][0];
	m.m[3][1] = m1.m[3][1] + m2.m[3][1];
	m.m[3][2] = m1.m[3][2] + m2.m[3][2];
	m.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return m;
}

inline matrix44f operator-(const matrix44f& m1, const matrix44f& m2)
{
	matrix44f m(1);
	m.m[0][0] = m1.m[0][0] - m2.m[0][0];
	m.m[0][1] = m1.m[0][1] - m2.m[0][1];
	m.m[0][2] = m1.m[0][2] - m2.m[0][2];
	m.m[0][3] = m1.m[0][3] - m2.m[0][3];
	m.m[1][0] = m1.m[1][0] - m2.m[1][0];
	m.m[1][1] = m1.m[1][1] - m2.m[1][1];
	m.m[1][2] = m1.m[1][2] - m2.m[1][2];
	m.m[1][3] = m1.m[1][3] - m2.m[1][3];
	m.m[2][0] = m1.m[2][0] - m2.m[2][0];
	m.m[2][1] = m1.m[2][1] - m2.m[2][1];
	m.m[2][2] = m1.m[2][2] - m2.m[2][2];
	m.m[2][3] = m1.m[2][3] - m2.m[2][3];
	m.m[3][0] = m1.m[3][0] - m2.m[3][0];
	m.m[3][1] = m1.m[3][1] - m2.m[3][1];
	m.m[3][2] = m1.m[3][2] - m2.m[3][2];
	m.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return m;
}

inline matrix44f operator*(const matrix44f& m1, const matrix44f& m2)
{
	if (m1.flagBits == matrix44f::Identity)
		return m2;
	else if (m2.flagBits == matrix44f::Identity)
		return m1;

	matrix44f m(1);
	m.m[0][0] = m1.m[0][0] * m2.m[0][0] +
		m1.m[1][0] * m2.m[0][1] +
		m1.m[2][0] * m2.m[0][2] +
		m1.m[3][0] * m2.m[0][3];
	m.m[0][1] = m1.m[0][1] * m2.m[0][0] +
		m1.m[1][1] * m2.m[0][1] +
		m1.m[2][1] * m2.m[0][2] +
		m1.m[3][1] * m2.m[0][3];
	m.m[0][2] = m1.m[0][2] * m2.m[0][0] +
		m1.m[1][2] * m2.m[0][1] +
		m1.m[2][2] * m2.m[0][2] +
		m1.m[3][2] * m2.m[0][3];
	m.m[0][3] = m1.m[0][3] * m2.m[0][0] +
		m1.m[1][3] * m2.m[0][1] +
		m1.m[2][3] * m2.m[0][2] +
		m1.m[3][3] * m2.m[0][3];
	m.m[1][0] = m1.m[0][0] * m2.m[1][0] +
		m1.m[1][0] * m2.m[1][1] +
		m1.m[2][0] * m2.m[1][2] +
		m1.m[3][0] * m2.m[1][3];
	m.m[1][1] = m1.m[0][1] * m2.m[1][0] +
		m1.m[1][1] * m2.m[1][1] +
		m1.m[2][1] * m2.m[1][2] +
		m1.m[3][1] * m2.m[1][3];
	m.m[1][2] = m1.m[0][2] * m2.m[1][0] +
		m1.m[1][2] * m2.m[1][1] +
		m1.m[2][2] * m2.m[1][2] +
		m1.m[3][2] * m2.m[1][3];
	m.m[1][3] = m1.m[0][3] * m2.m[1][0] +
		m1.m[1][3] * m2.m[1][1] +
		m1.m[2][3] * m2.m[1][2] +
		m1.m[3][3] * m2.m[1][3];
	m.m[2][0] = m1.m[0][0] * m2.m[2][0] +
		m1.m[1][0] * m2.m[2][1] +
		m1.m[2][0] * m2.m[2][2] +
		m1.m[3][0] * m2.m[2][3];
	m.m[2][1] = m1.m[0][1] * m2.m[2][0] +
		m1.m[1][1] * m2.m[2][1] +
		m1.m[2][1] * m2.m[2][2] +
		m1.m[3][1] * m2.m[2][3];
	m.m[2][2] = m1.m[0][2] * m2.m[2][0] +
		m1.m[1][2] * m2.m[2][1] +
		m1.m[2][2] * m2.m[2][2] +
		m1.m[3][2] * m2.m[2][3];
	m.m[2][3] = m1.m[0][3] * m2.m[2][0] +
		m1.m[1][3] * m2.m[2][1] +
		m1.m[2][3] * m2.m[2][2] +
		m1.m[3][3] * m2.m[2][3];
	m.m[3][0] = m1.m[0][0] * m2.m[3][0] +
		m1.m[1][0] * m2.m[3][1] +
		m1.m[2][0] * m2.m[3][2] +
		m1.m[3][0] * m2.m[3][3];
	m.m[3][1] = m1.m[0][1] * m2.m[3][0] +
		m1.m[1][1] * m2.m[3][1] +
		m1.m[2][1] * m2.m[3][2] +
		m1.m[3][1] * m2.m[3][3];
	m.m[3][2] = m1.m[0][2] * m2.m[3][0] +
		m1.m[1][2] * m2.m[3][1] +
		m1.m[2][2] * m2.m[3][2] +
		m1.m[3][2] * m2.m[3][3];
	m.m[3][3] = m1.m[0][3] * m2.m[3][0] +
		m1.m[1][3] * m2.m[3][1] +
		m1.m[2][3] * m2.m[3][2] +
		m1.m[3][3] * m2.m[3][3];
	return m;
}


inline matrix44f operator-(const matrix44f& matrix)
{
	matrix44f m(1);
	m.m[0][0] = -matrix.m[0][0];
	m.m[0][1] = -matrix.m[0][1];
	m.m[0][2] = -matrix.m[0][2];
	m.m[0][3] = -matrix.m[0][3];
	m.m[1][0] = -matrix.m[1][0];
	m.m[1][1] = -matrix.m[1][1];
	m.m[1][2] = -matrix.m[1][2];
	m.m[1][3] = -matrix.m[1][3];
	m.m[2][0] = -matrix.m[2][0];
	m.m[2][1] = -matrix.m[2][1];
	m.m[2][2] = -matrix.m[2][2];
	m.m[2][3] = -matrix.m[2][3];
	m.m[3][0] = -matrix.m[3][0];
	m.m[3][1] = -matrix.m[3][1];
	m.m[3][2] = -matrix.m[3][2];
	m.m[3][3] = -matrix.m[3][3];
	return m;
}

inline matrix44f operator*(float factor, const matrix44f& matrix)
{
	matrix44f m(1);
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	m.m[0][2] = matrix.m[0][2] * factor;
	m.m[0][3] = matrix.m[0][3] * factor;
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;
	m.m[1][2] = matrix.m[1][2] * factor;
	m.m[1][3] = matrix.m[1][3] * factor;
	m.m[2][0] = matrix.m[2][0] * factor;
	m.m[2][1] = matrix.m[2][1] * factor;
	m.m[2][2] = matrix.m[2][2] * factor;
	m.m[2][3] = matrix.m[2][3] * factor;
	m.m[3][0] = matrix.m[3][0] * factor;
	m.m[3][1] = matrix.m[3][1] * factor;
	m.m[3][2] = matrix.m[3][2] * factor;
	m.m[3][3] = matrix.m[3][3] * factor;
	return m;
}

inline matrix44f operator*(const matrix44f& matrix, float factor)
{
	matrix44f m(1);
	m.m[0][0] = matrix.m[0][0] * factor;
	m.m[0][1] = matrix.m[0][1] * factor;
	m.m[0][2] = matrix.m[0][2] * factor;
	m.m[0][3] = matrix.m[0][3] * factor;
	m.m[1][0] = matrix.m[1][0] * factor;
	m.m[1][1] = matrix.m[1][1] * factor;
	m.m[1][2] = matrix.m[1][2] * factor;
	m.m[1][3] = matrix.m[1][3] * factor;
	m.m[2][0] = matrix.m[2][0] * factor;
	m.m[2][1] = matrix.m[2][1] * factor;
	m.m[2][2] = matrix.m[2][2] * factor;
	m.m[2][3] = matrix.m[2][3] * factor;
	m.m[3][0] = matrix.m[3][0] * factor;
	m.m[3][1] = matrix.m[3][1] * factor;
	m.m[3][2] = matrix.m[3][2] * factor;
	m.m[3][3] = matrix.m[3][3] * factor;
	return m;
}

inline bool qFuzzyCompare(const matrix44f& m1, const matrix44f& m2)
{
	return qFuzzyCompare(m1.m[0][0], m2.m[0][0]) &&
		qFuzzyCompare(m1.m[0][1], m2.m[0][1]) &&
		qFuzzyCompare(m1.m[0][2], m2.m[0][2]) &&
		qFuzzyCompare(m1.m[0][3], m2.m[0][3]) &&
		qFuzzyCompare(m1.m[1][0], m2.m[1][0]) &&
		qFuzzyCompare(m1.m[1][1], m2.m[1][1]) &&
		qFuzzyCompare(m1.m[1][2], m2.m[1][2]) &&
		qFuzzyCompare(m1.m[1][3], m2.m[1][3]) &&
		qFuzzyCompare(m1.m[2][0], m2.m[2][0]) &&
		qFuzzyCompare(m1.m[2][1], m2.m[2][1]) &&
		qFuzzyCompare(m1.m[2][2], m2.m[2][2]) &&
		qFuzzyCompare(m1.m[2][3], m2.m[2][3]) &&
		qFuzzyCompare(m1.m[3][0], m2.m[3][0]) &&
		qFuzzyCompare(m1.m[3][1], m2.m[3][1]) &&
		qFuzzyCompare(m1.m[3][2], m2.m[3][2]) &&
		qFuzzyCompare(m1.m[3][3], m2.m[3][3]);
}

QDebug operator<<(QDebug dbg, const matrix44f &m)
{
    // Create a string that represents the matrix type.
    QByteArray bits;
    if ((m.flagBits & matrix44f::Identity) != 0)
        bits += "Identity,";
    if ((m.flagBits & matrix44f::General) != 0)
        bits += "General,";
    if ((m.flagBits & matrix44f::Translation) != 0)
        bits += "Translation,";
    if ((m.flagBits & matrix44f::Scale) != 0)
        bits += "Scale,";
    if ((m.flagBits & matrix44f::Rotation) != 0)
        bits += "Rotation,";
    if (bits.size() > 0)
        bits = bits.left(bits.size() - 1);

    // Output in row-major order because it is more human-readable.
    dbg.nospace() << "matrix44f(type:" << bits.constData() << endl
        << qSetFieldWidth(10)
        << m(0, 0) << m(0, 1) << m(0, 2) << m(0, 3) << endl
        << m(1, 0) << m(1, 1) << m(1, 2) << m(1, 3) << endl
        << m(2, 0) << m(2, 1) << m(2, 2) << m(2, 3) << endl
        << m(3, 0) << m(3, 1) << m(3, 2) << m(3, 3) << endl
        << qSetFieldWidth(0) << ')';
    return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const matrix44f &matrix)
{
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            stream << double(matrix(row, col));
    return stream;
}


QDataStream &operator>>(QDataStream &stream, matrix44f &matrix)
{
    double x;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            stream >> x;
            matrix(row, col) = double(x);
        }
    }
    matrix.optimize();
    return stream;
}


#include "Utility/Math/matrix44f.inl"
