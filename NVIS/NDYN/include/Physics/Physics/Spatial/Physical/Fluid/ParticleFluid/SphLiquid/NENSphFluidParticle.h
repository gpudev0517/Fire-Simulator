#pragma once

#include "Physics/Spatial/Physical/Fluid/ParticleFluid/SphLiquid/NESphLiquidParticle.h"

class NENSphFluidParticle : public NESphLiquidParticle
{
public:
    NENSphFluidParticle() : NESphLiquidParticle()
    {
       m_PressureOverSquaredDensity = 0.0f;
    }
    NENSphFluidParticle(const NEParticle& p) : NESphLiquidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
    }
    NENSphFluidParticle(const NEParticle& p, const vec3f& vel) : NESphLiquidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
    }
    NENSphFluidParticle(const NEParticle& p, const vec3f& vel, double temperature) : NESphLiquidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_Temperature = temperature;
    }
    NENSphFluidParticle(const NEParticle& p, const vec3f& vel, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies) : NESphLiquidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_ConcnsChemSpecies = concnsChemSpecies;
    }
    NENSphFluidParticle(const NEParticle& p, const vec3f& vel, double temperature, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies) : NESphLiquidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_Temperature = temperature;
        m_ConcnsChemSpecies = concnsChemSpecies;
    }

    inline const float& pressureOverSquaredDensity() const { return m_PressureOverSquaredDensity; }
    inline float& pressureOverSquaredDensity() { return m_PressureOverSquaredDensity; }

    float m_PressureOverSquaredDensity;
};

/*
class NENSphTwoFluidParticle : public NENSphFluidParticle
{
public:
    NENSphTwoFluidParticle() : NENSphFluidParticle()
    {
       m_PressureOverSquaredDensity = 0.0f;
    }
    NENSphTwoFluidParticle(const NEParticle& p) : NENSphFluidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
    }
    NENSphTwoFluidParticle(const NEParticle& p, const vec3f& vel) : NENSphFluidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
    }
    NENSphTwoFluidParticle(const NEParticle& p, const vec3f& vel, double temperature) : NENSphFluidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_Temperature = temperature;
    }
    NENSphTwoFluidParticle(const NEParticle& p, const vec3f& vel, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies) : NENSphFluidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_ConcnsChemSpecies = concnsChemSpecies;
    }
    NENSphTwoFluidParticle(const NEParticle& p, const vec3f& vel, double temperature, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies) : NENSphFluidParticle(p)
    {
        m_PressureOverSquaredDensity = 0.0f;
        m_Velocity = vel;
        m_Temperature = temperature;
        m_ConcnsChemSpecies = concnsChemSpecies;
    }

    inline const float& volumeFracFluid() const { return m_VolumeFracFluid; }
    inline float& volumeFracFluid() { return m_VolumeFracFluid; }
    inline const float& volumeFracGrains() const { return 1.0-m_VolumeFracFluid; }
    inline float& volumeFracGrains() { return 1.0-m_VolumeFracFluid; }

    inline float& densityFluid() const { return m_Density; }
    inline const float& densityFluid() const { return m_Density; }
    inline float& densityGrains(double restDensityGrains) const { return volumeFracGrains()*restDensityGrains; }
    inline const float& densityGrains(double restDensityGrains) const { return volumeFracGrains()*restDensityGrains; }
    inline float& phaseDensityFluid() const { return m_Density/volumeFracFluid(); }
    inline const float& phaseDensityFluid() const { return m_Density/volumeFracFluid(); }
    inline float& phaseDensityGrains(double restDensityGrains) const { return restDensityGrains; }
    inline const float& phaseDensityGrains(double restDensityGrains) const { return restDensityGrains; }

    inline float& pressureFluid() const { return m_Pressure; }
    inline const float& pressureFluid() const { return m_Pressure; }

    inline float& temperatureFluid() const { return m_Temperature; }
    inline const float& temperatureFluid() const { return m_Temperature; }

    float m_VolumeFracFluid;

};*/
