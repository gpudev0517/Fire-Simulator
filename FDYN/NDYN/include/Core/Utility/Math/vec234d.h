#pragma once

class vec2f;
class vec3f;
class vec4f;
class vec3d;
class vec4d;
class QVariant;

class vec2d
{
public:
	inline vec2d();
    inline vec2d(double val);
	inline vec2d(double xpos, double ypos);
	inline vec2d(const vec2d& vec);
    NEFEXP_CORE vec2d(const vec2f& vec);
    NEFEXP_CORE explicit vec2d(const vec3d& vector);
    NEFEXP_CORE explicit vec2d(const vec4d& vector);

	inline double x() const;
	inline double& x();
	inline double y() const;
	inline double& y();

	inline void setX(double x);
	inline void setY(double y);
	inline void set(double x, double y);
	inline void set(const vec2d& vec);

    NEFEXP_CORE double length() const;
    NEFEXP_CORE double lengthSquared() const;

    NEFEXP_CORE vec2d normalized() const;
    NEFEXP_CORE void normalize();

	inline vec2d& operator+=(const vec2d &vector);
    inline vec2d& operator-=(const vec2d &vector);
    inline vec2d& operator*=(double factor);
	inline vec2d& operator*=(const vec2d &vector);
    inline vec2d& operator/=(double divisor);
	inline vec2d& operator = (const vec2d& vec);
    NEFEXP_CORE vec2d& operator = (const vec2f& vec);

	inline double operator *(const vec2d& m) const;

    NEFEXP_CORE static double dotProduct(const vec2d& v1, const vec2d& v2);

	friend inline bool operator==(const vec2d &v1, const vec2d &v2);
	friend inline bool operator!=(const vec2d &v1, const vec2d &v2);
	friend inline const vec2d operator+(const vec2d &v1, const vec2d &v2);
	friend inline const vec2d operator-(const vec2d &v1, const vec2d &v2);
	friend inline const vec2d operator*(double factor, const vec2d &vector);
	friend inline const vec2d operator*(const vec2d &vector, double factor);
	friend inline const vec2d operator*(const vec2d &v1, const vec2d &v2);
	friend inline const vec2d operator-(const vec2d &vector);
	friend inline const vec2d operator/(const vec2d &vector, double divisor);

	friend inline bool qFuzzyCompare(const vec2d& v1, const vec2d& v2);
	friend inline double distTwovectors(const vec2d& v1, const vec2d& v2);
	friend inline double distTwovectorsSquare(const vec2d& v1, const vec2d& v2);

    NEFEXP_CORE vec3d toVector3D() const;
    NEFEXP_CORE vec4d toVector4D() const;

    NEFEXP_CORE operator QVariant() const;

private:
	double m_X, m_Y;

	inline vec2d(double xpos, double ypos, int dummy);

	friend class vec3d;
	friend class vec4d;
	friend inline QDebug operator<<(QDebug dbg, const vec2d &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec2d &);
	friend inline QDataStream &operator>>(QDataStream &, vec2d &);
};

Q_DECLARE_TYPEINFO(vec2d, Q_MOVABLE_TYPE);


inline bool operator==(const vec2d &v1, const vec2d &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y;
}

inline bool operator!=(const vec2d &v1, const vec2d &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y;
}

inline const vec2d operator+(const vec2d &v1, const vec2d &v2)
{
	return vec2d(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, 1);
}

inline const vec2d operator-(const vec2d &v1, const vec2d &v2)
{
	return vec2d(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, 1);
}

inline const vec2d operator*(double factor, const vec2d &vector)
{
	return vec2d(vector.m_X * factor, vector.m_Y * factor, 1);
}

inline const vec2d operator*(const vec2d &vector, double factor)
{
	return vec2d(vector.m_X * factor, vector.m_Y * factor, 1);
}

inline const vec2d operator*(const vec2d &v1, const vec2d &v2)
{
	return vec2d(v1.m_X * v2.m_X, v1.m_Y * v2.m_Y, 1);
}

