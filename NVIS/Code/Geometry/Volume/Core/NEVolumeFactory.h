#pragma once

#include "Base/NEMultiNodeFactory.h"

class NEVolumeFactory : public NEMultiNodeFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NENodeFactory");
    Q_INTERFACES(NENodeFactory);

public:
    NEVolumeFactory(NEManager* m = 0);
    ~NEVolumeFactory();

    NE_DERIVED(NEMultiNodeFactory,Public)
};

