#pragma once

#include "Manager/NEManager.h"


//! Geometry Manager holds pointers to all geometry in the scene.
/*!
	A more elaborate class description.
*/
class  NECEXP_CORE NEGeometryManager : public NEManager
{
    Q_OBJECT
public:
	NEGeometryManager(QObject* scene);
	virtual ~NEGeometryManager();

    //! Update called during simulation update
    virtual uint update();

    //! Creates a new Geometry instance.
    virtual NENode* createObject(QString, QString, const NEObject*);

    //! Deletes the geometry instance and removes all dependencies.
    virtual NENode* deleteObject( const QString& type, const QString& name, uint trashIndex );
    virtual NENode* deleteObject( const QString& type, const QString& name );

	//! Initializes the instance. Load all available plugins and static of simulation classes.
    virtual uint init();

    //! Loads stored data of simulation objects for the current frame.
    virtual uint loadFrameData(QString dataPath);

    //! Stores data of simulation objects for the current frame.
    virtual uint saveFrameData(QString dataPath);

    //! Deletes all stored data
    virtual uint cleanSavedData(QString dataPath);

    //! Deletes all stored data
    virtual uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
    virtual bool hasActiveGeometryConstructors();

    //! Allocate GPU buffers for the object
    void createDrawingBuffers(NEIndexedTriMesh *mesh, uint mode);
    //! Refresh GPU buffer contents for the object (the mode can also be changed)
    void rehintDrawingBuffers(NEIndexedTriMesh *mesh, uint mode);
    //! Clear drawing buffers for the object
    void clearDrawingBuffers(NEIndexedTriMesh *mesh);
    //!
    void updateDrawingBuffers(NEIndexedTriMesh *mesh, uint mode);

    void calcNormalsMWA(NEIndexedTriMesh *mesh);
    void calcNormalsMWASEL(NEIndexedTriMesh *mesh);
    void calcNormalsMWE(NEIndexedTriMesh *mesh);

signals:
    void createDrawingBuffers_Signal(NEIndexedTriMesh *mesh, uint mode);
    void rehintDrawingBuffers_Signal(NEIndexedTriMesh *mesh, uint mode);
    void clearDrawingBuffers_Signal(NEIndexedTriMesh *mesh);
    void updateDrawingBuffers_Signal(NEIndexedTriMesh *mesh, uint mode);

    void calcNormalsMWA_Signal(NEIndexedTriMesh *mesh);
    void calcNormalsMWASEL_Signal(NEIndexedTriMesh *mesh);
    void calcNormalsMWE_Signal(NEIndexedTriMesh *mesh);
};
