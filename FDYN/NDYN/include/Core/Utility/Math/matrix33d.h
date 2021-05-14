#pragma once

class QVariant;
class vec3d;
class vec3f;
class matrix33f;

class matrix33d
{
public:
    friend class matrix33dWrapper;

	inline matrix33d() { setToIdentity(); }
    NEFEXP_CORE explicit matrix33d(const double *values);
	inline matrix33d(double m11, double m12, double m13, 
		double m21, double m22, double m23, 
		double m31, double m32, double m33);
    inline matrix33d(vec3d  vec1, vec3d  vec2, vec3d vec3, bool column = 0)
    {
        if(column)
        {
            m[0][0] = vec1.x(); m[0][1] = vec2.x(); m[0][2] = vec3.x();
            m[1][0] = vec1.y(); m[1][1] = vec2.y(); m[1][2] = vec3.y();
            m[2][0] = vec1.z(); m[2][1] = vec2.z(); m[2][2] = vec3.z();
        } else {
            m[0][0] = vec1.x(); m[0][1] = vec1.y(); m[0][2] = vec1.z();
            m[1][0] = vec2.x(); m[1][1] = vec2.y(); m[1][2] = vec2.z();
            m[2][0] = vec3.x(); m[2][1] = vec3.y(); m[2][2] = vec3.z();
        }
    }
    inline matrix33d(vec3f  vec1, vec3f  vec2, vec3f vec3, const bool column = 0)
    {
        if(column)
        {
            m[0][0] = vec1.x(); m[0][1] = vec2.x(); m[0][2] = vec3.x();
            m[1][0] = vec1.y(); m[1][1] = vec2.y(); m[1][2] = vec3.y();
            m[2][0] = vec1.z(); m[2][1] = vec2.z(); m[2][2] = vec3.z();
        } else {
            m[0][0] = vec1.x(); m[0][1] = vec1.y(); m[0][2] = vec1.z();
            m[1][0] = vec2.x(); m[1][1] = vec2.y(); m[1][2] = vec2.z();
            m[2][0] = vec3.x(); m[2][1] = vec3.y(); m[2][2] = vec3.z();
        }
    }

    NEFEXP_CORE matrix33d(const double *values, int cols, int rows);
	inline matrix33d(const matrix33d& matrix);
    NEFEXP_CORE matrix33d(const matrix33f& matrix);

    static inline matrix33d tensorProduct(const vec3d& vec1, const vec3d& vec2)
    {
        return matrix33d(vec1.x()*vec2.x(), vec1.x()*vec2.y(), vec1.x()*vec2.z(),
                         vec1.y()*vec2.x(), vec1.y()*vec2.y(), vec1.y()*vec2.z(),
                         vec1.z()*vec2.x(), vec1.z()*vec2.y(), vec1.z()*vec2.z());
    }

    inline double m11() const;
    inline double& m11();
    inline double m12() const;
    inline double& m12();
    inline double m13() const;
    inline double& m13();
    inline double m21() const;
    inline double& m21();
    inline double m22() const;
    inline double& m22();
    inline double m23() const;
    inline double& m23();
    inline double m31() const;
    inline double& m31();
    inline double m32() const;
    inline double& m32();
    inline double m33() const;
    inline double& m33();

	inline const double& operator()(int row, int column) const;
	inline double& operator()(int row, int column);

    NEFEXP_CORE vec3d column(int index) const;
    NEFEXP_CORE void setColumn(int index, const vec3d& value);

    NEFEXP_CORE vec3d row(int index) const;
    NEFEXP_CORE void setRow(int index, const vec3d& value);

    inline void setM11(double m11);
    inline void setM12(double m12);
    inline void setM13(double m13);
    inline void setM21(double m21);
    inline void setM22(double m22);
    inline void setM23(double m23);
    inline void setM31(double m31);
    inline void setM32(double m32);
    inline void setM33(double m33);

    inline void set(double m11, double m12, double m13,
                    double m21, double m22, double m23,
                    double m31, double m32, double m33);

    inline void set(const matrix33d& matrix);

	inline bool isIdentity() const;
	inline void setToIdentity();

	inline bool isZero() const;
	inline void setToZero();

	inline void fill(double value);

