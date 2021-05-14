
#include "Utility/NEAABB.h"

inline void AABB::clear()
{
    const qreal value = FLT_MAX;
	//clear min
	m_MinPos.setX(value); m_MinPos.setY(value); m_MinPos.setZ(value);
	//clear max
	m_MaxPos.setX(-value); m_MaxPos.setY(-value); m_MaxPos.setZ(-value);
}

//! Inserts point p into the bounding box.
inline void AABB::insertPoint(const vec3f& p)
{
	if(p.x() < m_MinPos.x()) m_MinPos.setX(p.x());
	if(p.x() > m_MaxPos.x()) m_MaxPos.setX(p.x());

	if(p.y() < m_MinPos.y()) m_MinPos.setY(p.y());
	if(p.y() > m_MaxPos.y()) m_MaxPos.setY(p.y());

	if(p.z() < m_MinPos.z()) m_MinPos.setZ(p.z());
	if(p.z() > m_MaxPos.z()) m_MaxPos.setZ(p.z());

}

//! Inserts box b into the bounding box.
inline void AABB::insert(const AABB& b)
{
    if(b.minPos().x() < m_MinPos.x()) m_MinPos.setX(b.minPos().x());
    if(b.maxPos().x() > m_MaxPos.x()) m_MaxPos.setX(b.maxPos().x());

    if(b.minPos().y() < m_MinPos.y()) m_MinPos.setY(b.minPos().y());
    if(b.maxPos().y() > m_MaxPos.y()) m_MaxPos.setY(b.maxPos().y());

    if(b.minPos().z() < m_MinPos.z()) m_MinPos.setZ(b.minPos().z());
    if(b.maxPos().z() > m_MaxPos.z()) m_MaxPos.setZ(b.maxPos().z());

}

//! Tests if point p is inside the bounding box.


inline bool AABB::isInside(const vec3f& p) const
{
    return (p.x() <= m_MaxPos.x() && p.x() >= m_MinPos.x()) &&
        (p.y() <= m_MaxPos.y() && p.y() >= m_MinPos.y()) &&
        (p.z() <= m_MaxPos.z() && p.z() >= m_MinPos.z());

}

inline bool AABB::isInside(const vec3f& p, double eps) const
{
    vec3f minPos = m_MinPos-vec3f(eps, eps, eps);
    vec3f maxPos = m_MaxPos+vec3f(eps, eps, eps);

    return (p.x() <= maxPos.x() && p.x() >= minPos.x()) &&
    (p.y() <= maxPos.y() && p.y() >= minPos.y()) &&
    (p.z() <= maxPos.z() && p.z() >= minPos.z());
}


inline bool AABB::intersects(const AABB& aabb2)
{		
	const vec3f& min2 = aabb2.minPos();
	const vec3f& max2 = aabb2.maxPos();
	
	if( maxPos().x() < min2.x() || minPos().x() > max2.x()) return false;		
	if( maxPos().y() < min2.y() || minPos().y() > max2.y()) return false;		
	if( maxPos().z() < min2.z() || minPos().z() > max2.z()) return false;		
	return true;
}

inline AABB AABB::overlapAABB(const AABB& aabb2) const
{	
	const vec3f& min2 = aabb2.minPos();
	const vec3f& max2 = aabb2.maxPos();
	AABB overlap;
	vec3f olMin(vec3f(9999,9999,9999)), olMax(vec3f(-9999,-9999,-9999));

	if( minPos().x() < min2.x()) olMin.setX(min2.x()); 
	else	olMin.setX(m_MinPos.x()); 
	if( maxPos().x() > max2.x()) olMax.setX(max2.x());
	else olMax.setX(m_MaxPos.x());

	if( minPos().y() < min2.y()) olMin.setY(min2.y());
	else olMin.setY(m_MinPos.y());
	if( maxPos().y() > max2.y()) olMax.setY(max2.y());
	else olMax.setY(m_MaxPos.y());

	if( minPos().z() < min2.z()) olMin.setZ(min2.z());
	else olMin.setZ(m_MinPos.z());
	if( maxPos().z() > max2.z()) olMax.setZ(max2.z());
	else olMax.setZ(m_MaxPos.z());

	//if(olMin == m_MinPos && olMax == m_MaxPos) 
	//	return AABB();
	//else
	return AABB(olMin,olMax);
}




