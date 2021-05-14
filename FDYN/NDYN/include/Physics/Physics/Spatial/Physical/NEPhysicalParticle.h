#pragma once

#include "Physics/Spatial/Physical/NEParticle.h"
#include "Physics/Spatial/Physical/NEThermalParticle.h"
#include "Physics/Spatial/Physical/NEChemParticle.h"

/*!
	Physical particles can be integrated over time. 
	Thus, they store velocity and acceleration.
*/

class NEPhysicalParticle : public NEParticle, public NEThermalParticle, public NEChemParticle
{
public:
	NEPhysicalParticle() : NEParticle(), m_Accel(vec3f(0.0,0.0,0.0)), m_Velocity(vec3f(0.0,0.0,0.0)) { }
	
	NEPhysicalParticle(const NEParticle& particle) : NEParticle(particle)
	{
		m_Accel = vec3f(0.0,0.0,0.0);
		m_Velocity = vec3f(0.0,0.0,0.0);
	}

	inline const vec3f& accel() const { return m_Accel; }
	inline vec3f& accel() { return m_Accel; }
	inline void setAccel(const vec3f& accel){ m_Accel = accel; }

	inline const vec3f& velocity() const { return m_Velocity; }
	inline vec3f& velocity() { return m_Velocity; }
	inline void setVelocity(const vec3f& velocity) { m_Velocity = velocity; }
	
	inline void setVelocityX(const double x) {m_Velocity.setX(x);}
	inline void setVelocityY(const double y) {m_Velocity.setY(y);}
	inline void setVelocityZ(const double z) {m_Velocity.setZ(z);}

protected:
	//!acceleration
	vec3f m_Accel;
	//!velocity
	vec3f m_Velocity;
};
