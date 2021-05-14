#pragma once

class vec2d;
class vec3d;
class vec4d;
class vec3f;
class vec4f;
class eulerf;
class matrix44f;
class QVariant;
class vec2fWrapper;
class vec3fWrapper;

class vec2f
{
public:
    friend class vec2fWrapper;

    inline vec2f();
    inline vec2f(float val);
	inline vec2f(float xpos, float ypos);
	inline vec2f(const vec2f& vec);
    NEFEXP_CORE vec2f(const vec2d& vec);
    NEFEXP_CORE explicit vec2f(const vec3f& vector);
    NEFEXP_CORE explicit vec2f(const vec4f& vector);

	inline float x() const;
	inline float& x();
	inline float y() const;
	inline float& y();

	inline void setX(float x);
	inline void setY(float y);
	inline void set(float x, float y);
	inline void set(const vec2f& vec);

    NEFEXP_CORE float length() const;
    NEFEXP_CORE float lengthSquared() const;

    NEFEXP_CORE vec2f normalized() const;
    NEFEXP_CORE void normalize();

	inline vec2f& operator+=(const vec2f &vector);
	inline vec2f& operator-=(const vec2f &vector);
	inline vec2f& operator*=(float factor);
	inline vec2f& operator*=(const vec2f &vector);
    inline vec2f& operator/=(float divisor);
    NEFEXP_CORE vec2f& operator = (const vec2d& vec);
	inline float& operator [] (uint i) { return (&m_X)[i]; }
	inline float operator [] (uint i) const { return (&m_X)[i]; }

	inline float operator *(const vec2f& m) const;

    NEFEXP_CORE static float dotProduct(const vec2f& v1, const vec2f& v2);

	friend inline bool operator==(const vec2f &v1, const vec2f &v2);
	friend inline bool operator!=(const vec2f &v1, const vec2f &v2);
	friend inline const vec2f operator+(const vec2f &v1, const vec2f &v2);
	friend inline const vec2f operator-(const vec2f &v1, const vec2f &v2);
    friend inline const vec2f operator*(float factor, const vec2f &vector);
    friend inline const vec2f operator*(const vec2f &vector, float factor);
    friend inline const vec2f operator*(const vec2f &v1, const vec2f &v2);
	friend inline const vec2f operator-(const vec2f &vector);
	friend inline const vec2f operator/(const vec2f &vector, float divisor);
	friend inline const vec2f operator/(const vec2f &vector, const vec2f &vector2);

	friend inline bool qFuzzyCompare(const vec2f& v1, const vec2f& v2);
	friend inline float distTwovectors(const vec2f& v1, const vec2f& v2);
	friend inline float distTwovectorsSquare(const vec2f& v1, const vec2f& v2);

    NEFEXP_CORE vec3f toVector3D() const;
    NEFEXP_CORE vec4f toVector4D() const;

    NEFEXP_CORE operator QVariant() const;

private:
	float m_X, m_Y;

	inline vec2f(float xpos, float ypos, int dummy);

	friend class vec3f;
	friend class vec4f;
	friend inline QDebug operator<<(QDebug dbg, const vec2f &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec2f &);
	friend inline QDataStream &operator>>(QDataStream &, vec2f &);
};

Q_DECLARE_TYPEINFO(vec2f, Q_MOVABLE_TYPE);


inline bool operator==(const vec2f &v1, const vec2f &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y;
}

inline bool operator!=(const vec2f &v1, const vec2f &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y;
}

inline const vec2f operator+(const vec2f &v1, const vec2f &v2)
{
	return vec2f(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, 1);
}

inline const vec2f operator-(const vec2f &v1, const vec2f &v2)
{
	return vec2f(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, 1);
}

inline const vec2f operator*(float factor, const vec2f &vector)
{
	return vec2f(vector.m_X * factor, vector.m_Y * factor, 1);
}

inline const vec2f operator*(const vec2f &vector, float factor)
{
	return vec2f(vector.m_X * factor, vector.m_Y * factor, 1);
}

inline const vec2f operator*(const vec2f &v1, const vec2f &v2)
{
	return vec2f(v1.m_X * v2.m_X, v1.m_Y * v2.m_Y, 1);
}

