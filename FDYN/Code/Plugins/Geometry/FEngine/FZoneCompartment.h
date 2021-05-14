#pragma once
#include "Utility/FilePath.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NENodeFactory.h"
#include "Mesh/NEZoneCompartment.h"
#include "Utility/NESurfaceMaterial.h"

NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(FZoneCompartment))

class FDYNCEXP_PLUGINS FZoneCompartment : public NEZoneCompartment
{
    Q_OBJECT
    NE_CLASS("Fire Zone Container Properties")

public:
    FZoneCompartment( NENodeFactory* factory = 0);
    virtual ~FZoneCompartment();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    void initializeGUIActions() override;

    const QString& compartmentName();
    uint setCompartmentName(const QString& compartmentName);

    const QString& model3DInfo();
    void setModel3DInfo(const QString& model3dinfo);

    virtual QString nodeDescription() override;

    virtual void propertiesFromJSON( QString jsonStr );
    virtual QString propertiesToJSON();

    virtual QString categoryType() override;

    void updateParentCompBB();
    void updateTransform() override;

private:
    NE_DECL_PRIVATE(FZoneCompartment)

};

// The factory class
class FZoneCompartmentFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FZoneCompartmentFactory(NEManager* m = 0);
    ~FZoneCompartmentFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
