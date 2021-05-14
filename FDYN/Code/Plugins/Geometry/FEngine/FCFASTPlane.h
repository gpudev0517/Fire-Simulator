#pragma once

#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Utility/FilePath.h"
#include "Base/NENodeFactory.h"
#include "Utility/NEColorRamp.h"
#include "Rendering/Spatial/NEOSDBase.h"

class FCFASTPlanePrivate;

class FDYNCEXP_PLUGINS FCFASTPlane: public NESpatial
{
    Q_OBJECT
public:

    enum FPlaneDirection { X = 0, Y = 1, Z = 2};

    NE_PROP_RW(FPlaneDirection, PlaneDirection, "Direction Of Plane", "Direction Of Plane", false,)

    Q_ENUM(FPlaneDirection)

public:

    FCFASTPlane( NENodeFactory* factory  );
    virtual ~FCFASTPlane();

    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    virtual void render() override;
    virtual void renderPickable() override;
    virtual void renderPicked() override;

private:
    NE_DECL_PRIVATE(FCFASTPlane)
};

class FCFASTPlaneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FCFASTPlaneFactory(NEManager* m = 0);
    virtual ~FCFASTPlaneFactory();

    NE_DERIVED(NENodeFactory, Public)
};
