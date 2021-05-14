#pragma once

#include "Physics/Spatial/NEKiller.h"
#include "Physics/Spatial/Physical/NEParticle.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NEParticleKiller), NEParticleFluidSolver, NESphRigid)

//!  Killer class for particle fluids.
/*!
	Must be binded to a particle fluid solver.
*/
class NECEXP_PHYSICS NEParticleKiller : public NEKiller
{
    Q_OBJECT

public:

    enum ThermalSettingType { Adiabatic = 0, Isothermal = 1 };

    NE_PROP_RW(bool, KillFluidParticles, "Kill Fluid Particles", "Decides whether to kill fluid particles or not.", true, )
    NE_PROP_RW(bool, KillFluidGhosts, "Kill Fluid Ghosts", "Decides whether to kill fluid ghost particles or not.", true, )
    NE_PROP_RW(bool, KillSolidParticles, "Kill Solid Particles", "Decides whether to kill solid particles or not.", true, )
    NE_PROP_RW(ThermalSettingType, ThermalSetting, "Thermal Setting", "Type of thermal setting at the outlet", true, )
    NE_PROP_RW(double, Temperature, "Temperature", "Temperature of the fluid at the outlet", true, )

    Q_ENUMS(ThermalSettingType)

public:
    NEParticleKiller(NENodeFactory* factory);
    virtual ~NEParticleKiller();

    void reset() override;
    void resetProperties() override;

    void preProcess() override;
    void postProcess() override;
    uint checkAndMarkToKill() override;
	
    uint checkAndMarkToKillFluid();
    uint checkAndMarkToKillRigids();
    virtual uint checkAndMarkToKill(NEParticleFluidSolver* fluidSolver);
    virtual uint checkAndMarkToKill(NESphRigid* rigid);
    virtual bool toKill(const NEParticle& p) const;

    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Returns the list of connected fluid solvers
    std::forward_list<NEParticleFluidSolver*> connectedFluidSolvers() const;
    //! Returns the list of connected rigids
    std::forward_list<NESphRigid*> connectedRigids() const;
    //! Connects to a fluid solver
    uint connectToFluidSolver(NEParticleFluidSolver* fluidSolver);
    //! Connects to a rigid
    uint connectToRigid(NESphRigid* rigid);
    //! Disconnects from a fluid solver
    uint disconnectFromFluidSolver(NEParticleFluidSolver* fluidSolver);
    //! Disconnects from a rigid
    uint disconnectFromRigid(NESphRigid* rigid);

private:
	NE_DECL_PRIVATE(NEParticleKiller)
};

Q_DECLARE_INTERFACE(NEParticleKiller, "NEParticleKiller")
