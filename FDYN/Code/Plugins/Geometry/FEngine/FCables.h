#pragma once

#include "FZoneItem.h"
#include "Base/NENodeFactory.h"
#include "Utility/NEFireMaterial.h"
#include "Utility/NECableProperty.h"


class FCablesPrivate;
/*!
  Class defining a fire ignition source.
*/

class  FCables : public FZoneItem
{
    Q_OBJECT
    NE_CLASS("Cables")

public:

    NE_PROP_RW(NECableProperty, CableMaterial, "Cable Material", "Cable Material", true, )

public:
    FCables(NENodeFactory* factory = 0);
    ~FCables();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    virtual QString categoryType();

signals:

private:
    NE_DECL_PRIVATE(FCables)
};

class FCablesFactory : public NENodeFactory
{
public:
    FCablesFactory(NEManager* m = 0);
    virtual ~FCablesFactory();
    NE_DERIVED(NENodeFactory, Public)
};
