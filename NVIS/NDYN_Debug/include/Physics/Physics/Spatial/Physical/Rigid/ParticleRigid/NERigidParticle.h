#pragma once

#include "Physics/Spatial/Physical/NEParticle.h"
#include "Physics/Spatial/Physical/NEThermalParticle.h"
#include "Physics/Spatial/Physical/NEChemParticle.h"
#include "Physics/Spatial/Physical/NERigid.h"

class NERigidParticle : public NEParticle, public NEThermalParticle, public NEChemParticle
{
public:

    NERigidParticle()
    {
        m_Active = false;
        m_ActiveVector = false;
    }

    NERigidParticle(const NERigidParticle& p) : NEParticle(p)
    {
        m_ActiveVector = p.inActiveVector();
        m_Active = p.active();
        setInitialPosition(p.initialPosition());
    }

    virtual ~NERigidParticle(){}

    inline bool active() const {return m_Active;}
    inline void setActive(bool b) {m_Active = b;}

    inline bool inActiveVector() const {return m_ActiveVector;}
    inline void setInActiveVector(bool b) {m_ActiveVector = b;}

    inline void setInitialPosition(const vec3f& position) {m_InitialPosition = position;}
    inline const vec3f& initialPosition() const {return m_InitialPosition;}

    inline const vec3f rigidBodyVelocity() const
    {
        NERigid* rigid = static_cast<NERigid*>(solver());
        vec3f dPos = position()-rigid->Position();
        vec3f vel = rigid->LinearVelocity()+vec3f::crossProduct(rigid->AngularVelocity(), dPos);
        return vel;
    }

    inline const vec3f rigidBodyAccel() const
    {
        NERigid* rigid = static_cast<NERigid*>(solver());
        vec3f dPos = position()-rigid->Position();
        vec3f accel = rigid->LinearAcceleration()+vec3f::crossProduct(rigid->AngularAcceleration(), dPos)+vec3f::crossProduct(rigid->AngularVelocity(), vec3f::crossProduct(rigid->AngularVelocity(), dPos));
        return accel;
    }

private:
    //! The rigid particle is activated and deactivated in the collider.
    bool m_Active;
    //! True if this particle is in active std::vector
    bool m_ActiveVector;
    vec3f m_InitialPosition;
};