inline const vec2f operator-(const vec2f &vector)
{
	return vec2f(-vector.m_X, -vector.m_Y, 1);
}

inline const vec2f operator/(const vec2f &vector, float divisor)
{
	return vec2f(vector.m_X / divisor, vector.m_Y / divisor, 1);
}

inline const vec2f operator/(const vec2f &vector, const vec2f &vector2)
{
	return vec2f(vector.m_X / vector2.m_X, vector.m_Y / vector2.m_Y, 1);
}

inline bool qFuzzyCompare(const vec2f& v1, const vec2f& v2)
{
	return qFuzzyCompare(v1.m_X, v2.m_X) && qFuzzyCompare(v1.m_Y, v2.m_Y);
}

inline float distTwovectors(const vec2f& v1, const vec2f& v2)
{
	float dx = v1.m_X - v2.m_X;
	float dy = v1.m_Y - v2.m_Y;
	return sqrt(dx*dx + dy*dy);
}


inline float distTwovectorsSquare(const vec2f& v1, const vec2f& v2)
{
	float dx = v1.m_X - v2.m_X;
	float dy = v1.m_Y - v2.m_Y;
	return (dx*dx + dy*dy);
}

QDebug operator<<(QDebug dbg, const vec2f &vector)
{
	dbg.nospace() << "vec2f(" << vector.x() << ", " << vector.y() << ')';
	return dbg.space();
}

QDataStream& operator>>(QDataStream& stream, vec2f &vector)
{
	float x, y;
	stream >> x;
	stream >> y;
	vector.setX(float(x));
	vector.setY(float(y));
	return stream;
}

class vec3f
{
public:
	friend class vec3fWrapper;

	inline vec3f();
    inline vec3f(float val);
	inline vec3f(float xpos, float ypos, float zpos);	
    NEFEXP_CORE vec3f(const vec2f& vector);
    NEFEXP_CORE vec3f(const vec2f& vector, float zpos);
    NEFEXP_CORE explicit vec3f(const vec4f& vector);
    NEFEXP_CORE explicit vec3f(const vec3d& vector);

	inline float x() const;
	inline float& x();
	inline float y() const;
	inline float& y();
	inline float z() const;
	inline float& z();

	inline void setX(float x);
	inline void setY(float y);
	inline void setZ(float z);
	inline void set(float x, float y, float z);
	inline void set(const vec3f& vec);
    NEFEXP_CORE void set(const vec3d& vec);

	inline float length() const;
	inline float lengthSquaredInline() const;

    NEFEXP_CORE vec3f normalized() const;
    NEFEXP_CORE void normalize();

    NEFEXP_CORE vec3f normalizedWithoutZeroApproximation() const;

	inline vec3f &operator+=(const vec3f &vector);
	inline vec3f &operator-=(const vec3f &vector);
	inline vec3f &operator*=(float factor);
	inline vec3f &operator*=(const vec3f& vector);
	inline vec3f &operator/=(float divisor);
    NEFEXP_CORE vec3f &operator=(const vec3d& vector);

	inline float& operator [] (uint i) { return (&m_X)[i]; }
	inline float operator [] (uint i) const { return (&m_X)[i]; }

	inline vec3f elementWiseMultiplication(const vec3f& scale) const
	{
		return vec3f(m_X * scale.x(), m_Y * scale.y(), m_Z * scale.z());
	}

	inline vec3f elementWiseDivision(const vec3f& scale) const
	{
		return vec3f(m_X / scale.x(), m_Y / scale.y(), m_Z / scale.z());
	}

	static inline float dotProduct(const vec3f& v1, const vec3f& v2)
	{
		 return v1.m_X * v2.m_X + v1.m_Y * v2.m_Y + v1.m_Z * v2.m_Z;
	}
	
	static inline vec3f crossProduct(const vec3f& v1, const vec3f& v2)
	{
		return vec3f(v1.m_Y * v2.m_Z - v1.m_Z * v2.m_Y,
			v1.m_Z * v2.m_X - v1.m_X * v2.m_Z,
			v1.m_X * v2.m_Y - v1.m_Y * v2.m_X, 1);		
	}

