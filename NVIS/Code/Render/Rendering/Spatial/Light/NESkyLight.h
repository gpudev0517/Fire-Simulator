#pragma once

#include "Rendering/Spatial/NELight.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NESkyLight))

class NESkyLight : public NELight
{
    Q_OBJECT
    NE_CLASS("Sky Light")

    NE_PROP_GROUP_START(ExternalRendering, "External Rendering", "External renderer specific settings",true)
    NE_PROP_RW(double,Turbidity,"Turbidity","Determine the overall aerosol content of the air [1,10]",true,)
    NE_PROP_RW(QColor,GroundColor,"Ground Color","Color of the ground (albedo)",true,)
    NE_PROP_RW(double,Elevation,"Elevation","Elevation of the Sun [0,90]",true,)
    NE_PROP_RW(double,Azimuth,"Azimuth","Azimuthal position of the Sun [0,360]",true,)
    NE_PROP_RW(bool,DrawSunDisk,"Draw Sun Disk","Draw the Sun disk",true,)
    NE_PROP_RW(bool,DrawGround,"Draw Ground","Draw the ground (the space under the horizon)",true,)
    NE_PROP_GROUP_END(ExternalRendering)

public:
    NESkyLight(NENodeFactory* factory);
    virtual ~NESkyLight();
    uint init(uint initMode = NE::kInitOnCreate);
    void resetProperties();
    void propertyTweak(QtPropertyBase* p);

protected:
    NE_DERIVED(NESpatial,Protected)

public slots:
    NE_DERIVED(NESpatial,Public_Slots)

private:
    NE_DECL_PRIVATE(NESkyLight)
};


#include "Base/NENodeFactory.h"

class NESkyLightFactory : public NENodeFactory
{
public:
    NESkyLightFactory(NEManager* m);
    ~NESkyLightFactory();
    NE_DERIVED(NENodeFactory,Public)
};
