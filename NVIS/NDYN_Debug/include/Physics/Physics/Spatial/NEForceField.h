#pragma once

#include "Spatial/NESpatial.h"

class NECEXP_PHYSICS NEForceField : public NESpatial
{
	Q_OBJECT
public:
    NEForceField(NENodeFactory* factory);
    virtual ~NEForceField() {}

    virtual bool connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order = 0);
    virtual bool disconnectFrom(NENode* target, uint type, uint sIndex, uint tIndex);

    virtual const vec3f& force(const vec3f* pos = 0, const vec3f* vel = 0) const = 0;
};

Q_DECLARE_INTERFACE(NEForceField, "NEForceField")

