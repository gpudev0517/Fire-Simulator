#pragma once

#include "Spatial/NESpatial.h"
#include "Utility/NEFrameInfo.h"
#include "Mesh/NESpatialMesh.h"
#include "Rendering/Spatial/NECamera.h"

class NEMeasurementItemPrivate;

//! This class will be used to clip .
class NECEXP_NERENDERING NEMeasurementItem: public NESpatial
{
    Q_OBJECT
    NE_CLASS("Measurement Item")

public:
    enum NEMeasurementType { Point = 0, Edge = 1, Face = 2};

    NE_PROP_3D_RW(vec3f, float, Offset, "Offset", "Offset of the node", true, virtual)
    NE_PROP_RW(double, TextSize, "TextSize", "TextSize of the node", true, virtual)
    NE_PROP_RW(bool, TextScaleByLength, "Text Scale By Length", "Text size changed by Length", true, virtual)
    NE_PROP_RW(NEMeasurementType, MeasureType, "Type", "Measurement type (between vertices etc)", true,)
    NE_PROP_RW(NECamera::OrthographicType, CameraType, "Cam Type", "The type of ortho camera which is used for measurement", true,)

    Q_ENUM( NEMeasurementType );

public:

    NEMeasurementItem(NENodeFactory *factory);
    ~NEMeasurementItem();
    uint init( uint initMode = NE::kInitOnCreate ) override;

    virtual void render() override;
    virtual void renderPickable() override;
    virtual void renderPicked() override;
    virtual NESpatialType spatialType() override;

    QString toJson() override;
    void fromJson( QString& obj ) override;

    std::vector<QPair<NESpatialMesh*, int>> measureInfoOrigin;
    std::vector<QPair<NESpatialMesh*, QPair<QPair<int, int>, vec3f>>> measureEdgeInfo;
    QPair<vec3f, float> measureInfoMoved;

    bool isMeasurementEdge() const;
    void setMeasurementEdge(bool value);


protected:
    virtual bool renderModeVisible() override;
    virtual bool solidParamVisible() override;
    virtual bool receiveShadowVisible() override;
    virtual bool occludeShadowsVisible() override;

    virtual void resetProperties() override;
    virtual void propertyTweak(QtPropertyBase* p) override;

protected:
    virtual bool scaleVisible() override;
    virtual bool pivotVisible() override;


    virtual bool addSource(NENode* node, NEConn& conn) override;


private:
    NE_DECL_PRIVATE(NEMeasurementItem)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NEMeasurementItem, "NEMeasurementItem")

#include "Base/NENodeFactory.h"

class NEMeasurementItemFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEMeasurementItemFactory(NEManager* m);
    ~NEMeasurementItemFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

private:
    NEObject* createInstance();

protected:

};
