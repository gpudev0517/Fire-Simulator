#pragma once

class matrix44f;
class matrix33f;
class QVariant;
class vec3f;
class vec4f;
class quatfWrapper;

class quatf
{

	friend class quatfWrapper;
public:
	inline quatf();
	inline quatf(float scalar, float xpos, float ypos, float zpos);
    NEFEXP_CORE quatf(float scalar, const vec3f& vector);
    NEFEXP_CORE explicit quatf(const vec4f& vector);

	inline bool isNull() const;
	inline bool isIdentity() const;

    NEFEXP_CORE vec3f vector() const;
    NEFEXP_CORE void setVector(const vec3f& vector);
	inline void setVector(float x, float y, float z);

	inline float x() const;
	inline float y() const;
	inline float z() const;
	inline float scalar() const;

	inline float& x() { return m_X; }
	inline float& y() { return m_Y; }
	inline float& z() { return m_Z; }
	inline float& scalar() { return m_W; }

	inline void setX(float x);
	inline void setY(float y);
	inline void setZ(float z);
	inline void setScalar(float scalar);

    NEFEXP_CORE float length() const;
    NEFEXP_CORE float lengthSquared() const;

    NEFEXP_CORE quatf normalized() const;
    NEFEXP_CORE void normalize();

	inline quatf conjugate() const;

    NEFEXP_CORE vec3f rotatedVector(const vec3f& vector) const;

	inline quatf& operator+=(const quatf& quat);
	inline quatf& operator-=(const quatf& quat);
	inline quatf& operator*=(float factor);
	inline quatf& operator*=(const quatf& quat);
	inline quatf& operator/=(float divisor);

	friend inline bool operator==(const quatf& q1, const quatf& q2);
	friend inline bool operator!=(const quatf& q1, const quatf& q2);
	friend inline const quatf operator+(const quatf& q1, const quatf& q2);
	friend inline const quatf operator-(const quatf& q1, const quatf& q2);
	friend inline const quatf operator*(float factor, const quatf& quat);
	friend inline const quatf operator*(const quatf& quat, float factor);
	friend inline const quatf operator*(const quatf& q1, const quatf& q2);
	friend inline const quatf operator-(const quatf& quat);
	friend inline const quatf operator/(const quatf& quat, float divisor);

	friend inline bool qFuzzyCompare(const quatf& q1, const quatf& q2);
	friend inline QDebug operator<<(QDebug dbg, const quatf& q);
	friend inline QDataStream &operator<<(QDataStream&, const quatf&);
	friend inline QDataStream &operator>>(QDataStream&, quatf&);

    NEFEXP_CORE vec4f toVector4D() const;

    //
    //! Convert quaternion to rotation matrix
    //
    NEFEXP_CORE matrix44f toMatrix() const;

    NEFEXP_CORE operator QVariant() const;

    NEFEXP_CORE static quatf fromAxisAndAngle(const vec3f& axis, float angle);
    NEFEXP_CORE static quatf fromAxisAndAngle(float x, float y, float z, float angle);

    NEFEXP_CORE static quatf fromRotMatrix( matrix33f& rotMat );

    NEFEXP_CORE static quatf slerp(const quatf& q1, const quatf& q2, float t);
    NEFEXP_CORE static quatf nlerp(const quatf& q1, const quatf& q2, float t);

private:
	float m_W, m_X, m_Y, m_Z;
};

Q_DECLARE_TYPEINFO(quatf, Q_MOVABLE_TYPE);

inline const quatf operator*(const quatf& q1, const quatf& q2)
{
	float ww = (q1.m_Z + q1.m_X) * (q2.m_X + q2.m_Y);
	float yy = (q1.m_W - q1.m_Y) * (q2.m_W + q2.m_Z);
	float zz = (q1.m_W + q1.m_Y) * (q2.m_W - q2.m_Z);
	float xx = ww + yy + zz;
    float qq = 0.5f * (xx + (q1.m_Z - q1.m_X) * (q2.m_X - q2.m_Y));

	float w = qq - ww + (q1.m_Z - q1.m_Y) * (q2.m_Y - q2.m_Z);
	float x = qq - xx + (q1.m_X + q1.m_W) * (q2.m_X + q2.m_W);
	float y = qq - yy + (q1.m_W - q1.m_X) * (q2.m_Y + q2.m_Z);
	float z = qq - zz + (q1.m_Z + q1.m_Y) * (q2.m_W - q2.m_X);

	return quatf(w, x, y, z);
}