	static inline bool areEqual(const vec3f v1, const vec3f v2, float tolerance = FEps)
	{
		return (v1 - v2).length() < tolerance;
		//return fabsf(v1[0] - v2[0]) < tolerance && fabsf(v1[1] - v2[1]) < tolerance && fabsf(v1[2] - v2[2]) < tolerance;
	}

    NEFEXP_CORE static vec3f normal(const vec3f& v1, const vec3f& v2);
    NEFEXP_CORE static vec3f normal(const vec3f& v1, const vec3f& v2, const vec3f& v3);

    NEFEXP_CORE float distanceToPlane(const vec3f& plane, const vec3f& normal) const;
    NEFEXP_CORE float distanceToPlane(const vec3f& plane1, const vec3f& plane2, const vec3f& plane3) const;
    NEFEXP_CORE float distanceToLine(const vec3f& point, const vec3f& direction) const;
	
	//!returns the index of the smallest component
    NEFEXP_CORE int indexOfMinComponent() const;
    NEFEXP_CORE bool computeBaseVectors(vec3f &b1, vec3f &b2) const;
	friend inline bool operator==(const vec3f &v1, const vec3f &v2);
	friend inline bool operator!=(const vec3f &v1, const vec3f &v2);
	friend inline const vec3f operator+(const vec3f &v1, const vec3f &v2);
	friend inline const vec3f operator-(const vec3f &v1, const vec3f &v2);
    friend inline const vec3f operator*(float factor, const vec3f &vector);
    friend inline const vec3f operator*(const vec3f &vector, float factor);
	friend inline const vec3f operator-(const vec3f &vector);
	friend inline const vec3f operator/(const vec3f &vector, float divisor);
	

    inline float lengthSquared() const
    {
        return m_X * m_X + m_Y * m_Y + m_Z * m_Z;
    }
    //! Dot product
    inline float operator * (const vec3f& vec) const
    {
        return m_X*vec.m_X + m_Y*vec.m_Y + m_Z*vec.m_Z;
	}
	//! Cross product
	inline vec3f operator % (const vec3f& vec) const
	{
		return vec3f(m_Y*vec.m_Z - m_Z*vec.m_Y, m_Z*vec.m_X - m_X*vec.m_Z, m_X*vec.m_Y - m_Y*vec.m_X);
	}

	friend inline float distOfTwoVectors(const vec3f& v1, const vec3f& v2);
	friend inline float distOfTwoVectorsSquare(const vec3f& v1, const vec3f& v2);

    NEFEXP_CORE vec2f toVector2D() const;
    NEFEXP_CORE vec4f toVector4D() const;

    NEFEXP_CORE operator QVariant() const;
    NEFEXP_CORE QString toUtf8() const;

	// Static Variables
	static vec3f xAxis;
	static vec3f yAxis;
	static vec3f zAxis;
	static vec3f origin;

private:
	float m_X, m_Y, m_Z;

	inline vec3f(float xpos, float ypos, float zpos, int dummy);

	friend class vec2f;
	friend class vec4f;
	friend inline QDebug operator<<(QDebug dbg, const vec3f &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec3f &);
	friend inline QDataStream &operator>>(QDataStream &, vec3f &);

};

//Q_DECLARE_TYPEINFO(vec3f, Q_MOVABLE_TYPE);

inline bool operator==(const vec3f &v1, const vec3f &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y && v1.m_Z == v2.m_Z;
}

inline bool operator!=(const vec3f &v1, const vec3f &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y || v1.m_Z != v2.m_Z;
}

inline const vec3f operator+(const vec3f &v1, const vec3f &v2)
{
	return vec3f(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, v1.m_Z + v2.m_Z, 1);
}

inline const vec3f operator-(const vec3f &v1, const vec3f &v2)
{
	return vec3f(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, v1.m_Z - v2.m_Z, 1);
}

inline const vec3f operator*(float factor, const vec3f &vector)
{
	return vec3f(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, 1);
}

inline const vec3f operator*(const vec3f &vector, float factor)
{
	return vec3f(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, 1);
}

inline const vec3f operator-(const vec3f &vector)
{
	return vec3f(-vector.m_X, -vector.m_Y, -vector.m_Z, 1);
}

