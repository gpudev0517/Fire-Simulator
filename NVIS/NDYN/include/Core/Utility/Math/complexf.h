#pragma once

class complexf
{
public:
	inline complexf();
	inline complexf(float real, float imag);
	inline complexf(const complexf& c);

	inline float real() const;
	inline float& real();
	inline float imag() const;
	inline float& imag();

	inline void setReal(float real);
	inline void setImag(float imag);
	inline void set(float real, float imag);
	inline void set(const complexf& c);

	inline complexf& operator = (const complexf& c);

    NEFEXP_CORE float length() const;
    NEFEXP_CORE float lengthSquared() const;
    NEFEXP_CORE complexf conjugate() const;
    NEFEXP_CORE complexf exp() const;

	friend inline bool operator==(const complexf &c1, const complexf &c2);
	friend inline bool operator!=(const complexf &c1, const complexf &c2);
	friend inline const complexf operator+(const complexf &c1, const complexf &c2);
	friend inline const complexf operator-(const complexf &c1, const complexf &c2);
	friend inline const complexf operator*(const complexf &c1, const complexf &c2);
	friend inline const complexf operator/(const complexf &c1, const complexf &c2);

private:
	float m_Real, m_Imag;

	friend inline QDebug operator<<(QDebug dbg, const complexf &c);
	friend inline QDataStream &operator<<(QDataStream &, const complexf &);
	friend inline QDataStream &operator>>(QDataStream &, complexf &);
};

Q_DECLARE_TYPEINFO(complexf, Q_MOVABLE_TYPE);


inline bool operator==(const complexf &c1, const complexf &c2)
{
	return c1.m_Real == c2.m_Real && c1.m_Imag == c2.m_Imag;
}

inline bool operator!=(const complexf &c1, const complexf &c2)
{
	return c1.m_Real != c2.m_Real || c1.m_Imag != c2.m_Imag;
}

inline const complexf operator+(const complexf &c1, const complexf &c2)
{
	return complexf(c1.m_Real + c2.m_Real, c1.m_Imag + c2.m_Imag);
}

inline const complexf operator-(const complexf &c1, const complexf &c2)
{
	return complexf(c1.m_Real - c2.m_Real, c1.m_Imag - c2.m_Imag);
}

inline const complexf operator*(const complexf &c1, const complexf &c2)
{
	float re = c1.m_Real * c2.m_Real - c1.m_Imag * c2.m_Imag;
	float im = c1.m_Real * c2.m_Imag + c1.m_Imag * c2.m_Real;
	return complexf(re,im);
}

inline const complexf operator/( const complexf &c1, const complexf &c2 )
{
	float c2sqr = c2.m_Real * c2.m_Real + c2.m_Imag * c2.m_Imag;
	float re = (c1.m_Real * c2.m_Real + c1.m_Imag * c2.m_Imag) / c2sqr;
	float im = (c1.m_Imag * c2.m_Real - c1.m_Real * c2.m_Imag) / c2sqr;
	return complexf(re,im);
}

QDebug operator<<(QDebug dbg, const complexf &c)
{
	dbg.nospace() << "complexf(" << c.m_Real << ", " << c.m_Real << ')';
	return dbg.space();
}

QDataStream& operator>>(QDataStream& stream, complexf &c)
{
	float x, y;
	stream >> x;
	stream >> y;
	c.setReal(float(x));
	c.setImag(float(y));
	return stream;
}

#include"Utility/Math/complexf.inl"


