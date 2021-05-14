#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_6(class, NESpatial, NERigid, NERigidSolver, NERigidParticle, NELink, NE_PRIVATE(NERigidManager))

//!  Manages all Rigid instances.
/*!
	Rigids can only be instantiated by this class using createObject(QString,QString).
*/

class NERigidManager final : public NEPhysicalEntityManager
{
	Q_OBJECT

public:
    NERigidManager(QObject* parent);
    virtual ~NERigidManager();

    //! Creates a new Rigid instance of type and name.
    NENode* createObject(QString type, QString name, const NEObject* parent);

    //! Deletes an object and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);

    //! Deletes an object and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name);

    uint init();

    uint deInit();

    uint prepareRigidsForSimulation();

    uint reset();

    //! Does some pre-processing
    //! Called at the start of each simulation step
    void preProcess();

    //! Computes the kernel sums of the solid particles
    //! Called between the collider update and the SPH computations
    void computeKernelSums();

    //! Updates the forces applied on the rigids
    void updateForces();

    void syncRigids();

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

    //! Returns the memory consumption of the particles of the rigids
    uint memoryConsumption();

    //! Returns the list of rigids
    const std::vector<NERigid*>& rigids() const;
    //! Returns the list of rigid-body solvers
    const std::vector<NERigidSolver*>& rigidSolvers() const;

    //! Returns (as parameter) all rigid particles listed per active rigid
    void rigidParticles(std::vector<std::vector<NERigidParticle*>>& rParticles) const;

public slots:
    void linkChangedCallback(const NELink* link);

private:
    NE_DECL_PRIVATE(NERigidManager)
};