inline const vec3f operator/(const vec3f &vector, float divisor)
{
	return vec3f(vector.m_X / divisor, vector.m_Y / divisor, vector.m_Z / divisor, 1);
}

inline float distOfTwoVectors(const vec3f& v1, const vec3f& v2)
{
	float dx = v1.m_X - v2.m_X;
	float dy = v1.m_Y - v2.m_Y;
	float dz = v1.m_Z - v2.m_Z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}

inline float distOfTwoVectorsSquare(const vec3f& v1, const vec3f& v2)
{
	float dx = v1.m_X - v2.m_X;
	float dy = v1.m_Y - v2.m_Y;
	float dz = v1.m_Z - v2.m_Z;
	return (dx*dx + dy*dy + dz*dz);
}

QDebug operator<<(QDebug dbg, const vec3f &vector)
{
	dbg.nospace() << "vec3f(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ')';
	return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const vec3f &vector)
{
	stream << float(vector.x()) << float(vector.y())
		<< float(vector.z());
	return stream;
}

QDataStream& operator>>(QDataStream& stream, vec3f &vector)
{
	float x, y, z;
	stream >> x;
	stream >> y;
	stream >> z;
	vector.setX(float(x));
	vector.setY(float(y));
	vector.setZ(float(z));
	return stream;
}

class vec4f
{
public:
	inline vec4f();
    inline vec4f(float val);
	inline vec4f(float xpos, float ypos, float zpos, float wpos);
    NEFEXP_CORE vec4f(const vec2f& vector);
    NEFEXP_CORE vec4f(const vec2f& vector, float zpos, float wpos);
    NEFEXP_CORE vec4f(const vec3f& vector);
    NEFEXP_CORE vec4f(const vec3f& vector, float wpos);

	inline float x() const;
	inline float& x();
	inline float y() const;
	inline float& y();
	inline float z() const;
	inline float& z();
	inline float w() const;
	inline float& w();

	inline void setX(float x);
	inline void setY(float y);
	inline void setZ(float z);
	inline void setW(float w);
	inline void set(float x, float y, float z, float w);
	inline void set(const vec4f& vec);
    NEFEXP_CORE void set(const vec4d& vec);

    NEFEXP_CORE float length() const;
    NEFEXP_CORE float lengthSquared() const;

    NEFEXP_CORE vec4f normalized() const;
    NEFEXP_CORE void normalize();

	inline vec4f &operator+=(const vec4f &vector);
	inline vec4f &operator-=(const vec4f &vector);
	inline vec4f &operator*=(float factor);
	inline vec4f &operator*=(const vec4f &vector);
	inline vec4f &operator/=(float divisor);
    NEFEXP_CORE vec4f &operator=(const vec4d &vector);

    NEFEXP_CORE static float dotProduct(const vec4f& v1, const vec4f& v2);

	friend inline bool operator==(const vec4f &v1, const vec4f &v2);
	friend inline bool operator!=(const vec4f &v1, const vec4f &v2);
	friend inline const vec4f operator+(const vec4f &v1, const vec4f &v2);
	friend inline const vec4f operator-(const vec4f &v1, const vec4f &v2);
    friend inline const vec4f operator*(float factor, const vec4f &vector);
    friend inline const vec4f operator*(const vec4f &vector, float factor);
	friend inline const vec4f operator-(const vec4f &vector);
	friend inline const vec4f operator/(const vec4f &vector, float divisor);

	//! Dot product
	inline float operator * (const vec4f& vec) const
	{
		return m_X*vec.m_X + m_Y*vec.m_Y + m_Z*vec.m_Z + m_W*vec.m_W;
	}

	friend inline bool qFuzzyCompare(const vec4f& v1, const vec4f& v2);

    NEFEXP_CORE vec2f toVector2D() const;
    NEFEXP_CORE vec2f toVector2DAffine() const;

    NEFEXP_CORE vec3f toVector3D() const;
    NEFEXP_CORE vec3f toVector3DAffine() const;

    NEFEXP_CORE operator QVariant() const;

private:
	float m_X, m_Y, m_Z, m_W;

