inline matrix33f::matrix33f (float m11, float m12, float m13, 
							 float m21, float m22, float m23, 
							 float m31, float m32, float m33)
{
	m[0][0] = m11; m[0][1] = m21; m[0][2] = m31;
	m[1][0] = m12; m[1][1] = m22; m[1][2] = m32;
	m[2][0] = m13; m[2][1] = m23; m[2][2] = m33; 
}

inline matrix33f::matrix33f( const matrix33f& matrix )
{
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			m[i][j] = matrix.m[i][j];
}

inline const float& matrix33f::operator()(int aRow, int aColumn) const
{
	Q_ASSERT(aRow >= 0 && aRow < 3 && aColumn >= 0 && aColumn < 3);
	return m[aColumn][aRow];
}

inline float& matrix33f::operator()(int aRow, int aColumn)
{
	Q_ASSERT(aRow >= 0 && aRow < 3 && aColumn >= 0 && aColumn < 3);
	return m[aColumn][aRow];
}

inline bool matrix33f::isIdentity() const
{
	if (m[0][0] != 1.0f || m[0][1] != 0.0f || m[0][2] != 0.0f)
		return false;
	if (m[1][0] != 0.0f || m[1][1] != 1.0f || m[1][2] != 0.0f)
		return false;
	if (m[2][0] != 0.0f || m[2][1] != 0.0f || m[2][2] != 1.0f)
		return false;
	return true;
}

inline void matrix33f::setToIdentity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
}

inline bool matrix33f::isZero() const
{
	for(uint i=0; i < 3; i++)
		for(uint j=0; j < 3; j++)
			if(!FIsZero(m[i][j]))
				return false;

	return true;
}

inline void matrix33f::setToZero()
{
	m[0][0] = 0.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 0.0f;
	m[1][2] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 0.0f;
}

inline void matrix33f::fill(float value)
{
	m[0][0] = value;
	m[0][1] = value;
	m[0][2] = value;

	m[1][0] = value;
	m[1][1] = value;
	m[1][2] = value;

	m[2][0] = value;
	m[2][1] = value;
	m[2][2] = value;
}

inline matrix33f& matrix33f::operator+=(const matrix33f& other)
{
	m[0][0] += other.m[0][0];
	m[0][1] += other.m[0][1];
	m[0][2] += other.m[0][2];

	m[1][0] += other.m[1][0];
	m[1][1] += other.m[1][1];
	m[1][2] += other.m[1][2];

	m[2][0] += other.m[2][0];
	m[2][1] += other.m[2][1];
	m[2][2] += other.m[2][2];

	return *this;
}

inline matrix33f& matrix33f::operator-=(const matrix33f& other)
{
	m[0][0] -= other.m[0][0];
	m[0][1] -= other.m[0][1];
	m[0][2] -= other.m[0][2];

	m[1][0] -= other.m[1][0];
	m[1][1] -= other.m[1][1];
	m[1][2] -= other.m[1][2];

	m[2][0] -= other.m[2][0];
	m[2][1] -= other.m[2][1];
	m[2][2] -= other.m[2][2];

	return *this;
}

inline matrix33f& matrix33f::operator*=(const matrix33f& other)
{

	*this = *this * other;
	return *this;
}

inline matrix33f& matrix33f::operator*=(float factor)
{
	m[0][0] *= factor;
	m[0][1] *= factor;
	m[0][2] *= factor;

	m[1][0] *= factor;
	m[1][1] *= factor;
	m[1][2] *= factor;

	m[2][0] *= factor;
	m[2][1] *= factor;
	m[2][2] *= factor;

	return *this;
}

inline bool matrix33f::operator==(const matrix33f& other) const
{
	return m[0][0] == other.m[0][0] &&
		m[0][1] == other.m[0][1] &&
		m[0][2] == other.m[0][2] &&
		m[1][0] == other.m[1][0] &&
		m[1][1] == other.m[1][1] &&
		m[1][2] == other.m[1][2] &&
		m[2][0] == other.m[2][0] &&
		m[2][1] == other.m[2][1] &&
		m[2][2] == other.m[2][2] ;

}

