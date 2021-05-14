#pragma once
#include "Utility/FilePath.h"
#include "Utility/NESurfaceMaterial.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NENodeFactory.h"
#include "Base/NESceneManager.h"
#include "Mesh/NEZone.h"
#include "Rendering/Spatial/NEFloorPlane.h"

#include <QImage>


NE_FWD_DECL_5(class, FZoneItem, FScenario, FilePath, FZoneCompartment, NE_PRIVATE(FZone))
NE_FWD_DECL_1(class, NEFloorPlane)

class FDYNCEXP_PLUGINS FZone : public NEZone
{
    Q_OBJECT
    NE_CLASS("Fire Zone Properties")
public:
    enum FSwapDegree { Original = 0, degree_90 = 1, degree_180 = 2, degree_270 = 3 };
    enum NELineType { Solid = 0, Dash = 1, Dot = 2};
    enum FFloorPlaneEffect { Transparency = 0, AmbientOcclusion = 1 };
    NE_PROP_RW(NELineType, LineType, "Line Type", "boundary line Type", true,)

    NE_PROP_GROUP_START(FloorPlanUse, "Floor Plan", "Floor Plan", true)
    NE_PROP_RW(bool, ShowFloorPlan, "Floor Plan", "Show/hide Floor Plan.", true, )
    NE_PROP_RW(double, FloorElevation, "Floor elevation", "Floor elevation", true,)
    NE_PROP_RW(FilePath, FloorPlan, "Floor texture", "Floor texture", true,)
    NE_PROP_GROUP_START(FloorPlaneEffectUse, "Floor Rendering Effect", "Floor Rendering Effect", true)
    NE_PROP_RW(FFloorPlaneEffect, FloorPlaneEffect, "Effect", "Floor Rendering Effect", true,)
    NE_PROP_RW(double, FloorPlanTransparency, "Transparency", "Floor Plan Transparency", transparencyVisible,)
    NE_PROP_GROUP_END(FloorPlaneEffectUse)
    NE_PROP_RW(double, XMin, "X Min", "X Min", true,)
    NE_PROP_RW(double, XMax, "X Max", "X Max", true,)
    NE_PROP_RW(double, XSize, "X Size", "X Size", true,)
    NE_PROP_RW(double, ZMin, "Z Min", "Z Min", true,)
    NE_PROP_RW(double, ZMax, "Z Max", "Z Max", true,)
    NE_PROP_RW(double, ZSize, "Z Size", "Z Size", true,)
    NE_PROP_RW(FSwapDegree, Swap, "Swap", "Swap", true,)
    NE_PROP_RW(bool, Flip, "Flip", "Flip", true,)
    NE_PROP_GROUP_END(FloorPlanUse)

    NE_PROP_RW(NESurfaceMaterial, CeilingMaterial, "Ceiling Material", "Ceiling Material", true,)
    NE_PROP_RW(NESurfaceMaterial, FloorMaterial, "Floor Material", "Floor Material", true,)
    NE_PROP_RW(NESurfaceMaterial, WallMaterial, "Wall Material", "Wall Material", true,)
    NE_PROP_RW(double, CeilingThickness, "Ceiling Thickness", "Ceiling Thickness", false,)
    NE_PROP_RW(double, FloorThickness, "Floor Thickness", "Floor Thickness", false,)
    NE_PROP_RW(double, WallThickness, "Wall Thickness", "Wall Thickness", false,)
    NE_PROP_RW(double, WallLeakRatio, "Wall Leak Ratio", "Wall Leak Ratio", true,)
    NE_PROP_RW(double, FloorLeakRatio, "Floor Leak Ratio", "Floor Leak Ratio", true,)

    NE_PROP_RW(uint, ConnectorID, "Connector ID", "Zone Connection ID", false, )
    NE_PROP_RW(QString, Issues, "Issues", "Zone Issues not convered prior", true, )
    NE_PROP_RW(bool, ShowCompBB, "Box", "Show/hide the axis-aligned bounding box weighted by the compartment boundaries.", true, )
    NE_PROP_3D_R(vec3f, float, CompBBSize, "Box Size", "Size of the axis-aligned bounding box", true)
    NE_PROP_3D_R(vec3f, float, CompBBCenter, "Box Center", "Center of the axis-aligned bounding box", true)

    Q_ENUM(NELineType)
    Q_ENUM(FSwapDegree)
    Q_ENUM(FFloorPlaneEffect)

public slots:
    void ScaleFloorPlan( const vec3f& scale );

public:
    FZone( NENodeFactory* factory = 0);
    virtual ~FZone();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void createMesh();
    void loadImage();
    void resetProperties() override;

    QString zoneName();
    uint    setZoneName(const QString zoneName );

    QList<QString>& scenarios() override;
    QList<QString> &otherItems() override;

    QString model3DInfo();
    void setModel3DInfo( QString model3dinfo );

    virtual QString nodeDescription() override;

    virtual void propertiesFromJSON( QString jsonStr );
    virtual QString propertiesToJSON();

    QImage textureImage();

    // Update the wall/ceiling/floor thicknesses
    void updateThicknesses();

    // Return the compartment axis-aligned bounding box
    const AABB& compAABB() const;
    // Return the outer compartment axis-aligned bounding box,
    // accounting for the wall/ceiling/floor thicknesses
    const AABB& outerCompAABB() const;

    // Compute the compartment bounding box
    void computeCompBB();

    virtual NEIndexedTriMesh& mesh();

    // Return the height of the separation between lower and higher layers
    double layerHeight(double time) const;
    // Return the temperatures of the lower and higher layers
    double lowerLayerTemp(double time) const;
    double higherLayerTemp(double time) const;
    // Return the surface temperatures of the wall/ceiling/floor
    double ceilingSurfTemp(double time) const;
    double floorSurfTemp(double time) const;
    double lowerWallSurfTemp(double time) const;
    double higherWallSurfTemp(double time) const;

    // Return the minimum and maximum layer temperatures over time
    double minLayerTemp() const;
    double maxLayerTemp() const;
    // Return the minimum and maximum surface temperatures over time
    double minSurfTemp() const;
    double maxSurfTemp() const;

    // Add a height of the separation between lower and higher layers
    void addLayerHeight(double time, double height);
    // Add temperatures of the lower and higher layers
    void addLowerLayerTemp(double time, double temperature);
    void addHigherLayerTemp(double time, double temperature);
    // Add surface temperatures of the wall/ceiling/floor
    void addCeilingSurfTemp(double time, double temperature);
    void addFloorSurfTemp(double time, double temperature);
    void addLowerWallSurfTemp(double time, double temperature);
    void addHigherWallSurfTemp(double time, double temperature);


    NEFloorPlane *floorPlane();

    // Clear the simulation data
    void clearSimData();
    void clearCompartmentData();

protected:
    NEIndexedTriMesh m_TriMesh;
    vec3f m_normal;
    void childAdded(QObject* child) override;
    void childRemoved(QObject* child) override;

    virtual bool transparencyVisible();

private:
    NE_DECL_PRIVATE(FZone)
};

// The factory class
class FZoneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FZoneFactory(NEManager* m = 0);
    ~FZoneFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
