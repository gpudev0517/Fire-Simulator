#pragma once

#include "Base/NEMultiNodeFactory.h"

class FDYNCEXP_PLUGINS FFactory : public NEMultiNodeFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "FFactory");
    Q_INTERFACES(NENodeFactory);

public:
    FFactory(NEManager* m = 0);
    ~FFactory();

    NE_DERIVED(NEMultiNodeFactory,Public)
};

