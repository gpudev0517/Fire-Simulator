inline complexf::complexf() : m_Real(1.0), m_Imag(1.0) {}
inline complexf::complexf(float real, float imag) : m_Real(real), m_Imag(imag) {}
inline complexf::complexf(const complexf& c) :m_Real(c.m_Real), m_Imag(c.m_Imag) {}

complexf& complexf::operator = (const complexf& c)
{
	m_Real = c.real();
	m_Imag = c.imag();
	return *this;
}

inline float complexf::real() const { return m_Real; }

inline float& complexf::real() { return m_Real; }

inline float complexf::imag() const {return m_Imag;}

inline float& complexf::imag() {return m_Imag;}

inline void complexf::setReal( float real ) {m_Real = real;}

inline void complexf::setImag( float imag ) {m_Imag = imag;}

inline void complexf::set( float real, float imag ) {m_Real = real; m_Imag = imag;}

inline void complexf::set( const complexf& c ) {m_Real = c.real(); m_Imag = c.imag();}