inline const vec2d operator-(const vec2d &vector)
{
	return vec2d(-vector.m_X, -vector.m_Y, 1);
}

inline const vec2d operator/(const vec2d &vector, double divisor)
{
	return vec2d(vector.m_X / divisor, vector.m_Y / divisor, 1);
}

inline bool qFuzzyCompare(const vec2d& v1, const vec2d& v2)
{
	return qFuzzyCompare(v1.m_X, v2.m_X) && qFuzzyCompare(v1.m_Y, v2.m_Y);
}

inline double distTwovectors(const vec2d& v1, const vec2d& v2)
{
	double dx = v1.m_X - v2.m_X;
	double dy = v1.m_Y - v2.m_Y;
	return sqrt(dx*dx + dy*dy);
}


inline double distTwovectorsSquare(const vec2d& v1, const vec2d& v2)
{
	double dx = v1.m_X - v2.m_X;
	double dy = v1.m_Y - v2.m_Y;
	return (dx*dx + dy*dy);
}

QDebug operator<<(QDebug dbg, const vec2d &vector)
{
	dbg.nospace() << "vec2d(" << vector.x() << ", " << vector.y() << ')';
	return dbg.space();
}

QDataStream& operator>>(QDataStream& stream, vec2d &vector)
{
	double x, y;
	stream >> x;
	stream >> y;
	vector.setX(double(x));
	vector.setY(double(y));
	return stream;
}


class matrix44d;
class vec4d;
class vec3f;

class vec3d
{
public:
	inline vec3d();
    inline vec3d(double val);
    inline vec3d(float xpos, float ypos, float zpos);
	inline vec3d(double xpos, double ypos, double zpos);
    NEFEXP_CORE vec3d(const vec2d& vector);
    NEFEXP_CORE vec3d(const vec2d& vector, double zpos);
    NEFEXP_CORE explicit vec3d(const vec4d& vector);
    NEFEXP_CORE explicit vec3d(const vec3f& vector);

	inline double x() const;
	inline double& x();
	inline double y() const;
	inline double& y();
	inline double z() const;
	inline double& z();

	inline void setX(double x);
	inline void setY(double y);
	inline void setZ(double z);
	inline void set(double x, double y, double z);
	inline void set(const vec3d& vec);
    NEFEXP_CORE void set(const vec3f& vec);

    NEFEXP_CORE double length() const;
    NEFEXP_CORE double lengthSquared() const;

    NEFEXP_CORE vec3d normalized() const;
    NEFEXP_CORE vec3d normalizedWithoutZeroApproximation() const;
    NEFEXP_CORE void normalize();

	inline vec3d &operator+=(const vec3d &vector);
    inline vec3d &operator+=(const vec3f &vector);
    inline vec3d &operator-=(const vec3d &vector);
    inline vec3d &operator-=(const vec3f &vector);
    inline vec3d &operator*=(double factor);
	inline vec3d &operator*=(const vec3d& vector);
    inline vec3d &operator*=(const vec3f& vector);
    inline vec3d &operator/=(double divisor);
	inline vec3d &operator=(const vec3d& vector);
    vec3d &operator=(const vec3f& vector);

    NEFEXP_CORE static double dotProduct(const vec3d& v1, const vec3d& v2);
    NEFEXP_CORE static vec3d crossProduct(const vec3d& v1, const vec3d& v2);
    NEFEXP_CORE static vec3d normal(const vec3d& v1, const vec3d& v2);
    NEFEXP_CORE static vec3d normal(const vec3d& v1, const vec3d& v2, const vec3d& v3);

    NEFEXP_CORE double distanceToPlane(const vec3d& plane, const vec3d& normal) const;
    NEFEXP_CORE double distanceToPlane(const vec3d& plane1, const vec3d& plane2, const vec3d& plane3) const;
    NEFEXP_CORE double distanceToLine(const vec3d& point, const vec3d& direction) const;