inline bool matrix33f::operator!=(const matrix33f& other) const
{
	return m[0][0] != other.m[0][0] ||
		m[0][1] != other.m[0][1] ||
		m[0][2] != other.m[0][2] ||
		m[1][0] != other.m[1][0] ||
		m[1][1] != other.m[1][1] ||
		m[1][2] != other.m[1][2] ||
		m[2][0] != other.m[2][0] ||
		m[2][1] != other.m[2][1] ||
		m[2][2] != other.m[2][2] ;
}

inline matrix33f& matrix33f::operator=( const matrix33f& A )
{
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			m[i][j] = A.m[i][j];

	return *this;
}

inline float *matrix33f::data()
{

	return m[0];
}

inline void matrix33f::jacobiRotate(matrix33f &R, int p, int q)
{
  // rotates A through phi in pq-plane to set A.get(p, q) = 0
  // rotation stored in R whose columns are eigenvectors of A
  float d = (m[p][p] - m[q][q])/(2.0f * m[p][q]);
  float t = 1.0f / (fabs(d) + sqrt(d * d + 1.0f));
  if(d < 0) t = -t;
  float c = 1.0f / sqrt(t * t + 1);
  float s = t * c;
  m[p][p] += t * m[p][q];
  m[q][q] -= t * m[p][q];
  m[p][q] = m[q][p] = 0;
  // transform A
  int k;
  for (k = 0; k < 3; k++) {
    if(k != p && k != q) {
      float Akp = c * m[k][p] + s * m[k][q];
      float Akq =-s * m[k][p] + c * m[k][q];
      m[k][p] = m[p][k] = Akp;
      m[k][q] = m[q][k] = Akq;
    }
  }
  // store rotation in R
  for (k = 0; k < 3; k++) {
    float Rkp = c * R.m[k][p] + s * R.m[k][q];
    float Rkq =-s * R.m[k][p] + c * R.m[k][q];
    R.m[k][p] = Rkp;
    R.m[k][q] = Rkq;
  }
}

#define JACOBI_EPSILON 1e-15
#define JACOBI_ITERATIONS 100

inline void matrix33f::eigenDecomposition(matrix33f &R)
{
  // only for symmetric matrices!
  // A = R A' R^T, where A' is diagonal and R orthonormal

  R.setToIdentity();	// unit matrix
  int iter = 0;
  while (iter < JACOBI_ITERATIONS) {	// 3 off diagonal elements
    // find off diagonal element with maximum modulus
    int p,q;
    float a,max;
    max = fabs(m[0][1]);
    p = 0; q = 1;
    a  = fabs(m[0][2]);
    if(a > max) { p = 0; q = 2; max = a; }
    a  = fabs(m[1][2]);
    if(a > max) { p = 1; q = 2; max = a; }
    // all small enough -> done
    if(max < JACOBI_EPSILON) break;
    // rotate matrix with respect to that element
    jacobiRotate(R, p, q);
    iter++;
  }
}

inline matrix33f& matrix33f::dyad_product(const vec3f &a,const vec3f &b)
{         
	m[0][0] = a.x()*b.x();
	m[0][1] = a.x()*b.y();
	m[0][2] = a.x()*b.z();

	m[1][0] = a.y()*b.x();
	m[1][1] = a.y()*b.y();
	m[1][2] = a.y()*b.z();

	m[2][0] = a.z()*b.x();
	m[2][1] = a.z()*b.y();
	m[2][2] = a.z()*b.z();

	return (*this);
}

inline float matrix33f::norm(float p) const
{
	float result(0.f);
	for(int i=0; i<3; i++)
	{
		for(int j=0; j<1; j++)
		{
			result += pow(m[i][j], p);
		}
	}
	result = pow(result,  1.f/p);
	return result;
}