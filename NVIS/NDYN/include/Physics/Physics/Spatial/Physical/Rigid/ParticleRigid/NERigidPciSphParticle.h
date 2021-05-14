#pragma once

#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"

class NERigidPciSphParticle : public NERigidSphParticle
{
public:
	NERigidPciSphParticle()
	{
		
	}

	NERigidPciSphParticle(const NERigidSphParticle& basicP) : NERigidSphParticle(basicP)
	{
		m_Normal = vec3f(0.f, 0.f, 0.f);
		m_InitialNormal = vec3f(0.f, 0.f, 0.f);
	}
	virtual ~NERigidPciSphParticle(){}

	inline void setInitialNormal(const vec3f& normal) {m_InitialNormal = normal;}
	inline vec3f initialNormal() const {return m_InitialNormal;}

	inline const vec3f& normal() const {return m_Normal;}
	inline vec3f& normal() { return m_Normal; }
	void setNormal(const vec3f&  normal) {m_Normal = normal;}

private:
	vec3f m_InitialNormal;
	vec3f m_Normal;

};
