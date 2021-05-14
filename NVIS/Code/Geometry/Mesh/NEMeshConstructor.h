#pragma once

#include "Spatial/NESpatial.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"

NE_FWD_DECL_2(class, NE_PRIVATE(NEMeshConstructor), NEIndexedTriMesh)

class Q_DECL_IMPORT NESpatial;

class NECEXP_GEOMETRY NEMeshConstructor : public NESpatial
{
    Q_OBJECT
    NE_CLASS("Mesh Constructor General Settings")

public:
    enum NEMeshRenderingType {Polygons = 0, SubdivisionSurface = 1};
    enum NEMeshSaveVersion {Version_V1 = 1, Version_V2 = 2};

    NE_PROP_RW(bool, ShowTimings, "Show Timings", "If enabled, timings of all computation steps are shown.", true,)
    Q_ENUMS(NEMeshRenderingType NEMeshSaveVersion)

public:
    NEMeshConstructor(NENodeFactory* val);
    virtual ~NEMeshConstructor();

    //! Update method called from geometry manager.
    virtual void update() ;

    virtual uint init(uint initMode = NE::kInitOnCreate);
    virtual NEMeshSaveVersion saveVersion() { return m_saveVersion; }
    virtual void setSaveVersion(uint versionNumber) {  m_saveVersion = (NEMeshSaveVersion)versionNumber; }

    virtual void render(); //!< These are not called. Need to check again.
    virtual void renderPickable();
    virtual void renderPicked();

    virtual uint saveFrameData(QString dataPath) ;
    virtual uint loadFrameData(QString dataPath) ;
    virtual void resetProperties();

    const NEIndexedTriMesh& triangleMesh() const;

signals:
    void updateDrawingBuffers();


protected:
    //! The reconstructed mesh.
    NEIndexedTriMesh m_TriangleMesh;
    NEMeshSaveVersion m_saveVersion;

private:
    NE_DECL_PRIVATE(NEMeshConstructor)
};


Q_DECLARE_INTERFACE(NEMeshConstructor, "NEMeshConstructor")