	friend inline bool operator==(const vec3d &v1, const vec3d &v2);
	friend inline bool operator!=(const vec3d &v1, const vec3d &v2);
	friend inline const vec3d operator+(const vec3d &v1, const vec3d &v2);
	friend inline const vec3d operator-(const vec3d &v1, const vec3d &v2);
	friend inline const vec3d operator*(double factor, const vec3d &vector);
	friend inline const vec3d operator*(const vec3d &vector, double factor);
	friend inline const vec3d operator-(const vec3d &vector);
	friend inline const vec3d operator/(const vec3d &vector, double divisor);

	inline double& operator [] (uint i) { return (&m_X)[i]; }
	inline double operator [] (uint i) const { return (&m_X)[i]; }

	//! Dot product
	inline double operator * (const vec3d& vec) const
	{
		return m_X*vec.m_X + m_Y*vec.m_Y + m_Z*vec.m_Z;
	}
	//! Cross product
	inline vec3d operator % (const vec3d& vec) const
	{
		return vec3d(m_Y*vec.m_Z - m_Z*vec.m_Y, m_Z*vec.m_X - m_X*vec.m_Z, m_X*vec.m_Y - m_Y*vec.m_X);
	}

	friend inline bool qFuzzyCompare(const vec3d& v1, const vec3d& v2);
	friend inline double distOfTwoVectors(const vec3d& v1, const vec3d& v2);
	friend inline double distOfTwoVectorsSquare(const vec3d& v1, const vec3d& v2);

    NEFEXP_CORE vec2d toVector2D() const;
    NEFEXP_CORE vec4d toVector4D() const;

    NEFEXP_CORE operator QVariant() const;

	// Static Variables
	static vec3d xAxis;
	static vec3d yAxis;
	static vec3d zAxis;
	static vec3d origin;

private:
	double m_X, m_Y, m_Z;

	vec3d(double xpos, double ypos, double zpos, int dummy);

	friend class vec2d;
	friend class vec4d;
    NEFEXP_CORE friend vec3d operator*(const matrix44d& matrix, const vec3d& vector);
	friend inline QDebug operator<<(QDebug dbg, const vec3d &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec3d &);
	friend inline QDataStream &operator>>(QDataStream &, vec3d &);

};

Q_DECLARE_TYPEINFO(vec3d, Q_MOVABLE_TYPE);


inline bool operator==(const vec3d &v1, const vec3d &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y && v1.m_Z == v2.m_Z;
}

inline bool operator!=(const vec3d &v1, const vec3d &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y || v1.m_Z != v2.m_Z;
}

inline const vec3d operator+(const vec3d &v1, const vec3d &v2)
{
	return vec3d(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, v1.m_Z + v2.m_Z, 1);
}

inline const vec3d operator-(const vec3d &v1, const vec3d &v2)
{
	return vec3d(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, v1.m_Z - v2.m_Z, 1);
}

inline const vec3d operator*(double factor, const vec3d &vector)
{
	return vec3d(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, 1);
}

inline const vec3d operator*(const vec3d &vector, double factor)
{
	return vec3d(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, 1);
}

inline const vec3d operator-(const vec3d &vector)
{
	return vec3d(-vector.m_X, -vector.m_Y, -vector.m_Z, 1);
}

inline const vec3d operator/(const vec3d &vector, double divisor)
{
	return vec3d(vector.m_X / divisor, vector.m_Y / divisor, vector.m_Z / divisor, 1);
}

inline bool qFuzzyCompare(const vec3d& v1, const vec3d& v2)
{
	return qFuzzyCompare(v1.m_X, v2.m_X) &&
		qFuzzyCompare(v1.m_Y, v2.m_Y) &&
		qFuzzyCompare(v1.m_Z, v2.m_Z);
}

