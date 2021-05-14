#pragma once

#include "Spatial/NESpatial.h"
#include "Utility/NEFrameInfo.h"
#include "Mesh/NESpatialMesh.h"

class NEFloorPlanePrivate;

//! This class will be used to clip .
class NECEXP_NERENDERING NEFloorPlane: public NESpatial
{
    Q_OBJECT
    NE_CLASS("Floor Plane")

public:

    NEFloorPlane(NENodeFactory *factory);
    ~NEFloorPlane();
    uint init(uint initMode = NE::kInitOnCreate);

    enum NESwapDegree { Original = 0, degree_90 = 1, degree_180 = 2, degree_270 = 3 };
    enum NEFloorPlaneEffect { Transparency = 0, AmbientOcclusion = 1 };

    NE_PROP_RW(bool, ShowFloorPlan, "Floor Plan", "Show/hide Floor Plan.", true, )
    NE_PROP_RW(double, FloorElevation, "Floor elevation", "Floor elevation", true,)
    NE_PROP_RW(FilePath, FloorPlan, "Floor texture", "Floor texture", true,)
    NE_PROP_GROUP_START(FloorPlaneEffectUse, "Floor Rendering Effect", "Floor Rendering Effect", true)
    NE_PROP_RW(NEFloorPlaneEffect, FloorPlaneEffect, "Effect", "Floor Rendering Effect", true,)
    NE_PROP_RW(double, FloorPlanTransparency, "Transparency", "Floor Plan Transparency", transparencyVisible,)
    NE_PROP_GROUP_END(FloorPlaneEffectUse)
    NE_PROP_RW(double, XMin, "X Min", "X Min", true,)
    NE_PROP_RW(double, XMax, "X Max", "X Max", true,)
    NE_PROP_RW(double, XSize, "X Size", "X Size", true,)
    NE_PROP_RW(double, ZMin, "Z Min", "Z Min", true,)
    NE_PROP_RW(double, ZMax, "Z Max", "Z Max", true,)
    NE_PROP_RW(double, ZSize, "Z Size", "Z Size", true,)
    NE_PROP_RW(NESwapDegree, Swap, "Swap", "Swap", true,)
    NE_PROP_RW(bool, Flip, "Flip", "Flip", true,)

    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    virtual NESpatialType spatialType();
    virtual void setSpatialType(NESpatialType type);
    virtual void createMesh();
    NEIndexedTriMesh& mesh();

    Q_ENUM(NESwapDegree)
    Q_ENUM(NEFloorPlaneEffect)

signals:
    void sendScaleFloorPlane( const vec3f& scale );
    void sendPosFloorPlane( const vec3f& scale );

public slots:
    virtual uint SetScale(const vec3f& Scale);
    virtual uint SetPosition(const vec3f& Pos);

    QImage TextureImage();


protected:
    virtual bool renderModeVisible();
    virtual bool solidParamVisible();
    virtual bool receiveShadowVisible();
    virtual bool occludeShadowsVisible();
    virtual bool transparencyVisible();


private:
    NE_DECL_PRIVATE(NEFloorPlane)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NEFloorPlane, "NEFloorPlane")

#include "Base/NENodeFactory.h"

class NEFloorPlaneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEFloorPlaneFactory(NEManager* m);
    ~NEFloorPlaneFactory();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

    QString nodeName();
private:
    NEObject* createInstance();

protected:

};
