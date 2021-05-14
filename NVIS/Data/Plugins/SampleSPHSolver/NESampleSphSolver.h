#pragma once

#include "Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"
#include "NESampleSphParticle.h"
#include "Base/NENodeFactory.h"


//!  Implementation of the Predictive-Corrective Incompressible SPH.
/*!
  This class extends the basic PCISPH algorithm proposed by Solenthaler (SIGGRAPH,2009) with an adaptive formulation.
*/
class NECEXP NESampleSphSolver : public NESphLiquidSolver
{
    Q_INTERFACES(NESphLiquidSolver)
    Q_OBJECT


    Q_PROPERTY(int iterations READ iterations WRITE setIterations)
    Q_PROPERTY(int maxIter READ maxIter WRITE setMaxIter)
    Q_PROPERTY(PCISPHModel pci_model READ pcisphModel WRITE setPcisphModel)
    Q_PROPERTY(bool isolatedDamping READ isolatedDamping  WRITE setIsolatedDamping)
    Q_PROPERTY(qreal isoDampingFactor READ isoDampingFactor WRITE setIsoDampingFactor)
    Q_PROPERTY(qreal maxDensityError READ maxDensityErrorUser WRITE setMaxDensityErrorUser)
    Q_CLASSINFO("help-maxDensityError", "The maximum density error the user tolerates given in percentage.")
    Q_PROPERTY(qreal avgDensityError READ avgDensityYieldError WRITE setAvgDensityYieldError)
    Q_CLASSINFO("help-avgDensityError", "The average density error the user tolerates given in percentage.")
    Q_PROPERTY(PCIStopCriterion stopCriterion READ stopCriterion WRITE setStopCriterion)
    Q_ENUMS(PCISPHModel)
    Q_ENUMS(PCIStopCriterion)
public:

    enum PCISPHModel
    {
        pciModelAkinci =1,
        pciModelIhmsen = 2,
        pciModelSolenthaler = 3
    };

    enum PCIStopCriterion
    {
        scAvgDensity = 1,
        scMaxDensity = 4
    };

    //  NESampleSphSolver() { };

    NESampleSphSolver(NENodeFactory* factory);
    ~NESampleSphSolver();

    uint init(uint initMode = NE::kInitOnCreate);

    //! Deletes all marked particles out of the vector.
    virtual void deleteParticles();
    //! Deletes the particle
    virtual void deleteParticle(NEParticle* particle);

    //! Returns true if explicit boundary handling is required used when integrating particles.
    virtual bool explicitBoundaryHandling() const;
    //! Compute and returns the optimal timestep for this solver, also sets the number of frames to go back
    virtual qreal computeOptimalTimestep(int& framesToGoBack );
    //! Returns the memory consumption of the particles
    virtual uint memoryConsumption();

    virtual NEParticleFluid::NERigidParticleType rigidParticleType() { return NEParticleFluid::PciSphParticle; }


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

    PCISPHModel pcisphModel() const { return m_PcisphModel; }
    void setPcisphModel(PCISPHModel val) {m_PcisphModel = val;}

    bool isolatedDamping() const { return m_IsolatedDamping; }
    void setIsolatedDamping(bool val) { m_IsolatedDamping = val; }

    float isoDampingFactor() const { return m_IsoDampingFactor; }
    void setIsoDampingFactor(float val) { m_IsoDampingFactor = val; }

    qreal maxDensityErrorUser() const {return m_MaxDensErrUser;}
    void setMaxDensityErrorUser(qreal val) {m_MaxDensErrUser = val;}

    int maxIter() const { return m_MaxNumIters; }
    void setMaxIter(int val) { m_MaxNumIters = val; }

    qreal avgDensityYieldError() const {return m_AvgDensErrUser;}
    void setAvgDensityYieldError(qreal val) {m_AvgDensErrUser = val;}

    PCIStopCriterion stopCriterion() const {return m_StopCriterion;}
    void setStopCriterion(PCIStopCriterion val)  {m_StopCriterion = val;}

protected:
    //! Computes the forces and the resulting acceleration for the particle.
    void computeAcceleration(const int particleVecId);
    //! Computes the pressure (scalar value) of the particle.
    inline void computePressure(const int particleVecId);
    void resizeParticlesVector(uint nItems);
    void reserveSpaceForParticles(uint nItems);
    //!
    void computeForcesWithoutPressure();
    //!Implements the new boundary handling for PCISPH
    void iterativePressureSolveAkinci();
    //!Implements the new boundary handling for multiphases
    void iterativePressureSolveMultiAkinci();
    //!Implements the PCISPH loop as explained in IAGT10
    void iterativePressureSolveIhmsen();
    //!Implements the PCISPH loop as explained in SP09
    void iterativePressureSolveSolenthaler();
    void recomputeDistancesWithNeighbors(int pIndex);
    float recomputeDensityWithUpdatedNeighbors(int pIndex);

    void recomputeDistancesWithNeighbors(NERigidSphParticle& bParticle);

    virtual void boundaryCollision(NEPhysicalParticle* particle, vec3f& pos, bool correctVel);


private:
    void computePrototypeDelta();
    //! Updates the delta value according to the current time step
    void updateDelta();
    void computeAcceleration_V_G_EXT(const int particleVecId);
    //void computeParticleDensity( int pIndex );
    //void computeParticleDensity(NERigidSphParticle& bParticle);
    void computeAcceleration_P( int pIndex );

    void computeAcceleration_P_oldPosition( int pIndex );

    //! Computes acceleration due to all forces except the pressure force.
    void computeMultiAcceleration_NoPressure(const int particleVecId);
    //! Computes acceleration due to pressure forces only.
    void computeMultiAcceleration_Pressure(const int particleVecId);

    std::vector<NESampleSphParticle> m_Particles;
    NEVector<NESampleSphParticle> m_ParticleNEVector;

    //! Returns the volume of the fluid in m^3 (according to the restdensity)
    float getRestVolume();

    uint m_MinNumIters;	// Minimum number of iterations in convergence loop
    uint m_MaxNumIters; // Maximum number of iterations in convergence loop
    //! Current iteration step, required for multiphase handling
    uint m_CurrentIter;
    //! Total number of iterations
    int m_TotalNrOfIterations;
    //! The user defined maximum density error
    qreal m_MaxDensErrUser;
    //!
    NESphUpdatePart m_IterativePart;
    float m_PrototypeDelta;		// Pcisph delta value for the current timestep
    float m_PrototypeDeltaWOTimestep;  // Pcisph delta value excluding timestep (for adaptive timestep pcisph)
    bool m_OldDeltaInitialized;
    double m_NewDelta;
    double m_OldDelta;
    bool m_ShockOccured;
    double m_ShockInstant;
    double m_LastTimestep;
    bool m_FrameChangedByUser;

    bool m_IsolatedDamping;
    float m_IsoDampingFactor;

    double m_OldAveDensity;

    //!
    PCISPHModel m_PcisphModel;
    qreal m_AvgDensErrUser;
    PCIStopCriterion m_StopCriterion;
    //! Time for one pressure iteration
    qreal m_PressureIterationTime;
    //! Total time for solving the pressure.
    qreal m_TotalPressureTime;
};


// The factory class
class NESampleSphFactory : public NENodeFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NENodeFactory")
    Q_INTERFACES(NENodeFactory)

public:
//    NEPciSphFactory();
    NESampleSphFactory(NEManager* m = 0);
    ~NESampleSphFactory();
    NE_DERIVED(NENodeFactory, Public)

};