    NEFEXP_CORE matrix33d transposed() const;
    NEFEXP_CORE matrix33d adjoint() const;
    NEFEXP_CORE matrix33d power(int i) const;
    NEFEXP_CORE double determinant() const;
    NEFEXP_CORE double trace() const;
    NEFEXP_CORE vec3d diagonal() const;

    NEFEXP_CORE matrix33d inverse(bool *invertible = 0) const;
    NEFEXP_CORE void invert(bool *invertible = 0);

	inline matrix33d& operator+=(const matrix33d& other);
	inline matrix33d& operator-=(const matrix33d& other);
	inline matrix33d& operator*=(const matrix33d& other);
	inline matrix33d& operator*=(double factor);
    NEFEXP_CORE matrix33d& operator/=(double divisor);
	inline bool operator==(const matrix33d& other) const;
	inline bool operator!=(const matrix33d& other) const;

	inline matrix33d& operator=(const matrix33d& A);
    NEFEXP_CORE matrix33d& operator=(const matrix33f& A);

    NEFEXP_CORE friend matrix33d operator+(const matrix33d& m1, const matrix33d& m2);
    NEFEXP_CORE friend matrix33d operator-(const matrix33d& m1, const matrix33d& m2);
    NEFEXP_CORE friend matrix33d operator*(const matrix33d& m1, const matrix33d& m2);
    NEFEXP_CORE friend vec3d operator*(const matrix33d& matrix, const vec3d& vector);
    NEFEXP_CORE friend vec3d operator*(const vec3d& vector, const matrix33d& matrix);
    NEFEXP_CORE friend matrix33d operator-(const matrix33d& matrix);
    NEFEXP_CORE friend matrix33d operator*(double factor, const matrix33d& matrix);
    NEFEXP_CORE friend matrix33d operator*(const matrix33d& matrix, double factor);
    NEFEXP_CORE friend matrix33d operator/(const matrix33d& matrix, double divisor);

	friend inline bool qFuzzyCompare(const matrix33d& m1, const matrix33d& m2);

    NEFEXP_CORE void rotate(double angle, const vec3d& vector);
    NEFEXP_CORE void rotate(double angle, double x, double y, double z = 0.0f);

    NEFEXP_CORE void computeMaxEigenValue(double& valMax);

    NEFEXP_CORE void copyDataTo(double *values) const;

    NEFEXP_CORE vec3d mapVector(const vec3d& vector) const;

	inline double *data();
	inline const double *data() const { return m[0]; }
	inline const double *constData() const { return m[0]; }

    NEFEXP_CORE operator QVariant() const;

private:
	double m[3][3];          // Column-major order to match OpenGL.
    friend inline QDebug operator<<(QDebug dbg, const matrix33d &matrix);
    friend inline QDataStream &operator<<(QDataStream &, const matrix33d &);
    friend inline QDataStream &operator>>(QDataStream &, matrix33d &);
};

Q_DECLARE_TYPEINFO(matrix33d, Q_MOVABLE_TYPE);

NEFEXP_CORE matrix33d operator/(const matrix33d& matrix, double divisor);

