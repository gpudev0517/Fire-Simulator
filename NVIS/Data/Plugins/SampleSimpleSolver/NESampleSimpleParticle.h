#pragma once

#include "Spatial/Physical/Fluid/ParticleFluid/SphLiquid/NESphLiquidParticle.h"

class NESampleSphParticle : public NESphLiquidParticle
{
public:
    NESampleSphParticle() : NESphLiquidParticle(){}
    NESampleSphParticle(const NEParticle& particle) : NESphLiquidParticle(particle) {}
    NESampleSphParticle(const NEParticle& particle, const vec3f& vel) : NESphLiquidParticle(particle) { m_Velocity = vel;}
    //virtual ~NESampleSphParticle(){}


};