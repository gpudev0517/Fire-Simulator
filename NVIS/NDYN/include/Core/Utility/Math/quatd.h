#pragma once

class matrix44d;
class QVariant;
class vec3d;
class vec4d;

class quatd
{
public:
	inline quatd();
	inline quatd(double scalar, double x, double y, double z);
    NEFEXP_CORE quatd(double scalar, const vec3d& vector);
    NEFEXP_CORE explicit quatd(const vec4d& vector);

	inline bool isNull() const;
	inline bool isIdentity() const;

    NEFEXP_CORE vec3d vector() const;
    NEFEXP_CORE void setVector(const vec3d& vector);
	inline void setVector(double x, double y, double z);

	inline double x() const;
	inline double y() const;
	inline double z() const;
	inline double scalar() const;

	inline void setX(double x);
	inline void setY(double y);
	inline void setZ(double z);
	inline void setScalar(double scalar);

    NEFEXP_CORE double length() const;
    NEFEXP_CORE double lengthSquared() const;

    NEFEXP_CORE quatd normalized() const;
    NEFEXP_CORE void normalize();

	inline quatd conjugate() const;

    NEFEXP_CORE vec3d rotatedVector(const vec3d& vector) const;

	inline quatd& operator+=(const quatd& quat);
	inline quatd& operator-=(const quatd& quat);
	inline quatd& operator*=(double factor);
	inline quatd& operator*=(const quatd& quat);
	inline quatd& operator/=(double divisor);

	friend inline bool operator==(const quatd& q1, const quatd& q2);
	friend inline bool operator!=(const quatd& q1, const quatd& q2);
	friend inline const quatd operator+(const quatd& q1, const quatd& q2);
	friend inline const quatd operator-(const quatd& q1, const quatd& q2);
	friend inline const quatd operator*(double factor, const quatd& quat);
	friend inline const quatd operator*(const quatd& quat, double factor);
	friend inline const quatd operator*(const quatd& q1, const quatd& q2);
	friend inline const quatd operator-(const quatd& quat);
	friend inline const quatd operator/(const quatd& quat, double divisor);

	friend inline bool qFuzzyCompare(const quatd& q1, const quatd& q2);
	friend inline QDebug operator<<(QDebug dbg, const quatd& q);
	friend inline QDataStream &operator<<(QDataStream&, const quatd&);
	friend inline QDataStream &operator>>(QDataStream&, quatd&);

    NEFEXP_CORE vec4d toVector4D() const;

    NEFEXP_CORE operator QVariant() const;

    NEFEXP_CORE static quatd fromAxisAndAngle(const vec3d& axis, double angle);
    NEFEXP_CORE static quatd fromAxisAndAngle(double x, double y, double z, double angle);

    NEFEXP_CORE static quatd slerp(const quatd& q1, const quatd& q2, double t);
    NEFEXP_CORE static quatd nlerp(const quatd& q1, const quatd& q2, double t);

private:
	double m_W, m_X, m_Y, m_Z;
};

Q_DECLARE_TYPEINFO(quatd, Q_MOVABLE_TYPE);

inline const quatd operator*(const quatd& q1, const quatd& q2)
{
	double ww = (q1.m_Z + q1.m_X) * (q2.m_X + q2.m_Y);
	double yy = (q1.m_W - q1.m_Y) * (q2.m_W + q2.m_Z);
	double zz = (q1.m_W + q1.m_Y) * (q2.m_W - q2.m_Z);
	double xx = ww + yy + zz;
	double qq = 0.5 * (xx + (q1.m_Z - q1.m_X) * (q2.m_X - q2.m_Y));

	double w = qq - ww + (q1.m_Z - q1.m_Y) * (q2.m_Y - q2.m_Z);
	double x = qq - xx + (q1.m_X + q1.m_W) * (q2.m_X + q2.m_W);
	double y = qq - yy + (q1.m_W - q1.m_X) * (q2.m_Y + q2.m_Z);
	double z = qq - zz + (q1.m_Z + q1.m_Y) * (q2.m_W - q2.m_X);

	return quatd(w, x, y, z);
}



inline bool operator==(const quatd& q1, const quatd& q2)
{
	return q1.m_X == q2.m_X && q1.m_Y == q2.m_Y && q1.m_Z == q2.m_Z && q1.m_W == q2.m_W;
}

inline bool operator!=(const quatd& q1, const quatd& q2)
{
	return q1.m_X != q2.m_X || q1.m_Y != q2.m_Y || q1.m_Z != q2.m_Z || q1.m_W != q2.m_W;
}

inline const quatd operator+(const quatd& q1, const quatd& q2)
{
	return quatd(q1.m_W + q2.m_W, q1.m_X + q2.m_X, q1.m_Y + q2.m_Y, q1.m_Z + q2.m_Z);
}

inline const quatd operator-(const quatd& q1, const quatd& q2)
{
	return quatd(q1.m_W - q2.m_W, q1.m_X - q2.m_X, q1.m_Y - q2.m_Y, q1.m_Z - q2.m_Z);
}

inline const quatd operator*(double factor, const quatd& quat)
{
	return quatd(quat.m_W * factor, quat.m_X * factor, quat.m_Y * factor, quat.m_Z * factor);
}

inline const quatd operator*(const quatd& quat, double factor)
{
	return quatd(quat.m_W * factor, quat.m_X * factor, quat.m_Y * factor, quat.m_Z * factor);
}

inline const quatd operator-(const quatd& quat)
{
	return quatd(-quat.m_W, -quat.m_X, -quat.m_Y, -quat.m_Z);
}

inline const quatd operator/(const quatd& quat, double divisor)
{
	return quatd(quat.m_W / divisor, quat.m_X / divisor, quat.m_Y / divisor, quat.m_Z / divisor);
}

inline bool qFuzzyCompare(const quatd& q1, const quatd& q2)
{
	return qFuzzyCompare(q1.m_X, q2.m_X) &&
		qFuzzyCompare(q1.m_Y, q2.m_Y) &&
		qFuzzyCompare(q1.m_Z, q2.m_Z) &&
		qFuzzyCompare(q1.m_W, q2.m_W);
}

QDebug operator<<(QDebug dbg, const quatd& q)
{
    dbg.nospace() << "quatd(scalar:" << q.scalar()
        << ", vector:(" << q.x() << ", "
        << q.y() << ", " << q.z() << "))";
    return dbg.space();
}

QDataStream &operator<<(QDataStream& stream, const quatd& quat)
{
    stream << double(quat.scalar()) << double(quat.x())
           << double(quat.y()) << double(quat.z());
    return stream;
}

QDataStream &operator>>(QDataStream& stream, quatd& quat)
{
    double scalar, x, y, z;
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

#include "Utility/Math/quatd.inl"
