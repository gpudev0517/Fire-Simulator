#pragma once
#include "Utility/FilePath.h"
#include "Spatial/NESpatial.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NENodeFactory.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"

NE_FWD_DECL_4(class, NEIndexedTriMesh, NEMeshContainer, FilePath, NE_PRIVATE(NESpatialMesh))
NE_FWD_DECL_1(struct, NEPrimitivePolygon)

class NECEXP_GEOMETRY NESpatialMesh : public NESpatialGroup
{
    Q_OBJECT
    NE_CLASS("General Mesh Options")

public:
    enum NEMeshRenderingType {Polygons = 0, SubdivisionSurface = 1};
    enum NESpatialMeshType { Box = 0, Cuboid = 1, Plane = 2, Custom = 3, Sphere = 4, Cylinder = 5, CADMesh = 6, None = 7 };

    NE_PROP_RW(bool, FlipMesh, "Invert Normals", "Inverts the normals and vertex order", true, )
    NE_PROP_RW(bool, TwoSidedLighting, "Two-Side Lighting", "Gives the dark sides of the objects a diffuse color in case of a single positional light source", true,)

    NE_PROP_RW(NESpatialMeshType, SpatialMeshType, "Spatial Mesh Type", "Type of Spatial Mesh - different kinds are possible", true,)

    NE_SUB_PROP_START(SetSpatialMeshType, SpatialMeshType)
    NE_PROP_RW(FilePath, CADFile, "CAD File Path", "Choose the path of the file to be refered to for a CAD Mesh.", isCADMesh,)
    NE_PROP_RW(bool, SwapUpAxes,  "Swap Y/Z Axis", "Swaps the object's local Y/Z axes.", isCADMesh,)
    NE_PROP_RW(bool, FlipXAxis, "Flip X Axis", "Flips the X Axis.", isCADMesh,)
    NE_PROP_RW(bool, FlipYAxis, "Flip Y Axis", "Flips the Y Axis.", isCADMesh,)
    NE_PROP_RW(bool, FlipZAxis, "Flip Z Axis", "Flips the Z Axis.", isCADMesh,)
    NE_SUB_PROP_END(SetSpatialMeshType)

    NE_PROP_RW(double, UnitScale, "Unit Scale", "scale multiplied before all transform", true, )
    NE_PROP_RW(double, Thickness, "Thickness", "Thickness", true, )

    NE_PROP_RW(bool, TopCap, "Top Cap", "Should the cylinder have a top cap.", isCylinder,)
    NE_PROP_RW(bool, BottomCap, "Bottom Cap", "Should the cylinder have a bottom cap.", isCylinder,)
    NE_PROP_RW(vec3f, CylAxis, "Axis", "Axis of orientation of the cylinder.", isCylinder,)


    Q_ENUMS(NESpatialMeshType)

public:
    NESpatialMesh( NENodeFactory* factory);
    virtual ~NESpatialMesh();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    virtual bool updateAABB();

    //! Returns the triangle mesh of the rigid
    NEMeshContainer& meshContainer();
    virtual NEIndexedTriMesh& triangleMesh() override;
    const NEIndexedTriMesh& triangleMesh() const;
    void setTriMesh(const NEIndexedTriMesh& val);

    std::vector<NEPrimitivePolygon>& quadFaces();

    virtual const vec3f& centerOfMass() const;
    virtual const matrix33d InertiaTensor() const; //undefined

    vec3f findClosetFaceAndIntersectionPoint(QList<NESpatialMesh *> boundaries, vec3f rayStart, vec3f rayEnd, QPair<NESpatialMesh *, int> &selected, QPair<NESpatialMesh *, int> &targeted);
    vec3f findClosetFaceAndIntersectionPointAndRotate(QList<NESpatialMesh *> boundaries, vec3f rayStart, vec3f rayEnd, QPair<NESpatialMesh *, int> &selected, QPair<NESpatialMesh *, int> &targeted, vec3f &otherNormal);

    bool isCADMesh();
    bool isCylinder();

    void renderPickable() override;
    void render() override;

    void renderPicked() override;

    void initializeGUIActions() override;

    double oldThickness();
    void setOldThickness(double t);
    void addMeasureObject(int num);
    std::vector<int> getMeasureObjects();

signals:
    void triMeshChanged();
    void updateDrawingBuffers();
    void initObject();

private slots:
    void setThickness();

public slots:

    uint SetCenterMethod(const NESpatial::NECenterCalculation& val) override;
    uint SetLocalizedTransforms(const bool& val) override;

    void reloadCADFile();
    void clearCADFile();

protected:

    uint localizeTransforms() override;
    uint relocalizeTransforms() override;
    uint freezeTransforms() override;
    uint computeCenter() override;

    uint applyUnitScale(double unitScale, bool appliedBeforLocalizing);
    uint setUnitScale(const double& val);

private:
    NE_DECL_PRIVATE(NESpatialMesh)

};


// The factory class
class NESpatialMeshFactory : public NENodeFactory
{
    Q_PLUGIN_METADATA(IID "NENodeFactory")
    Q_INTERFACES(NENodeFactory)

public:
    NESpatialMeshFactory(NEManager* m = 0);
    ~NESpatialMeshFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames();
};


