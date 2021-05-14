#pragma once

#include "FZoneItem.h"
#include "Base/NENodeFactory.h"
#include "Utility/NEFireMaterial.h"

class FFirePrivate;
/*!
  Class defining a fire ignition source.
*/

class  FFire : public FZoneItem
{
    Q_OBJECT
    NE_CLASS("Fire Definition/Source")

public:

    enum FireIgnitionType {
        Time,
        Temperature,
        HeatFlux
    };

    NE_PROP_RW(FireIgnitionType, FireIgnition, "Ignition Criteria", "Fire Ignition Crieterion", true, )
    NE_PROP_RW(NEFireMaterial, FireMaterial, "Fire Material", "Fire Material", true, )

    Q_ENUMS(FireIgnitionType)
public:
    FFire(NENodeFactory* factory = 0);
    ~FFire();

    void copyValues(const NEZoneItem *zoneitem) override;
    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    virtual QString categoryType() override;

signals:

private:
    NE_DECL_PRIVATE(FFire)
};

class FFireFactory : public NENodeFactory
{
public:
    FFireFactory(NEManager* m = 0);
    virtual ~FFireFactory();
    NE_DERIVED(NENodeFactory, Public)
};
