#pragma once

#include "NEPostProcessor.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NEMeshPostProcessor), NEMarchingCubes, NEIndexedTriMesh)

class NECEXP_NEPOSTPROCESSOR NEMeshPostProcessor : public NEPostProcessor
{
    Q_OBJECT

public:
    enum NEMeshRenderingType {Polygons = 0, SubdivisionSurface = 1};

    NE_PROP_RW(bool, SaveBinaryMesh, "Save Binary Mesh", "", true,)
    NE_PROP_RW(bool, DrawNormals, "Draw Normals", "", true,)
    NE_PROP_RW(bool, ShowTimings, "Show Timings", "", true,)
    NE_PROP_RW(bool, ShowStatistics, "Show Statistics", "", true,)
    NE_PROP_GROUP_START(ExportRelated, "Export Related", "", true)
    NE_PROP_RW(bool, ExportTextureCoord, "Export Texture Coordinates", "", true,)
    NE_PROP_RW(bool, ExportVelocities, "Export Velocities", "", true,)
    NE_PROP_RW(bool, ExportSplashes, "Export Splashes", "", true,)
    NE_PROP_GROUP_END(ExportRelated)


    Q_ENUMS(NEMeshRenderingType)

public:
     NEMeshPostProcessor(NENodeFactory* val);
    virtual ~NEMeshPostProcessor();

    //! The main update method of the post processor, called from outside
    virtual uint update() = 0;
    virtual  uint init(uint initMode = NE::kInitOnCreate);
    virtual  void render();
    virtual  void renderPickable();
    virtual  void renderPicked();
    uint saveFrameData(QString path);
    uint loadFrameData(QString path);

    NEMarchingCubes* inputMC() const;
    void setInputMC(NEMarchingCubes* val);
    NEIndexedTriMesh* outputMesh() const;
    void setOutputMesh(NEIndexedTriMesh* val);

    virtual  void deformMesh() = 0;

public slots:

    uint SetPosition(const vec3f& Position);

protected:
    virtual bool addSource(NENode* node, NEConn &conn);

    virtual void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);

private:
    NE_DECL_PRIVATE(NEMeshPostProcessor)
};
