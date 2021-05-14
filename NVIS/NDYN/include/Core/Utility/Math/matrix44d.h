#pragma once

class QVariant;
class vec3d;
class vec4d;
class matrix33d;
class matrix44f;
class quatd;

class matrix44d
{
public:
	inline matrix44d() { setToIdentity(); }
    NEFEXP_CORE explicit matrix44d(const double *values);
	inline matrix44d(double m11, double m12, double m13, double m14,
		double m21, double m22, double m23, double m24,
		double m31, double m32, double m33, double m34,
		double m41, double m42, double m43, double m44);
    NEFEXP_CORE matrix44d(const double *values, int cols, int rows);
    NEFEXP_CORE matrix44d(const matrix33d& matrix);
	inline matrix44d(const matrix44d& matrix);
    NEFEXP_CORE matrix44d(const matrix44f& matrix);

	inline const double& operator()(int row, int column) const;
	inline double& operator()(int row, int column);

    NEFEXP_CORE vec4d column(int index) const;
    NEFEXP_CORE void setColumn(int index, const vec4d& value);

    NEFEXP_CORE vec4d row(int index) const;
    NEFEXP_CORE void setRow(int index, const vec4d& value);

	inline bool isIdentity() const;
	inline void setToIdentity();

	inline bool isZero() const;
	inline void setToZero();

	inline void fill(double value);

    NEFEXP_CORE double determinant() const;
    NEFEXP_CORE matrix44d inverted(bool *invertible = 0) const;
    NEFEXP_CORE matrix44d transposed() const;
    NEFEXP_CORE matrix33d normalMatrix() const;

    NEFEXP_CORE double trace() const;
    NEFEXP_CORE vec4d diagonal() const;
    NEFEXP_CORE matrix44d power(int i) const;
	// Gets one set of possible x-y-z fixed angles that will generate this matrix
	// Assumes that upper 3x3 is a rotation matrix
    NEFEXP_CORE void getFixedAngles( double& xRotation, double& yRotation, double& zRotation );
	// Gets one possible axis-angle pair that will generate this matrix
	// Assumes that upper 3x3 is a rotation matrix
    NEFEXP_CORE void getAxisAngle( vec3d& axis, double& angle );

	inline matrix44d& operator+=(const matrix44d& other);
	inline matrix44d& operator-=(const matrix44d& other);
	inline matrix44d& operator*=(const matrix44d& other);
	inline matrix44d& operator*=(double factor);
    NEFEXP_CORE matrix44d& operator/=(double divisor);
	inline bool operator==(const matrix44d& other) const;
	inline bool operator!=(const matrix44d& other) const;

	inline matrix44d& operator=(const matrix44d& A);
    NEFEXP_CORE matrix44d& operator=(const matrix44f& A);

    NEFEXP_CORE friend matrix44d operator+(const matrix44d& m1, const matrix44d& m2);
    NEFEXP_CORE friend matrix44d operator-(const matrix44d& m1, const matrix44d& m2);
    NEFEXP_CORE friend matrix44d operator*(const matrix44d& m1, const matrix44d& m2);
    NEFEXP_CORE friend vec3d operator*(const matrix44d& matrix, const vec3d& vector);
    NEFEXP_CORE friend vec3d operator*(const vec3d& vector, const matrix44d& matrix);
    NEFEXP_CORE friend vec4d operator*(const vec4d& vector, const matrix44d& matrix);
    NEFEXP_CORE friend vec4d operator*(const matrix44d& matrix, const vec4d& vector);
    NEFEXP_CORE friend matrix44d operator-(const matrix44d& matrix);
    NEFEXP_CORE friend matrix44d operator*(double factor, const matrix44d& matrix);
    NEFEXP_CORE friend matrix44d operator*(const matrix44d& matrix, double factor);
    friend NEFEXP_CORE matrix44d operator/(const matrix44d& matrix, double divisor);

	friend inline bool qFuzzyCompare(const matrix44d& m1, const matrix44d& m2);

    NEFEXP_CORE void scale(const vec3d& vector);
    NEFEXP_CORE void translate(const vec3d& vector);
    NEFEXP_CORE void rotate(double angle, const vec3d& vector);
    NEFEXP_CORE void scale(double x, double y);
    NEFEXP_CORE void scale(double x, double y, double z);
    NEFEXP_CORE void scale(double factor);
    NEFEXP_CORE void translate(double x, double y);
    NEFEXP_CORE void translate(double x, double y, double z);
    NEFEXP_CORE void rotate(double angle, double x, double y, double z = 0.0);
    NEFEXP_CORE void rotate(const quatd& quaternion);

