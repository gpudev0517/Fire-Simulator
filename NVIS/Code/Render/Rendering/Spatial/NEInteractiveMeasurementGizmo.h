#pragma once

#include "Spatial/NESpatial.h"
#include "Utility/NEFrameInfo.h"

class NEInteractiveMeasurementGizmoPrivate;

//! This class will be used to clip .
class NEInteractiveMeasurementGizmo: public NESpatial
{
    Q_OBJECT
    NE_CLASS("Interactive Measurement Gizmo")

public:
    NE_PROP_3D_RW(vec3f, float, Offset, "Offset", "Offset of the node", true, virtual)
    NE_PROP_RW(double, TextSize, "TextSize", "TextSize of the node", true, virtual)
    NE_PROP_RW(bool, TextScaleByLength, "Text Scale By Length", "Text size changed by Length", true, virtual)

public:
    NEInteractiveMeasurementGizmo(NENodeFactory *factory);
    ~NEInteractiveMeasurementGizmo();
    uint init(uint initMode = NE::kInitOnCreate);

    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    virtual NESpatialType spatialType(){return NESpatial::NESpatialType::kGizmo;}
    virtual void resetProperties();
    virtual void propertyTweak(QtPropertyBase* p);

protected:
    virtual bool scaleVisible() {return false;}
    virtual bool pivotVisible() {return false;}
    virtual bool renderModeVisible() {return false;}
    virtual bool solidParamVisible() {return false;}
    virtual bool receiveShadowVisible() {return false;}
    virtual bool occludeShadowsVisible() {return false;}

    virtual bool addSource(NENode* node, NEConn& conn);

private:
    NE_DECL_PRIVATE(NEInteractiveMeasurementGizmo)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NEInteractiveMeasurementGizmo, "NEInteractiveMeasurementGizmo")

#include "Base/NENodeFactory.h"

class NEInteractiveMeasurementGizmoFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEInteractiveMeasurementGizmoFactory(NEManager* m);
    ~NEInteractiveMeasurementGizmoFactory();

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