inline matrix33d operator+(const matrix33d& m1, const matrix33d& m2)
{
	matrix33d m;
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

inline matrix33d operator-(const matrix33d& m1, const matrix33d& m2)
{
	matrix33d m;
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

matrix33d& matrix33d::operator=( const matrix33d& A )
{
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			m[i][j] = A.m[i][j];

	return *this;
}

inline matrix33d operator*(const matrix33d& m1, const matrix33d& m2)
{

	matrix33d m;
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


inline matrix33d operator-(const matrix33d& matrix)
{
	matrix33d m;
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

inline matrix33d operator*(double factor, const matrix33d& matrix)
{
	matrix33d m;
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

inline matrix33d operator*(const matrix33d& matrix, double factor)
{
	matrix33d m;
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


QDebug operator<<(QDebug dbg, const matrix33d &matrix)
{
    dbg.nospace() << "matrix33d("
        << matrix.m11() << ", " << matrix.m12() << ", " << matrix.m13() << "\n "
        << matrix.m21() << ", " << matrix.m22() << ", " << matrix.m23() << "\n "
        << matrix.m31() << ", " << matrix.m32() << ", " << matrix.m33() << ')';
    return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const matrix33d &matrix)
{
    stream << matrix.m11() << matrix.m12() << matrix.m13()
           << matrix.m21() << matrix.m22() << matrix.m23()
           << matrix.m31() << matrix.m32() << matrix.m33();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, matrix33d &matrix)
{
    double m11, m12, m13, m21, m22, m23, m31, m32, m33;
    stream >> m11;
    stream >> m12;
    stream >> m13;
    stream >> m21;
    stream >> m22;
    stream >> m23;
    stream >> m31;
    stream >> m32;
    stream >> m33;
    matrix.setM11(m11);
    matrix.setM12(m12);
    matrix.setM13(m13);
    matrix.setM21(m21);
    matrix.setM22(m22);
    matrix.setM23(m23);
    matrix.setM31(m31);
    matrix.setM32(m32);
    matrix.setM33(m33);
    return stream;
}

Q_DECLARE_METATYPE(matrix33d)

inline bool qFuzzyCompare(const matrix33d& m1, const matrix33d& m2)
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

class matrix33dWrapper : public QObject
{
    Q_OBJECT

public slots:
    matrix33d* new_matrix33d();
    matrix33d* new_matrix33d(double m11, double m12, double m13,
                             double m21, double m22, double m23,
                             double m31, double m32, double m33);
    matrix33d* new_matrix33d(const vec3f& vec1, const vec3f& vec2, const vec3f& vec3, bool column);
    matrix33d* new_matrix33d(const matrix33d& other) {
    matrix33d* a = new matrix33d();
    *((matrix33d*)a) = other;
    return a; }
    void delete_matrix33d(matrix33d* obj) { delete obj; }

    matrix33d transposed(matrix33d* theWrappedObject) const;
    matrix33d adjoint(matrix33d* theWrappedObject) const;
    matrix33d power(matrix33d* theWrappedObject, int i) const;
    double determinant(matrix33d* theWrappedObject) const;
    double trace(matrix33d* theWrappedObject) const;
    vec3d diagonal(matrix33d* theWrappedObject) const;
    matrix33d inverse(matrix33d* theWrappedObject, bool *invertible = 0) const;
    void invert(matrix33d* theWrappedObject, bool *invertible = 0);

    matrix33d  __mul__(matrix33d* theWrappedObject, const matrix33d&  matrix);
    matrix33d  __mul__(matrix33d* theWrappedObject, double factor);
    vec3d  __mul__(matrix33d* theWrappedObject, vec3d  vector);
    matrix33d*  __imul__(matrix33d* theWrappedObject, const matrix33d&  matrix);
    matrix33d*  __imul__(matrix33d* theWrappedObject, float  factor);
    const matrix33d  __add__(matrix33d* theWrappedObject, const matrix33d&  matrix2);
    matrix33d*  __iadd__(matrix33d* theWrappedObject, const matrix33d&  matrix);
    const matrix33d  __sub__(matrix33d* theWrappedObject, const matrix33d&  matrix2);
    matrix33d*  __isub__(matrix33d* theWrappedObject, const matrix33d&  matrix);
    const matrix33d  __div__(matrix33d* theWrappedObject, double  divisor);
    matrix33d*  __idiv__(matrix33d* theWrappedObject, double  divisor);
    bool  __eq__(matrix33d* theWrappedObject, const matrix33d&  matrix2);

    void setM11(matrix33d* theWrappedObject, float  m11);
    void setM12(matrix33d* theWrappedObject, float  m12);
    void setM13(matrix33d* theWrappedObject, float  m13);
    void setM21(matrix33d* theWrappedObject, float  m21);
    void setM22(matrix33d* theWrappedObject, float  m22);
    void setM23(matrix33d* theWrappedObject, float  m23);
    void setM31(matrix33d* theWrappedObject, float  m31);
    void setM32(matrix33d* theWrappedObject, float  m32);
    void setM33(matrix33d* theWrappedObject, float  m33);

    double  m11(matrix33d* theWrappedObject) const;
    double  m12(matrix33d* theWrappedObject) const;
    double  m13(matrix33d* theWrappedObject) const;
    double  m21(matrix33d* theWrappedObject) const;
    double  m22(matrix33d* theWrappedObject) const;
    double  m23(matrix33d* theWrappedObject) const;
    double  m31(matrix33d* theWrappedObject) const;
    double  m32(matrix33d* theWrappedObject) const;
    double  m33(matrix33d* theWrappedObject) const;

    QString py_toString(matrix33d*);
};

#include "Utility/Math/matrix33d.inl"




