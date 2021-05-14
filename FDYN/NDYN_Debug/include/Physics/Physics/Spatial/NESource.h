#pragma once

#include "Physics/Spatial/NEPhysical.h"

class NECEXP_PHYSICS NESource : public NEPhysical
{
    Q_OBJECT

    Q_CLASSINFO("ClassName", "General Source")

    Q_PROPERTY(NEMPParticleType particleType READ particleType WRITE setParticleType DESIGNABLE isConnectedToMultiPhysicsSolver())
    Q_CLASSINFO("name-particleType", "Particle Type")
    Q_CLASSINFO("help-particleType", "Sets the particle type of the source as fluid or solid.")

    Q_ENUMS(NEMPParticleType)
public:
    NESource(NENodeFactory* factory):NEPhysical(factory, PhysicalType::kNone) {m_Type = NEMPParticleType::NEFluidType;}
    virtual ~NESource(){}
    virtual uint init(uint initMode);

    enum NEMPParticleType
    {
            NEFluidType,
            NESolidType
    };

    bool isConnectedToMultiPhysicsSolver() {return false;}
    bool isMPFluid()
    {
        if(m_Type==NEMPParticleType::NEFluidType)
            return true;
        else
            return false;
    }

public slots:
    NEMPParticleType particleType() const {return m_Type;}
    void setParticleType(NEMPParticleType type) {m_Type = type;}  //set preset values can be used here

protected:
    NEMPParticleType m_Type; // Fluid or solid particles can be generated

    // NEObject interface
protected:
    virtual void initializeGUIActions() override;

    // NENode interface
public:
    virtual uint eval();
};

Q_DECLARE_INTERFACE(NESource, "NESource")