    NEFEXP_CORE void ortho(double left, double right, double bottom, double top, double nearPlane, double farPlane);
    NEFEXP_CORE void frustum(double left, double right, double bottom, double top, double nearPlane, double farPlane);
    NEFEXP_CORE void perspective(double angle, double aspect, double nearPlane, double farPlane);
    NEFEXP_CORE void lookAt(const vec3d& eye, const vec3d& center, const vec3d& up);
    NEFEXP_CORE void flipCoordinates();

    NEFEXP_CORE void copyDataTo(double *values) const;

    NEFEXP_CORE vec3d map(const vec3d& point) const;
    NEFEXP_CORE vec3d mapVector(const vec3d& vector) const;
    NEFEXP_CORE vec4d map(const vec4d& point) const;

	inline double *data();
	inline const double *data() const { return m[0]; }
	inline const double *constData() const { return m[0]; }

    NEFEXP_CORE void optimize();

    NEFEXP_CORE operator QVariant() const;

	inline friend QDebug operator<<(QDebug dbg, const matrix44d &m);
	inline friend QDataStream &operator<<(QDataStream &, const matrix44d &);
	inline friend QDataStream &operator>>(QDataStream &, matrix44d &);

private:
	double m[4][4];          // Column-major order to match OpenGL.
	int flagBits;           // Flag bits from the enum below.

	enum {
		Identity        = 0x0001,   // Identity matrix
		General         = 0x0002,   // General matrix, unknown contents
		Translation     = 0x0004,   // Contains a simple translation
		Scale           = 0x0008,   // Contains a simple scale
		Rotation        = 0x0010    // Contains a simple rotation
	};

	// Construct without initializing identity matrix.
	matrix44d(int) { flagBits = General; }

	matrix44d orthonormalInverse() const;

	void projectedRotate(double angle, double x, double y, double z);
};

Q_DECLARE_TYPEINFO(matrix44d, Q_MOVABLE_TYPE);

NEFEXP_CORE matrix44d operator/(const matrix44d& matrix, double divisor);

inline matrix44d operator+(const matrix44d& m1, const matrix44d& m2)
{
	matrix44d m(1);
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

inline matrix44d operator-(const matrix44d& m1, const matrix44d& m2)
{
	matrix44d m(1);
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

inline matrix44d operator*(const matrix44d& m1, const matrix44d& m2)
{
	if (m1.flagBits == matrix44d::Identity)
		return m2;
	else if (m2.flagBits == matrix44d::Identity)
		return m1;

	matrix44d m(1);
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


inline matrix44d operator-(const matrix44d& matrix)
{
	matrix44d m(1);
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

inline matrix44d operator*(double factor, const matrix44d& matrix)
{
	matrix44d m(1);
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

inline matrix44d operator*(const matrix44d& matrix, double factor)
{
	matrix44d m(1);
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

inline bool qFuzzyCompare(const matrix44d& m1, const matrix44d& m2)
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

QDebug operator<<(QDebug dbg, const matrix44d &m)
{
	// Create a string that represents the matrix type.
	QByteArray bits;
	if ((m.flagBits & matrix44d::Identity) != 0)
		bits += "Identity,";
	if ((m.flagBits & matrix44d::General) != 0)
		bits += "General,";
	if ((m.flagBits & matrix44d::Translation) != 0)
		bits += "Translation,";
	if ((m.flagBits & matrix44d::Scale) != 0)
		bits += "Scale,";
	if ((m.flagBits & matrix44d::Rotation) != 0)
		bits += "Rotation,";
	if (bits.size() > 0)
		bits = bits.left(bits.size() - 1);

	// Output in row-major order because it is more human-readable.
	dbg.nospace() << "matrix44d(type:" << bits.constData() << endl
		<< qSetFieldWidth(10)
		<< m(0, 0) << m(0, 1) << m(0, 2) << m(0, 3) << endl
		<< m(1, 0) << m(1, 1) << m(1, 2) << m(1, 3) << endl
		<< m(2, 0) << m(2, 1) << m(2, 2) << m(2, 3) << endl
		<< m(3, 0) << m(3, 1) << m(3, 2) << m(3, 3) << endl
		<< qSetFieldWidth(0) << ')';
	return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const matrix44d &matrix)
{
	for (int row = 0; row < 4; ++row)
		for (int col = 0; col < 4; ++col)
			stream << double(matrix(row, col));
	return stream;
}


QDataStream &operator>>(QDataStream &stream, matrix44d &matrix)
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

#include "Utility/Math/matrix44d.inl"