inline double distOfTwoVectors(const vec3d& v1, const vec3d& v2)
{
	double dx = v1.m_X - v2.m_X;
	double dy = v1.m_Y - v2.m_Y;
	double dz = v1.m_Z - v2.m_Z;
	return sqrt(dx*dx + dy*dy + dz*dz);
}


inline double distOfTwoVectorsSquare(const vec3d& v1, const vec3d& v2)
{
	double dx = v1.m_X - v2.m_X;
	double dy = v1.m_Y - v2.m_Y;
	double dz = v1.m_Z - v2.m_Z;
	return (dx*dx + dy*dy + dz*dz);

}
QDebug operator<<(QDebug dbg, const vec3d &vector)
{
	dbg.nospace() << "vec3d(" << vector.x() << ", " << vector.y() << ", " << vector.z() << ')';
	return dbg.space();
}

QDataStream& operator>>(QDataStream& stream, vec3d &vector)
{
	double x, y, z;
	stream >> x;
	stream >> y;
	stream >> z;
	vector.setX(double(x));
	vector.setY(double(y));
	vector.setZ(double(z));
	return stream;
}

class matrix44d;

class vec4d
{
public:
	inline vec4d();
    inline vec4d(double val);
	inline vec4d(double xpos, double ypos, double zpos, double wpos);
    NEFEXP_CORE vec4d(const vec2d& vector);
    NEFEXP_CORE vec4d(const vec2d& vector, double zpos, double wpos);
    NEFEXP_CORE vec4d(const vec3d& vector);
    NEFEXP_CORE vec4d(const vec3d& vector, double wpos);

	inline double x() const;
	inline double& x();
	inline double y() const;
	inline double& y();
	inline double z() const;
	inline double& z();
	inline double w() const;
	inline double& w();

	inline void setX(double x);
	inline void setY(double y);
	inline void setZ(double z);
	inline void setW(double w);
	inline void set(double x, double y, double z, double w);
	inline void set(const vec4d& vec);
	void set(const vec4f& vec);

    NEFEXP_CORE double length() const;
    NEFEXP_CORE double lengthSquared() const;

    NEFEXP_CORE vec4d normalized() const;
    NEFEXP_CORE void normalize();

	inline vec4d &operator+=(const vec4d &vector);
	inline vec4d &operator-=(const vec4d &vector);
	inline vec4d &operator*=(double factor);
	inline vec4d &operator*=(const vec4d &vector);
	inline vec4d &operator/=(double divisor);
	inline vec4d &operator=(const vec4d &vector);
	vec4d &operator=(const vec4f &vector);

    NEFEXP_CORE static double dotProduct(const vec4d& v1, const vec4d& v2);

	friend inline bool operator==(const vec4d &v1, const vec4d &v2);
	friend inline bool operator!=(const vec4d &v1, const vec4d &v2);
	friend inline const vec4d operator+(const vec4d &v1, const vec4d &v2);
	friend inline const vec4d operator-(const vec4d &v1, const vec4d &v2);
	friend inline const vec4d operator*(double factor, const vec4d &vector);
	friend inline const vec4d operator*(const vec4d &vector, double factor);
	friend inline const vec4d operator-(const vec4d &vector);
	friend inline const vec4d operator/(const vec4d &vector, double divisor);

	//! Dot product
	inline double operator * (const vec4d& vec) const
	{
		return m_X*vec.m_X + m_Y*vec.m_Y + m_Z*vec.m_Z + m_W*vec.m_W;
	}

	friend inline bool qFuzzyCompare(const vec4d& v1, const vec4d& v2);

    NEFEXP_CORE vec2d toVector2D() const;
    NEFEXP_CORE vec2d toVector2DAffine() const;

    NEFEXP_CORE vec3d toVector3D() const;
    NEFEXP_CORE vec3d toVector3DAffine() const;

    NEFEXP_CORE operator QVariant() const;

private:
	double m_X, m_Y, m_Z, m_W;

	inline vec4d(double xpos, double ypos, double zpos, double wpos, int dummy);

