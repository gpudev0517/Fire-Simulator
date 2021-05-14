#pragma once

#include "Manager/NEManager.h"
#include "Collider/NECollider.h"

NE_FWD_DECL_6(class, NE_PRIVATE(NECollisionManager), NESpatial, NEParticle, NERigid, NECollider, NEParticleFluidSolver)
NE_FWD_DECL_1(struct, Neighbor)

//! The collision manager manages the colliders, which are used for neighbordhood search and collision detection.

class NECEXP_PHYSICS NECollisionManager final : public NEManager
{
    Q_OBJECT

public:
    NECollisionManager();
    ~NECollisionManager();

    static NECollisionManager& Instance();

    //! Initializes the collision manager
    uint init();
    //! Deinitializes the collision manager
    uint deInit();

    uint deLoadInit();

    //! Does nothing
    NENode* createObject(QString type, QString name, const NEObject* parent=0) override;

    //! Returns the collider
    //! Currently only one collider can be used at a time
    NECollider* collider() const;

    //! Creates the collider
    NECollider* createCollider();
    //! Resets the collider, including the stats and timers
    void resetCollider();
    //! Clears the collider
    void clearCollider();

    //! Does some preprocessing
    //! Called at the start of each time step
    void preProcess();
    //! Updates the collider
    uint updateCollider(bool avoidReorder=false);
    //! Does some postprocessing
    //! Called at the end of each time step
    void postProcess();

    //! Queries and stores all neighbors of the fluid particles handled by fluidSolver
    void queryAndStoreNeighbors(NEParticleFluidSolver* fluidSolver) const;
    //! Queries the solid neighbors of particle p and returns them (as parameter) in sNeighborList
    void querySolidNeighbors(NEParticle* p, std::vector<Neighbor>& sNeighborList) const;

    //! Loads stored data for the current frame
    double loadFrameData(QString dataPath, int frameNumber);
    //! Stores data for the current frame
    double saveFrameData(QString dataPath, int frameNumber);
    //! Deletes all data
    uint cleanSavedData(QString dataPath);

public slots:
    //! Adds the newly created physical object to the collider
    void physicalObjectCreated(NE::NEObjectType, NESpatial*);
    //! Removes the newly created physical object to the collider
    void physicalObjectDeleted(NE::NEObjectType type, NESpatial* object);

signals:
    void updateSceneInspector();

private:
    NE_DECL_PRIVATE(NECollisionManager)

};

#define NECOLMGR NECollisionManager::Instance()
