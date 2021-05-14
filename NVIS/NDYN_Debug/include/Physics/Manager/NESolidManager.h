#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_5(class, NERigidParticle, NERigidManager, NERigid, NERigidSolver, NE_PRIVATE(NESolidManager))
#if NE_DEFORMABLE
NE_FWD_DECL_2(class, NEDeformableManager, NEDeformable)
#endif



//!  Manages all solid body instances, which may be rigid bodies or deformable bodies.

class NECEXP_PHYSICS NESolidManager final : public NEPhysicalEntityManager //NEManager
{
    Q_OBJECT

public:
    NESolidManager(QObject* parent);
    virtual ~NESolidManager();
    uint init() override;
    uint deInit();
    uint reset();
    //! Prepares the solids before simulation
    uint prepareSolidsForSimulation();

    //! Does some pre-processing
    //! Called at the start of each simulation step
    void preProcess();
    //! Computes the kernel sums of the solid particles
    //! Called between the collider update and the SPH computations
    void computeKernelSums();
    //! Updates the forces applied on the solids
    void updateForces();
    //! Synchronizes the solid solver and the solids
    void syncSolids();

    //! Samples the mesh-based solids with particles
    void sampleSolidMeshes();
    //! Unsamples the mesh-based solids that have particles
    void unsampleSolidMeshes();
    //// SAVING AND LOADING OF DATA /////
    //! Loads stored data of fluids for the current frame.
    double loadFrameData(QString dataPath, int frameNumber);
    //! Stores data of fluids for the current frame.
    double saveFrameData(QString dataPath, int frameNumber);
    //! Deletes all data
    uint cleanSavedData(QString dataPath);
    //! Deletes all data at the end or at the beginning
    uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
    //! Writes the cache information of each solver into fout.
    uint writeFrameInfo(QXmlStreamWriter& xmlSW, int frameNumber);
    //! Sets all node behaviors to either cache (b = true) or active (b = false)
    void setCacheMode(bool b);

    //! Returns a pointer to the manager of rigid bodies
    std::shared_ptr<NERigidManager> rigidManager();
    //! Returns the list of rigid bodies
    const std::vector<NERigid*>& rigids() const;
    //! Returns the list of rigid-body solvers
    const std::vector<NERigidSolver*>& rigidSolvers() const;
    //! Returns (as parameter) all solid particles listed per active solid
    void solidParticles(std::vector<std::vector<NERigidParticle*>>& rParticles) const;

#if NE_DEFORMABLE
    //! Returns a pointer to the manager of deformable bodies
    std::shared_ptr<NEDeformableManager> deformableManager();
    //! Returns a list of deformable bodies
    QList<NEDeformable*>& deformables();
#endif


signals:
    //! Signal after all the various sub managers are created
    void managersCreated(QList<std::shared_ptr<NEManager>> managerList);

private:
    NE_DECL_PRIVATE(NESolidManager)
};