	inline vec4f(float xpos, float ypos, float zpos, float wpos, int dummy);

	friend class vec2f;
	friend class vec3f;
	friend inline QDebug operator<<(QDebug dbg, const vec4f &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec4f &);
	friend inline QDataStream &operator>>(QDataStream &, vec4f &);
};

Q_DECLARE_TYPEINFO(vec4f, Q_MOVABLE_TYPE);



inline bool operator==(const vec4f &v1, const vec4f &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y && v1.m_Z == v2.m_Z && v1.m_W == v2.m_W;
}

inline bool operator!=(const vec4f &v1, const vec4f &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y || v1.m_Z != v2.m_Z || v1.m_W != v2.m_W;
}

inline const vec4f operator+(const vec4f &v1, const vec4f &v2)
{
	return vec4f(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, v1.m_Z + v2.m_Z, v1.m_W + v2.m_W, 1);
}

inline const vec4f operator-(const vec4f &v1, const vec4f &v2)
{
	return vec4f(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, v1.m_Z - v2.m_Z, v1.m_W - v2.m_W, 1);
}

inline const vec4f operator*(float factor, const vec4f &vector)
{
	return vec4f(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, vector.m_W * factor, 1);
}

inline const vec4f operator*(const vec4f &vector, float factor)
{
	return vec4f(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, vector.m_W * factor, 1);
}

inline const vec4f operator-(const vec4f &vector)
{
	return vec4f(-vector.m_X, -vector.m_Y, -vector.m_Z, -vector.m_W, 1);
}

inline const vec4f operator/(const vec4f &vector, float divisor)
{
	return vec4f(vector.m_X / divisor, vector.m_Y / divisor, vector.m_Z / divisor, vector.m_W / divisor, 1);
}

inline bool qFuzzyCompare(const vec4f& v1, const vec4f& v2)
{
	return qFuzzyCompare(v1.m_X, v2.m_X) &&
		qFuzzyCompare(v1.m_Y, v2.m_Y) &&
		qFuzzyCompare(v1.m_Z, v2.m_Z) &&
		qFuzzyCompare(v1.m_W, v2.m_W);
}

QDebug operator<<(QDebug dbg, const vec4f &vector)
{
    dbg.nospace() << "vec4f("
        << vector.x() << ", " << vector.y() << ", "
        << vector.z() << ", " << vector.w() << ')';
    return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const vec4f &vector)
{
    stream << float(vector.x()) << float(vector.y())
           << float(vector.z()) << float(vector.w());
    return stream;
}

QDataStream &operator>>(QDataStream &stream, vec4f &vector)
{
    float x, y, z, w;
    stream >> x;
    stream >> y;
    stream >> z;
    stream >> w;
    vector.setX(float(x));
    vector.setY(float(y));
    vector.setZ(float(z));
    vector.setW(float(w));
    return stream;
}

Q_DECLARE_METATYPE(vec2f)
Q_DECLARE_METATYPE(vec3f)
Q_DECLARE_METATYPE(vec4f)

class vec2fWrapper : public QObject
{
    Q_OBJECT

public slots:
    vec2f* new_vec2f();
    vec2f* new_vec2f(const vec3f&  vector);
    vec2f* new_vec2f(const vec4f&  vector);
    vec2f* new_vec2f(float  xpos, float  ypos);
    vec2f* new_vec2f(const vec2f& other) {
    vec2f* a = new vec2f();
    *((vec2f*)a) = other;
    return a; }
    void delete_vec2f(vec2f* obj) { delete obj; }
    float  static_vec2f_dotProduct(const vec2f&  v1, const vec2f&  v2);
    float  length(vec2f* theWrappedObject) const;
    float  lengthSquared(vec2f* theWrappedObject) const;
    void normalize(vec2f* theWrappedObject);
    vec2f  normalized(vec2f* theWrappedObject) const;
    const vec2f  __mul__(vec2f* theWrappedObject, float  factor);
    vec2f*  __imul__(vec2f* theWrappedObject, const vec2f&  vector);
    vec2f*  __imul__(vec2f* theWrappedObject, float  factor);
    const vec2f  __add__(vec2f* theWrappedObject, const vec2f&  v2);
    vec2f*  __iadd__(vec2f* theWrappedObject, const vec2f&  vector);
    const vec2f  __sub__(vec2f* theWrappedObject, const vec2f&  v2);
    vec2f*  __isub__(vec2f* theWrappedObject, const vec2f&  vector);
    const vec2f  __div__(vec2f* theWrappedObject, float  divisor);
    vec2f*  __idiv__(vec2f* theWrappedObject, float  divisor);
    bool  __eq__(vec2f* theWrappedObject, const vec2f&  v2);
    void setX(vec2f* theWrappedObject, float  x);
    void setY(vec2f* theWrappedObject, float  y);
    vec3f  toVector3D(vec2f* theWrappedObject) const;
    vec4f  toVector4D(vec2f* theWrappedObject) const;
    float  x(vec2f* theWrappedObject) const;
    float  y(vec2f* theWrappedObject) const;

