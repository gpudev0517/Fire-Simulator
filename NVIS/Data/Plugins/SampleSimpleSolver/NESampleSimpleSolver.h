#pragma once

#include "Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"
#include "NESampleSimpleParticle.h"
#include "Base/NENodeFactory.h"


//!  Implementation of a simple particle solver
/*!
  A Simple Solver which moves particles
*/
class NECEXP NESampleSimpleSolver : public NESphLiquidSolver
{
    Q_INTERFACES(NESphLiquidSolver)
    Q_OBJECT

public:

    NESampleSimpleSolver(NENodeFactory* factory);
    ~NESampleSimpleSolver();

    uint init(uint initMode = NE::kInitOnCreate);

    //! Deletes all marked particles out of the vector.
    virtual void deleteParticles();
    //! Deletes the particle
    virtual void deleteParticle(NEParticle* particle);

    //! Returns true if explicit boundary handling is required used when integrating particles.
    virtual bool explicitBoundaryHandling() const;

    //! Returns the memory consumption of the particles
    virtual uint memoryConsumption();


public slots:
    //! For adding particles to the fluid, for example by a source that is connected to the fluid.
    virtual void addParticles(NEParticleSource *source, std::vector<NEParticle> &newParticles, const vec3f vel);

    //! The main update routine for single-phase fluids, called by Liquid Manager.
    void update();
    //! The main update routine for multiphase fluids.
    NESphUpdatePart updateMultiPhase(NESphUpdatePart currentStep);
    //! Computes the required time step and sets it
    virtual qreal updateTimeStep();
    //! Overloaded function, to additionally save the timestep
    virtual uint saveFrameData(QString path, const int frameNumber);
    //! Overloaded function, to additionally load the timestep
    virtual uint loadFrameData(QString path, const int frameNumber);
    //! Overloaded function for modifying fluid radius
    void setRadius(qreal r);
    //! Overloaded function for modifying rest density of the fluid
    void setRestDensity(qreal density);

    int iterations() { return m_MinNumIters; }
    void setIterations(int count);




protected:
    //! Computes the forces and the resulting acceleration for the particle.
    void computeAcceleration(const int particleVecId);
    //! Computes the pressure (scalar value) of the particle.
    inline void computePressure(const int particleVecId);
    void resizeParticlesVector(uint nItems);
    void reserveSpaceForParticles(uint nItems);
    //!
    void computeForcesWithoutPressure();

    void recomputeDistancesWithNeighbors(int pIndex);
    float recomputeDensityWithUpdatedNeighbors(int pIndex);

    void recomputeDistancesWithNeighbors(NERigidSphParticle& bParticle);

    virtual void boundaryCollision(NEPhysicalParticle* particle, vec3f& pos, bool correctVel);


private:


    std::vector<NESampleSphParticle> m_Particles;
    NEVector<NESampleSphParticle> m_ParticleNEVector;

    //! Returns the volume of the fluid in m^3 (according to the restdensity)
    float getRestVolume();

};


// The factory class
class NESampleSimpleFactory : public NENodeFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NENodeFactory")
    Q_INTERFACES(NENodeFactory)

public:

    NESampleSimpleFactory(NEManager* m = 0);
    ~NESampleSimpleFactory();
    NE_DERIVED(NENodeFactory, Public)

};
