#pragma once

#include "NENode.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NECollider), NEParticleFluidSolver, NESphRigid)
NE_FWD_DECL_3(class, NEFluidParticle, NERigidSphParticle, NEParticle)
struct Neighbor;

// Debug flag
#define DEBUG_COLLIDER 1

//! The base collider class.
/*!
    This is the base class for particle-particle neighborhood search nodes
*/
class NECEXP_PHYSICS NECollider : public NENode
{
    Q_OBJECT
    NE_PROP_RW(bool, ShowTimings, "Display Timings", "Computes and displays simulation timings.", true, )
    NE_PROP_RW(bool, ShowStatistics ,"Display Statistics", "Computes and displays simulation statistics.", true, )
    // TODO: Must be moved to derived classes
    NE_PROP_RW(int, FluidNeighborsVectorLimit, "Fluid Neighbors Limit", "Specifies the initial reserved space for keeping fluid neighborhood information.", true, )

public:
    NECollider(NENodeFactory* factory);
    virtual ~NECollider();

    //! Initializes the collider
    virtual uint init(uint initMode = NE::kInitOnCreate);
    //! Resets the collider
    virtual void reset();
    //! Resets the properties with their default values
    virtual void resetProperties();

    //! Does some preprocessing
    //! Called at the start of each time step
    virtual void preProcess();
    //! Updates the collider
    virtual void update(bool avoidReorder = false);
    //! Does some postprocessing
    //! Called at the end of each time step
    virtual void postProcess();

    //! Clears the acceleration data structure
    virtual void clearAccDataStructure();

    //! Queries the neighbors of particle p and returns them (as parameter) in
    //! fNeighborList and sNeighborList for, respectively, the fluid and solid neighbors
    virtual void queryNeighbors(NEParticle* p, std::vector<Neighbor>& fNeighborList, std::vector<Neighbor>& sNeighborList);
    //! Queries the solid neighbors of particle p and returns them (as parameter) in sNeighborList
    virtual void querySolidNeighbors(NEParticle* p, std::vector<Neighbor>& sNeighborList);
    //! Queries the neighbors for each particle of fluidSolver
    //! and stores them within the fluid and solid neighbor lists
    virtual void queryAndStoreNeighbors(NEParticleFluidSolver* fluidSolver);
    //! Queries the neighbors for each particle and stores
    //! them within the fluid and solid neighbor lists
    virtual void queryAndStoreNeighbors();

    //! Returns the active rigid particles vector
    std::vector<NERigidSphParticle*>* activeRigidParticles();

    //! Returns the list of connected fluid solvers
    const std::forward_list<NEParticleFluidSolver*>& connectedFluidSolvers() const;
    //! Returns the list of connected rigid objects
    const std::vector<NESphRigid*>& connectedRigids() const;

    //! Adds a fluid solver to the list of fluid solvers connected to the collider
    virtual uint addFluidSolver(const NEParticleFluidSolver* fluidSolver);
    //! Adds a rigid object to the list of rigid objects connected to the collider
    virtual uint addRigid(const NESphRigid* rigid);
    //! Removes a fluid solver to the list of fluid solvers connected to the collider
    virtual uint removeFluidSolver(const NEParticleFluidSolver* fluidSolver);
    //! Removes a rigid object to the list of rigid objects connected to the collider
    virtual uint removeRigid(const NESphRigid* rigid);

    //! Saves the loading of data for the current simulation/animation frame.
    uint saveFrameData(QString path, const int frameNumber);
    //! Calls the loading of data for the current simulation/animation frame.
    uint loadFrameData(QString path, const int frameNumber);

    //! Returns the time spent on building the collider data structures for the current/past update
    double buildColliderUpdateTime() const;
    //! Returns the total time spent on building the collider data structures
    double buildColliderTotalTime() const;
    //! Returns the time spent on querying the neighbor particles for the current/past update
    double queryNeighborsUpdateTime() const;
    //! Returns the total time spent on querying the neighbor particles
    double queryNeighborsTotalTime() const;

public slots:
    //! Callback when the interaction radius of a connected fluid solver changes
    virtual void interRadiusChangedCallback();
    //! Callback when the particles of the connected rigid are getting deleted
    virtual void deletingSolidParticlesCallback(NESphRigid* rigid);
    //! Callback when the fluid particles fParticles of a connected fluid solver are getting deleted
    virtual void deletingFluidParticlesCallback(std::vector<NEFluidParticle*>& fParticles);

protected:
    //! Returns whether rebuilding the collider is required or not
    bool isRebuildRequired();

    //! Adds an active rigid particle
    void addActiveRigidParticle(NERigidSphParticle* p);
    //! Cleans the active rigid particles and their neighborhood information
    void resetActiveRigidParticles();

    //! Accumulates the time spent on building the collider data structures
    void accumulateBuildColliderTime(double val);
    //! Accumulates the time spent on querying the neighbor particles
    void accumulateQueryNeighborsTime(double val);

private:
    NE_DECL_PRIVATE(NECollider)
};

Q_DECLARE_INTERFACE(NECollider, "NECollider")
