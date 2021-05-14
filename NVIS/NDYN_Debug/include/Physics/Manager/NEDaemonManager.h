#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_6(class, NE_PRIVATE(NEDaemonManager), NEParticle, NEFluidParticle, NESource, NEKiller, NEDualBoundary)

class NECEXP_PHYSICS NEDaemonManager final : public NEPhysicalEntityManager
{
    Q_OBJECT

public:
    NEDaemonManager(QObject* parent);
    virtual ~NEDaemonManager();

    //! Does some initialization
    //! Called at the start of the session
    uint init() override;
	//! Does some de-initialization
	//! Called at the end of the session
    uint deInit();
    //! Resets some values
    //! Called when restarting the simulation from the start
    void reset();

    //! Creates a node and adds appropriate dependencies
    NENode* createObject(QString, QString, const NEObject*);
    //! Deletes a node and removes all dependencies
    NENode* deleteObject(const QString& type, const QString& name, uint trashIndex);
    NENode* deleteObject(const QString& type, const QString& name);

    //! Loads cached data of daemon nodes for the current frame
    double loadFrameData(QString dataPath, int frameNumber);
    //! Stores data of daemon nodes for the current frame
    double saveFrameData(QString dataPath, int frameNumber);
    //! Deletes all cached data of daemon nodes
    uint cleanSavedData(QString dataPath);
    //! Deletes all data of daemon nodes at the end or at the start
    uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame = false);


    //! Does some pre-processing
    //! Called at the start of each simulation step
    void preProcess();
    //! For each fluid particle and solid particle, decides whether to "kill" it or not;
    //! if to kill, either marks it for deletion or transforms it into an outflow ghost (fluid only)
    //! Called at each time step
    void checkAndMarkToKill();
    //! Generates particles and aff them to the connected fluid solver
    //! Called at each time step
    void generateParticles();
    //! Does some post-processing
    //! Called at the end of each simulation step
    void postProcess();

    //! Sets all node behaviors to either inactive (b = true) or active (b = false)
    void setInactiveMode(bool b);

    //! Returns the list of sources
    std::forward_list<NESource*> sources() const;
    //! Returns the list of killers
    std::forward_list<NEKiller*> killers() const;
    //! Returns the list of dual boundaries
    std::forward_list<NEDualBoundary*> dualBoundaries() const;

private:
    NE_DECL_PRIVATE(NEDaemonManager)
};
