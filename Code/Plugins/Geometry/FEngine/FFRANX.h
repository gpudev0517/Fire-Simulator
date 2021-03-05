#pragma once

#include "Core/Spatial/NESpatialGroup.h"
#include "Core/Utility/FilePath.h"
#include "Base/NENodeFactory.h"

#include "FFRANXProperties.h"

class FFRANXPrivate;




class FDYNCEXP_PLUGINS FFRANX: public NESpatialGroup
{
    Q_OBJECT

public:

    FFRANX( NENodeFactory* factory  );
    virtual ~FFRANX();

    NE_PROP_RW(FilePath, FRANXFile, "FRANX File", "File for the FRANX Database", true,)


    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

public slots:
    QString toJson();
    void fromJson( QString jsonStr );

private:
    NE_DECL_PRIVATE(FFRANX)
};

class FFRANXFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FFRANXFactory(NEManager* m = 0);
    virtual ~FFRANXFactory();

    NE_DERIVED(NENodeFactory, Public)
};

