#pragma once

#include "Physics/Spatial/Physical/NEPhysicalParticle.h"

class NEFluidParticle : public NEPhysicalParticle
{
public:

    enum NEFluidParticleType
	{	
		kInternalParticle = 1,
        kSurfaceParticle = 2,
        kInitiallySurfaceParticle = 3,
        kFreeSurfaceParticle = 4
	};

	NEFluidParticle(){}
    NEFluidParticle(const NEParticle& particle) : NEPhysicalParticle(particle)
	{
		//m_Position = particle.position();
		//m_Id = particle.id();
		//m_CellId = particle.cellId();
		//m_pSolver = particle.solver();
		//m_Color = particle.color();
		m_Density = -1;
		m_Pressure = 0.0;
        m_ParticleType = kInternalParticle;
		m_Normal.set(0.0f,0.0f,0.0f);
	}
	
	inline float density() const { return m_Density; }
	inline float& density() { return m_Density; }
	inline void setDensity(const float density){m_Density = density; }

	inline float pressure() const {return m_Pressure; }
	inline float& pressure() { return m_Pressure; }
    inline void setPressure(float pressure){m_Pressure = pressure; }

    inline uint particleTypeData() const { return m_ParticleType; }
    inline uint& particleTypeData() { return m_ParticleType; }

	inline uint particleType() const { return m_ParticleType & 0x00FFFFFF; }
    inline uint& particleType() { return m_ParticleType; }
    inline void setParticleType(uint type) {m_ParticleType = type;}
    inline void setParticleType(uint nrNeighbors, uint particleType)
    {
        uint type(nrNeighbors<<24);
        m_ParticleType =type | particleType;
    }

	inline uint nrNeighborsFromType() { return m_ParticleType >> 24; }

    inline vec3f normal() const { return m_Normal; }
    inline vec3f& normal() { return m_Normal; }
    inline void setNormal(vec3f val) { m_Normal = val; }

protected:		
	float m_Density;	
	float m_Pressure;	
	//!Type is set according to spatial location of particle, e.g. surface or internal. See NEParticleSectionType
	uint m_ParticleType;
//	uchar offset_NEFluidParticle;
	//! The particle normal is used in the surface reconstruction step 
	vec3f m_Normal;
};

class NEFluidParticleCopy
{
public:

	NEFluidParticleCopy(){}
	
	inline uint id() const {return m_Id; }
    inline uint& id() { return m_Id; }
    inline void setId(const uint id){ m_Id = id; }

	inline const vec3f& position() const { return m_Position; }	
    inline vec3f& position() { return m_Position; }
	inline void setPosition(const vec3f& position) { m_Position = position; }

    inline const vec3f& velocity() const { return m_Velocity; }
    inline vec3f& velocity() { return m_Velocity; }
    inline void setVelocity(const vec3f& velocity) { m_Velocity = velocity; }

	inline const vec3f& accel() const { return m_Accel; }
    inline vec3f& accel() { return m_Accel; }
	inline void setAccel(const vec3f& accel){ m_Accel = accel; }
	
    inline float density() const { return m_Density; }
    inline float& density() { return m_Density; }

	inline float pressure() const {return m_Pressure; }
    inline float& pressure() { return m_Pressure; }
    inline void setPressure(float pressure){m_Pressure = pressure; }

    inline double temperature() const { return m_Temperature; }
    inline double& temperature() { return m_Temperature; }

    inline std::array<double, maxNumOfChemSpecies> concnsChemSpecies() const { return m_ConcnsChemSpecies; }
    inline std::array<double, maxNumOfChemSpecies>& concnsChemSpecies() { return m_ConcnsChemSpecies; }

protected:		
	uint m_Id;
	vec3f m_Position;	
	vec3f m_Accel;
	vec3f m_Velocity;
    float m_Density;
	float m_Pressure;
    double m_Temperature;
    std::array<double, maxNumOfChemSpecies> m_ConcnsChemSpecies;
};
