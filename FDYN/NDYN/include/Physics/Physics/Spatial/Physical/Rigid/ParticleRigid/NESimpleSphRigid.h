#pragma once

#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"

class NESimpleSphRigid
{
public:
	NESimpleSphRigid(void);
	~NESimpleSphRigid(void);

	//! Returns the std::vector of rigid particles.
	const std::vector<NERigidSphParticle>& sphParticles() const { return m_SphParticles; }
	std::vector<NERigidSphParticle>& sphParticles() { return m_SphParticles; }

	//! Transforms the particles according to WorldTransform, should be called after particles are generated
    NEFEXP_CORE void transformSimpleSphParticles(eulerf orientation,  matrix44f worldTransform);
	void generateSimpleSphParticle(vec3f position);

	void clearSimpleSphParticles();

protected:
	//! Stores the rigid body particles for pci sph solver
	std::vector<NERigidSphParticle> m_SphParticles;
};

