#pragma once

#include "Spatial/NESpatial.h"

NE_FWD_DECL_2(class, NEParticle, NEFluidParticle)

class NECEXP_PHYSICS NEDualBoundary : public NESpatial
{
    Q_OBJECT

public:
    NEDualBoundary(NENodeFactory* factory) : NESpatial(factory) {}
    virtual ~NEDualBoundary(){}

    //! Resets some values
    //! Called when restarting the simulation from the start
    virtual void reset();

    //! Does some pre-processing
    //! Called at the start of each time step
    virtual void preProcess();

    //! Does some post-processing
    //! Called at the end of each time step
    virtual void postProcess();

    //! For each fluid particle and solid particle, decides whether to "kill" it or not;
    //! if to kill, either marks it for deletion or transforms it into an outflow ghost (fluid only)
    //! Called at each time step
    virtual uint checkAndMarkToKill();

    //! Generates particles and adds them to the connected fluid solver
    //! Called at each time step
    virtual void generateParticles();

    //! Generates clone particles and adds them to the connected fluid solver
    //! Called at each time step, after the generation of all other particles
    virtual void generateClones();

protected:
    void render() override;
    void renderPicked() override;
    void renderPickable() override;
};

Q_DECLARE_INTERFACE(NEDualBoundary, "NEDualBoundary")
