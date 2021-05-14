#pragma once

#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidPciSphParticle.h"

class NEPciSphRigid
{
public:
	NEPciSphRigid(void);
	~NEPciSphRigid(void);

	//! Returns the std::vector of rigid particles.
	const std::vector<NERigidPciSphParticle>& pciSphParticles() const { return m_PciSphParticles; }
	std::vector<NERigidPciSphParticle>& pciSphParticles() { return m_PciSphParticles; }

	//! Transforms the particles according to WorldTransform, should be called after particles are generated
    NEFEXP_CORE void transformPciSphParticles(eulerf orientation,  matrix44f worldTransform);
	void generatePciSphParticle(vec3f position, vec3f normal);

	void clearPciSphParticles();

protected:
	//! Stores the rigid body particles for pci sph solver
	std::vector<NERigidPciSphParticle> m_PciSphParticles;
};

