#pragma once

#include "Manager/NEGeometryManager.h"

NE_FWD_DECL_2(class, NEMeshConstructor, NE_PRIVATE(NEMeshVolGeometryManager))

//! Geometry Manager holds pointers to all geometries in the scene.
/*!
	A more elaborate class description.
*/
class NECEXP_GEOMETRY NEMeshVolGeometryManager  : public NEGeometryManager
{
public:
    NEMeshVolGeometryManager( QObject* scene );
    virtual ~NEMeshVolGeometryManager();

    //! Update called during simulation update
    uint update();

    //! Creates a new Geometry instance.
	NENode* createObject(QString, QString, const NEObject*);

    //! Deletes the geometry instance and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
    NENode* deleteObject(const QString& type ,const QString& name);

    void deleteChildNode(NENode* node);
    //! Initializes the instance. Load all available plugins and static of simulation classes.
    virtual uint init();
    void deInit();

    //! Loads stored data of simulation objects for the current frame.
    virtual uint loadFrameData(QString dataPath);

	//! Stores data of simulation objects for the current frame.
    virtual uint saveFrameData(QString dataPath);

    //! Deletes all stored data
     virtual uint cleanSavedData(QString dataPath);

    //! Deletes all stored data
    virtual uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
    virtual bool hasActiveGeometryConstructors();

private:
    NE_DECL_PRIVATE(NEMeshVolGeometryManager)
};
