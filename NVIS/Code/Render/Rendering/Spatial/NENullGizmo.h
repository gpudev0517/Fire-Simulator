#pragma once

#include "Spatial/NESpatial.h"
#include "Utility/NEFrameInfo.h"

class NENullGizmoPrivate;

//! This class will be used to clip .
class NENullGizmo: public NESpatial
{
    Q_OBJECT
    NE_CLASS("Null Gizmo")

public:

    NENullGizmo(NENodeFactory *factory);
    ~NENullGizmo();
    uint init(uint initMode = NE::kInitOnCreate);

    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    virtual NESpatialType spatialType(){return NESpatialType::kGizmo;}

protected:
    virtual bool renderModeVisible() {return false;}
    virtual bool solidParamVisible() {return false;}
    virtual bool receiveShadowVisible() {return false;}
    virtual bool occludeShadowsVisible() {return false;}

public slots:
    bool connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order = 0);

private:
    NE_DECL_PRIVATE(NENullGizmo)

};



// This is necessary to make a plugin instance castable to NESpatial type
Q_DECLARE_INTERFACE(NENullGizmo, "NENullGizmo")

#include "Base/NENodeFactory.h"

class NENullGizmoFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NENullGizmoFactory(NEManager* m);
    ~NENullGizmoFactory();

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
