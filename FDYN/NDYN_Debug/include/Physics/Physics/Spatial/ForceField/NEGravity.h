#pragma once

#include "Physics/Spatial/NEForceField.h"
#include "Base/NENodeFactory.h"

class NEGravityPrivate;

class NECEXP_PHYSICS NEGravity : public NEForceField
{
	Q_OBJECT
	NE_CLASS("Gravity")

    NE_PROP_3D_RW(vec3f, float, Gravity, "Gravitational Acceleration", "Acceleration caused by the gravity", true,)

public:
	NEGravity(NENodeFactory* factory);
    virtual ~NEGravity();

    const vec3f& force(const vec3f* pos = 0, const vec3f* vel = 0) const;

protected:
	NE_DERIVED(NESpatial, Protected)

private:
	NE_DECL_PRIVATE(NEGravity)
};


class NEGravityFactory : public NENodeFactory
{
public:
	NEGravityFactory(NEManager* m);
	~NEGravityFactory();
	NE_DERIVED(NENodeFactory, Public)

};