    QString py_toString(vec2f*);
};

class vec3fWrapper : public QObject
{
    Q_OBJECT

public slots:
	vec3f* new_vec3f();
	vec3f* new_vec3f(const vec2f&  vector);
	vec3f* new_vec3f(const vec2f&  vector, float  zpos);
	vec3f* new_vec3f(const vec4f&  vector);
	vec3f* new_vec3f(float  xpos, float  ypos, float  zpos);
	vec3f* new_vec3f(const vec3f& other) {
	vec3f* a = new vec3f();
	*((vec3f*)a) = other;
	return a; }
    void delete_vec3f(vec3f* obj) { delete obj; }
    vec3f  static_vec3f_crossProduct(const vec3f&  v1, const vec3f&  v2);
    float  distanceToLine(vec3f* theWrappedObject, const vec3f&  point, const vec3f&  direction) const;
    float  distanceToPlane(vec3f* theWrappedObject, const vec3f&  plane, const vec3f&  normal) const;
    float  distanceToPlane(vec3f* theWrappedObject, const vec3f&  plane1, const vec3f&  plane2, const vec3f&  plane3) const;
    float  static_vec3f_dotProduct(const vec3f&  v1, const vec3f&  v2);
    float  length(vec3f* theWrappedObject) const;
    float  lengthSquared(vec3f* theWrappedObject) const;
    vec3f  static_vec3f_normal(const vec3f&  v1, const vec3f&  v2);
    vec3f  static_vec3f_normal(const vec3f&  v1, const vec3f&  v2, const vec3f&  v3);
    void normalize(vec3f* theWrappedObject);
    vec3f  normalized(vec3f* theWrappedObject) const;
    vec3f  __mul__(vec3f* theWrappedObject, const matrix44f&  matrix);
    const vec3f  __mod__(vec3f* theWrappedObject, const vec3f&  v2);
    const vec3f  __mul__(vec3f* theWrappedObject, float  factor);
    vec3f*  __imul__(vec3f* theWrappedObject, const vec3f&  vector);
    vec3f*  __imul__(vec3f* theWrappedObject, float  factor);
    const vec3f  __add__(vec3f* theWrappedObject, const vec3f&  v2);
    vec3f*  __iadd__(vec3f* theWrappedObject, const vec3f&  vector);
    const vec3f  __sub__(vec3f* theWrappedObject, const vec3f&  v2);
    vec3f*  __isub__(vec3f* theWrappedObject, const vec3f&  vector);
    const vec3f  __div__(vec3f* theWrappedObject, float  divisor);
    vec3f*  __idiv__(vec3f* theWrappedObject, float  divisor);
    bool  __eq__(vec3f* theWrappedObject, const vec3f&  v2);
    void setX(vec3f* theWrappedObject, float  x);
    void setY(vec3f* theWrappedObject, float  y);
    void setZ(vec3f* theWrappedObject, float  z);
    vec2f  toVector2D(vec3f* theWrappedObject) const;
    vec4f  toVector4D(vec3f* theWrappedObject) const;
    float  x(vec3f* theWrappedObject) const;
    float  y(vec3f* theWrappedObject) const;
    float  z(vec3f* theWrappedObject) const;

    QString py_toString(vec3f*);
};

#include "Utility/Math/vec234f.inl"
