#pragma once
#include "Utility/FilePath.h"
#include "Spatial/NESpatialGroup.h"
#include "Base/NENodeFactory.h"
#include "FZone.h"


NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(FSubZone))


class FDYNCEXP_PLUGINS FSubZone : public FZone
{
    Q_OBJECT
    NE_CLASS("Sub Zone")

public:
    FSubZone( NENodeFactory* factory = 0);
    virtual ~FSubZone();



    virtual QString categoryType();


private:
    NE_DECL_PRIVATE(FSubZone)

};


// The factory class
class FSubZoneFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FSubZoneFactory(NEManager* m = 0);
    ~FSubZoneFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};


