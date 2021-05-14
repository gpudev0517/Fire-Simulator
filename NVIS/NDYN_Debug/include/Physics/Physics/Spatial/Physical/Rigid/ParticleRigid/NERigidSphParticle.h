#pragma once

#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidParticle.h"

class NERigidSphParticle : public NERigidParticle
{
public:
	NERigidSphParticle()
	{
        m_Velocity = vec3f(0.0f, 0.0f, 0.0f);
        m_Accel = vec3f(0.0f, 0.0f, 0.0f);
		m_Density = 1.0;
		m_Pressure = 0.0;
		m_KernelSum = 0.0;
		m_OneOverKernelSum = 0.0;
	}

	NERigidSphParticle(const NERigidParticle& basicP) : NERigidParticle(basicP)
	{
        m_Velocity = vec3f(0.0f, 0.0f, 0.0f);
        m_Accel = vec3f(0.0f, 0.0f, 0.0f);
		m_Density = 1.0;
		m_Pressure = 0.0;
		m_KernelSum = 0.0;
		m_OneOverKernelSum = 0.0;
	}
	virtual ~NERigidSphParticle(){}

    inline float density() const {return m_Density; }
    inline float& density() { return m_Density; }
    inline void setDensity(const float val){m_Density = val; }

    inline float pressure() const {return m_Pressure; }
    inline float& pressure() { return m_Pressure; }
    inline void setPressure(const float val) { m_Pressure = val; }

    inline const float& pressureOverSquaredDensity() const { return m_PressureOverSquaredDensity; }
    inline float& pressureOverSquaredDensity() { return m_PressureOverSquaredDensity; }
    inline void setPressureOverSquaredDensity(float val) { m_PressureOverSquaredDensity = val; }

    inline float kernelSum() const {return m_KernelSum; }
    inline void setKernelSum(const float density){ m_KernelSum = density; setOneOverKernelSum();}

    //do not use mass anymore in initial density compuation of rigid particle
    inline float oneOverKernelSum() { return m_OneOverKernelSum; }
    inline void setOneOverKernelSum() {m_OneOverKernelSum = 1.f / m_KernelSum;}

    inline const vec3f& force() const { return m_Force; }
    inline vec3f& force() { return m_Force; }
	inline void setForce(const vec3f& val) { m_Force = val; }

    inline const vec3f& velocity() const { return m_Velocity; }
    inline vec3f& velocity() { return m_Velocity; }
    inline void setVelocity(const vec3f& val) { m_Velocity = val; }

    inline const vec3f& slipVelocity() const { return m_SlipVelocity; }
    inline vec3f& slipVelocity() { return m_SlipVelocity; }
    inline void setSlipVelocity(const vec3f& val) { m_SlipVelocity = val; }

    inline const vec3f& accel() const { return m_Accel; }
    inline vec3f& accel() { return m_Accel; }
    inline void setAccel(const vec3f& val) { m_Accel = val; }

    //! Returns the adapted mass with respect to the provided rest density of the fluid
	inline float mass(float restDensity) const {return restDensity * m_OneOverKernelSum;}

private:
	float m_KernelSum;
	float m_OneOverKernelSum;
	//! The total force on the particle (required for 2-way coupling)
	vec3f m_Force;
    vec3f m_Velocity;
    vec3f m_SlipVelocity;
    vec3f m_Accel;
	float m_Pressure;
	float m_Density;
    float m_PressureOverSquaredDensity;
};
