inline eulerf::eulerf() : m_phi(0.0f), m_theta(0.0f), m_psi(0.0f) {}
inline eulerf::eulerf(float phi, float theta, float psi) : m_phi(phi), m_theta(theta), m_psi(psi) {}
inline eulerf::eulerf(const vec3f& vector) : m_phi(vector.x()), m_theta(vector.y()), m_psi(vector.z()) {}

inline float eulerf::phi() const { return m_phi; }
inline float& eulerf::phi() { return m_phi; }
inline float eulerf::theta() const { return m_theta; }
inline float& eulerf::theta() { return m_theta; }
inline float eulerf::psi() const { return m_psi; }
inline float& eulerf::psi() { return m_psi; }

inline void eulerf::setPhi(float phi) {m_phi = phi;}
inline void eulerf::setTheta(float theta) {m_theta = theta; }
inline void eulerf::setPsi(float psi) {m_psi = psi; }
inline void eulerf::set(float phi, float theta, float psi) 
{
	m_phi = phi;
	m_theta = theta;
	m_psi = psi;
}
inline void eulerf::set(const vec3f& vec)
{
	m_phi = vec.x();
	m_theta = vec.y();
	m_psi = vec.z();
}

inline eulerf::operator QVariant() const
{
    return QVariant(getNEVarIds().eulerfId, this);
}

inline bool operator==(const eulerf &v1, const eulerf &v2)
{
	return v1.m_theta == v2.m_theta && v1.m_phi == v2.m_phi && v1.m_psi == v2.m_psi;
}

inline const eulerf operator+(const eulerf &v1, const eulerf &v2)
{
	return eulerf(v1.m_phi + v2.m_phi, v1.m_theta + v2.m_theta, v1.m_psi + v2.m_psi);
}

inline const eulerf operator-(const eulerf &v1, const eulerf &v2)
{
    return eulerf(v1.m_phi - v2.m_phi, v1.m_theta - v2.m_theta, v1.m_psi - v2.m_psi);
}
