#pragma once

#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"

class NESphLiquid;

class NESphLiquidParticle : public NEFluidParticle
{
public:
	NESphLiquidParticle(){}
	NESphLiquidParticle(const NEParticle& particle) : NEFluidParticle(particle) {}
	//virtual ~NESphLiquidParticle(){}

//	inline const double colorValue() const {return m_ColorValue; }
//	inline double colorValue() { return m_ColorValue; }
//	inline void setColorValue(const double colVal){m_ColorValue = colVal; }
//
//
//protected:
//	float m_ColorValue;
};