const double NEAR_ONE = 0.999999;


inline OBB::OBB()
{
    this->m_extents = vec3f( 1.0, 1.0, 1.0 );
    this->m_center = vec3f( 0.0f, 0.0f, 0.0f );
    this->m_orientationQuat = quatf( 0.0, 0.0, 0.0, 1.0 );
    updateAxes( m_orientationQuat );
}

inline void OBB::updateAxes( const quatf &orientation )
{
    vec3f m_axes[3];
    m_axes[0] = orientation.rotatedVector( vec3f( 1.0, 0.0, 0.0 ) );
    m_axes[1] = orientation.rotatedVector( vec3f( 0.0, 1.0, 0.0 ) );
    m_axes[2] = orientation.rotatedVector( vec3f( 0.0, 0.0, 1.0 ) );
}


inline OBB::OBB( const vec3f &position, const quatf &orientation, const vec3f &extents )
{
    this->m_extents = extents;
    sync( position, orientation );
}


inline OBB::~OBB()
{
}

inline void OBB::setOrientationQuat(const quatf &orientation)
{
    this->m_orientationQuat = orientation;
    sync( m_center, m_orientationQuat );
}


inline void OBB::setCenter(const vec3f &position)
{
    this->m_center = position;
    sync( m_center, m_orientationQuat );
}

inline void OBB::setExtents(const vec3f &extents)
{
    this->m_extents = extents;
}

inline const quatf& OBB::getOrientationQuat() const
{
    return m_orientationQuat;
}

inline const vec3f& OBB::getCenter() const
{
    return m_center;
}

inline const vec3f& OBB::getExtents() const
{
    return m_extents;
}

inline const vec3f& OBB::getAxis(unsigned int axis) const
{
  //  assert(axis <= 2);
    return m_axes[axis];
}

inline bool OBB::isInside( vec3f& p )
{
    vec3f op = p - m_center;
    vec3f np = m_orientationQuat.conjugate().rotatedVector( op );

    return (np.x() <= m_extents.x() && np.x() >= m_extents.x()) &&
            (np.y() <= m_extents.y() && np.y() >= m_extents.y()) &&
            (np.z() <= m_extents.z() && np.z() >= m_extents.z());


    return false;
}

inline void OBB::sync( const vec3f &position, const quatf &orientation )
{
    this->m_orientationQuat = orientation ;
    this->m_orientationQuat.normalize();


    m_center = position ;
    updateAxes( this->m_orientationQuat ); //must be careful not to confuse and use the argument orientation
}


inline bool OBB::intersects(const OBB &obb1)
{
    OBB& obb0 = (*this);
    double c[3][3];       //c[i][j] = obb0.m_axes[i] . obb1.m_axes[j]
    double absC[3][3];    //absc[i][j] = fabs(c[i][j]
    double d[3];          //(obb1.centre - obb0.centre) . obb0.m_axes[i]
    double r, r0, r1;     //interval radii and distance between centres
    bool existsParallelPair = false;

    vec3f diff = obb1.m_center - obb0.m_center;

//testinging separation direction as surface normal of obb0
//axis go in loop is in turn axis obb0.centre + t * obb0.m_axes[i] for  0, 1, 2
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            c[i][j] = (obb0.m_axes[i]) * (obb1.m_axes[j]);
            absC[i][j] = fabs(c[i][j]);
            if (absC[i][j] > NEAR_ONE) existsParallelPair = true;
        }

        d[i] = diff * (obb0.m_axes[i]);
        r = fabs(d[i]);
        r0 = obb0.m_extents[i];
        r1 = obb1.m_extents[0] * absC[i][0] + obb1.m_extents[1] * absC[i][1] + obb1.m_extents[2] * absC[i][2];

        if (r > (r0 + r1)) return false;
    }

