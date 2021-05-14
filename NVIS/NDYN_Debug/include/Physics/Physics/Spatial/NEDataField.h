#pragma once

#include "Spatial/NESpatial.h"

class NEDataField : public NESpatial
{
    Q_OBJECT
public:
    NEDataField(NENodeFactory* factory) : NESpatial(factory) { }
    virtual ~NEDataField() { }

    virtual void update() = 0;
};

Q_DECLARE_INTERFACE(NEDataField, "NEDataField")
