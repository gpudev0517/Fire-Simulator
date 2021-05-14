#pragma once
class vec3f;
class eulerfWrapper;

class eulerf
{
	friend class eulerfWrapper;
public:
	inline eulerf();
	inline eulerf(float phi, float theta, float psi);
	inline eulerf(const vec3f& vec);


	inline float phi() const;
	inline float& phi();
	inline float theta() const;
	inline float& theta();
	inline float psi() const;
	inline float& psi();

	inline void setPhi(float phi);
	inline void setTheta(float theta);
	inline void setPsi(float psi);
	inline void set(float phi, float theta, float psi);
	inline void set(const vec3f& vec);

    NEFEXP_CORE vec3f toVector3D() const;
    NEFEXP_CORE operator QVariant() const;

	friend inline bool operator==(const eulerf &v1, const eulerf &v2);
	friend inline const eulerf operator+(const eulerf &v1, const eulerf &v2);
    friend inline const eulerf operator-(const eulerf &v1, const eulerf &v2);

private:
	float m_phi, m_theta, m_psi;
};

Q_DECLARE_METATYPE(eulerf)

	class eulerfWrapper : public QObject
{
	Q_OBJECT

public slots:
	eulerf* new_eulerf(float x, float y, float z)
	{
		return new eulerf(x,y,z);
	}

	float x(eulerf* o) { return o->m_phi; }
	float y(eulerf* o) { return o->m_theta; }
	float z(eulerf* o) { return o->m_psi; }

	void setX(eulerf* o, float val) { o->m_phi = val; }
	void setY(eulerf* o, float val) { o->m_theta = val; }
	void setZ(eulerf* o, float val) { o->m_psi = val; }

	QString py_toString(eulerf* o)
	{
		QString result;
		QDebug d(&result);
		d << *o;
		return result;
	}
};

#include "Utility/Math/eulerf.inl"