//testinging separation direction as surface normal of obb1
//axis go in loop is in turn axis obb1.centre + t * obb1.m_axes[i] for  0, 1, 2
    for (int i = 0; i < 3; i++)
    {
        r = fabs(diff * (obb1.m_axes[i]));
        r0 = obb0.m_extents[0] * absC[0][i] + obb0.m_extents[1] * absC[1][i] + obb0.m_extents[2] * absC[2][i];
        r1 = obb1.m_extents[i];

        if (r > (r0 + r1)) return false;
    }

//early rejection
    if (existsParallelPair) return true;

//axis obb0.centre + t * (obb0.m_axes[0] X obb1.m_axes[0])
    r = fabs(d[2] * c[1][0] - d[1] * c[2][0]);
    r0 = obb0.m_extents[1] * absC[2][0] + obb0.m_extents[2] * absC[1][0];
    r1 = obb1.m_extents[1] * absC[0][2] + obb1.m_extents[2] * absC[0][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[0] X obb1.m_axes[1])
    r = fabs(d[2] * c[1][1] - d[1] * c[2][1]);
    r0 = obb0.m_extents[1] * absC[2][1] + obb0.m_extents[2] * absC[1][1];
    r1 = obb1.m_extents[0] * absC[0][2] + obb1.m_extents[2] * absC[0][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[0] X obb1.m_axes[2])
    r = fabs(d[2] * c[1][2] - d[1] * c[2][2]);
    r0 = obb0.m_extents[1] * absC[2][2] + obb0.m_extents[2] * absC[1][2];
    r1 = obb1.m_extents[0] * absC[0][1] + obb1.m_extents[1] * absC[0][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[1] X obb1.m_axes[0])
    r = fabs(d[0] * c[2][0] - d[2] * c[0][0]);
    r0 = obb0.m_extents[0] * absC[2][0] + obb0.m_extents[2] * absC[0][0];
    r1 = obb1.m_extents[1] * absC[1][2] + obb1.m_extents[2] * absC[1][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[1] X obb1.m_axes[1])
    r = fabs(d[0] * c[2][1] - d[2] * c[0][1]);
    r0 = obb0.m_extents[0] * absC[2][1] + obb0.m_extents[2] * absC[0][1];
    r1 = obb1.m_extents[0] * absC[1][2] + obb1.m_extents[2] * absC[1][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[1] X obb1.m_axes[2])
    r = fabs(d[0] * c[2][2] - d[2] * c[0][2]);
    r0 = obb0.m_extents[0] * absC[2][2] + obb0.m_extents[2] * absC[0][2];
    r1 = obb1.m_extents[0] * absC[1][1] + obb1.m_extents[1] * absC[1][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[2] X obb1.m_axes[0])
    r = fabs(d[1] * c[0][0] - d[0] * c[1][0]);
    r0 = obb0.m_extents[0] * absC[1][0] + obb0.m_extents[1] * absC[0][0];
    r1 = obb1.m_extents[1] * absC[2][2] + obb1.m_extents[2] * absC[2][1];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[2] X obb1.m_axes[1])
    r = fabs(d[1] * c[0][1] - d[0] * c[1][1]);
    r0 = obb0.m_extents[0] * absC[1][1] + obb0.m_extents[1] * absC[0][1];
    r1 = obb1.m_extents[0] * absC[2][2] + obb1.m_extents[2] * absC[2][0];
    if (r > (r0 + r1)) return false;

    //axis obb0.centre + t * (obb0.m_axes[2] X obb1.m_axes[2])
    r = fabs(d[1] * c[0][2] - d[0] * c[1][2]);
    r0 = obb0.m_extents[0] * absC[1][2] + obb0.m_extents[1] * absC[0][2];
    r1 = obb1.m_extents[0] * absC[2][1] + obb1.m_extents[1] * absC[2][0];
    if (r > (r0 + r1)) return false;

    return true;
}