	friend class vec2d;
	friend class vec3d;
	friend inline QDebug operator<<(QDebug dbg, const vec4d &vector);
	friend inline QDataStream &operator<<(QDataStream &, const vec4d &);
	friend inline QDataStream &operator>>(QDataStream &, vec4d &);
};

Q_DECLARE_TYPEINFO(vec4d, Q_MOVABLE_TYPE);


inline bool operator==(const vec4d &v1, const vec4d &v2)
{
	return v1.m_X == v2.m_X && v1.m_Y == v2.m_Y && v1.m_Z == v2.m_Z && v1.m_W == v2.m_W;
}

inline bool operator!=(const vec4d &v1, const vec4d &v2)
{
	return v1.m_X != v2.m_X || v1.m_Y != v2.m_Y || v1.m_Z != v2.m_Z || v1.m_W != v2.m_W;
}

inline const vec4d operator+(const vec4d &v1, const vec4d &v2)
{
	return vec4d(v1.m_X + v2.m_X, v1.m_Y + v2.m_Y, v1.m_Z + v2.m_Z, v1.m_W + v2.m_W, 1);
}

inline const vec4d operator-(const vec4d &v1, const vec4d &v2)
{
	return vec4d(v1.m_X - v2.m_X, v1.m_Y - v2.m_Y, v1.m_Z - v2.m_Z, v1.m_W - v2.m_W, 1);
}

inline const vec4d operator*(double factor, const vec4d &vector)
{
	return vec4d(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, vector.m_W * factor, 1);
}

inline const vec4d operator*(const vec4d &vector, double factor)
{
	return vec4d(vector.m_X * factor, vector.m_Y * factor, vector.m_Z * factor, vector.m_W * factor, 1);
}

inline const vec4d operator-(const vec4d &vector)
{
	return vec4d(-vector.m_X, -vector.m_Y, -vector.m_Z, -vector.m_W, 1);
}

inline const vec4d operator/(const vec4d &vector, double divisor)
{
	return vec4d(vector.m_X / divisor, vector.m_Y / divisor, vector.m_Z / divisor, vector.m_W / divisor, 1);
}

inline bool qFuzzyCompare(const vec4d& v1, const vec4d& v2)
{
	return qFuzzyCompare(v1.m_X, v2.m_X) &&
		qFuzzyCompare(v1.m_Y, v2.m_Y) &&
		qFuzzyCompare(v1.m_Z, v2.m_Z) &&
		qFuzzyCompare(v1.m_W, v2.m_W);
}

QDebug operator<<(QDebug dbg, const vec4d &vector)
{
	dbg.nospace() << "vec4d("
		<< vector.x() << ", " << vector.y() << ", "
		<< vector.z() << ", " << vector.w() << ')';
	return dbg.space();
}

QDataStream &operator<<(QDataStream &stream, const vec4d &vector)
{
	stream << double(vector.x()) << double(vector.y())
		<< double(vector.z()) << double(vector.w());
	return stream;
}

QDataStream &operator>>(QDataStream &stream, vec4d &vector)
{
	double x, y, z, w;
	stream >> x;
	stream >> y;
	stream >> z;
	stream >> w;
	vector.setX(double(x));
	vector.setY(double(y));
	vector.setZ(double(z));
	vector.setW(double(w));
	return stream;
}



Q_DECLARE_METATYPE(vec2d)
Q_DECLARE_METATYPE(vec3d)
Q_DECLARE_METATYPE(vec4d)

class vec3dWrapper : public QObject
{
	Q_OBJECT

		public slots:
			vec3d* new_vec3d(double x, double y, double z)
			{
				return new vec3d(x,y,z);
			}

			double x(vec3d* o) { return o->x(); }
			double y(vec3d* o) { return o->y(); }
			double z(vec3d* o) { return o->z(); }
			QString py_toString(vec3d* o)
			{
				QString result;
				QDebug d(&result);
				d << *o;
				return result;
			}
};

#include "Utility/Math/vec234d.inl"
