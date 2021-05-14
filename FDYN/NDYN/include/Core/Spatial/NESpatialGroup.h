#pragma once

#include "Spatial/NESpatial.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_2(class, NEIndexedTriMesh, NE_PRIVATE(NESpatialGroup))

//! This class is used to group spatials and apply to them a common transformation.

class NECEXP_CORE NESpatialGroup : public NESpatial
{
    Q_OBJECT
    NE_CLASS("Spatial Group")

    NE_PROP_RW(bool, GroupBox, "Show Group Box", "Shows group box of the spatial group object.", groupBoxVisible,)

public:
    NESpatialGroup(NENodeFactory* factory);
    ~NESpatialGroup();

    virtual uint init(uint initMode = NE::kInitOnCreate);

    void render();
    void renderPickable();
    void renderPicked();

public slots:
    uint SetBehavior(NENodeState val);
    uint SetCacheData(bool val);
    uint SetVerbose(bool val);
    uint SetRenderMode(NERenderMode val);
    uint SetExternalRender(bool val);

public:
    void recomputeAABB();
    AABB getGroupAABB(matrix44f transform);

    virtual void resetProperties();
private:
    NE_DECL_PRIVATE(NESpatialGroup)

protected:
    void childAdded(QObject* child);
    void childRemoved(QObject* child);
    virtual bool groupBoxVisible() {return true;}
};

class NECEXP_CORE NESpatialGroupFactory : public NENodeFactory
{
public:
    NESpatialGroupFactory(NEManager* m);
    ~NESpatialGroupFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();
    NEObject* createInstance();

};