inline bool operator==(const quatf& q1, const quatf& q2)
{
	return q1.m_X == q2.m_X && q1.m_Y == q2.m_Y && q1.m_Z == q2.m_Z && q1.m_W == q2.m_W;
}

inline bool operator!=(const quatf& q1, const quatf& q2)
{
	return q1.m_X != q2.m_X || q1.m_Y != q2.m_Y || q1.m_Z != q2.m_Z || q1.m_W != q2.m_W;
}

inline const quatf operator+(const quatf& q1, const quatf& q2)
{
	return quatf(q1.m_W + q2.m_W, q1.m_X + q2.m_X, q1.m_Y + q2.m_Y, q1.m_Z + q2.m_Z);
}

inline const quatf operator-(const quatf& q1, const quatf& q2)
{
	return quatf(q1.m_W - q2.m_W, q1.m_X - q2.m_X, q1.m_Y - q2.m_Y, q1.m_Z - q2.m_Z);
}

inline const quatf operator*(float factor, const quatf& quat)
{
	return quatf(quat.m_W * factor, quat.m_X * factor, quat.m_Y * factor, quat.m_Z * factor);
}

inline const quatf operator*(const quatf& quat, float factor)
{
	return quatf(quat.m_W * factor, quat.m_X * factor, quat.m_Y * factor, quat.m_Z * factor);
}

inline const quatf operator-(const quatf& quat)
{
	return quatf(-quat.m_W, -quat.m_X, -quat.m_Y, -quat.m_Z);
}

inline const quatf operator/(const quatf& quat, float divisor)
{
	return quatf(quat.m_W / divisor, quat.m_X / divisor, quat.m_Y / divisor, quat.m_Z / divisor);
}

inline bool qFuzzyCompare(const quatf& q1, const quatf& q2)
{
	return qFuzzyCompare(q1.m_X, q2.m_X) &&
		qFuzzyCompare(q1.m_Y, q2.m_Y) &&
		qFuzzyCompare(q1.m_Z, q2.m_Z) &&
		qFuzzyCompare(q1.m_W, q2.m_W);
}

QDebug operator<<(QDebug dbg, const quatf& q)
{
    dbg.nospace() << "quatf(scalar:" << q.scalar()
        << ", vector:(" << q.x() << ", "
        << q.y() << ", " << q.z() << "))";
    return dbg.space();
}

QDataStream &operator<<(QDataStream& stream, const quatf& quat)
{
    stream << float(quat.scalar()) << float(quat.x())
           << float(quat.y()) << float(quat.z());
    return stream;
}

QDataStream &operator>>(QDataStream& stream, quatf& quat)
{
    float scalar, x, y, z;
    stream >> scalar;
    stream >> x;
    stream >> y;
    stream >> z;
    quat.setScalar(scalar);
    quat.setX(x);
    quat.setY(y);
    quat.setZ(z);
    return stream;
}

Q_DECLARE_METATYPE(quatf)

class quatfWrapper : public QObject
{
	Q_OBJECT

public slots:
	quatf* new_quatf(float x, float y, float z, float scalar)
	{
		return new quatf(x,y,z,scalar);
	}

	float x(quatf* o) { return o->m_X; }
	float y(quatf* o) { return o->m_Y; }
	float z(quatf* o) { return o->m_Z; }
	float scalar(quatf* o) { return o->m_W; }


	void setX(quatf* o, float val) { o->m_X = val; }
	void setY(quatf* o, float val) { o->m_Y = val; }
	void setZ(quatf* o, float val) { o->m_Z = val; }
	void setScalar(quatf* o, float val) { o->m_W = val; }

	QString py_toString(quatf* o)
	{
		QString result;
		QDebug d(&result);
		d << *o;
		return result;
	}
};

#include "Utility/Math/quatf.inl"
