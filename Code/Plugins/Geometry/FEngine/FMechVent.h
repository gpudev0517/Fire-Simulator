#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

#include "FVent.h"

class FMechVentPrivate;
/*!
  Class defining a solid and its physical properties.
*/

class  FMechVent : public FVent
{
    Q_OBJECT
    NE_CLASS("Mechanical Vent Properties")

public:

    NE_PROP_RW(double, FlowRate, "Flow Rate", "Flow Rate", true, )
    NE_PROP_RW(double, BeginDropOff, "Begin Drop Off", "Begin Drop Off", true, )
    NE_PROP_RW(double, ZeroAt, "Zero At", "Zero At", true, )
    NE_PROP_RW(double, FilterEfficiency, "Filter Efficiency", "Filter Efficiency", true, )
    NE_PROP_RW(double, FilterStart, "Filter Start", "Filter Start", true, )

public:
    FMechVent(NENodeFactory* factory = 0);
    ~FMechVent();

    void resetProperties() override;


private:
    NE_DECL_PRIVATE(FMechVent)
};

class FMechVentFactory : public NENodeFactory
{
public:
    FMechVentFactory(NEManager* m);
    virtual ~FMechVentFactory();
    NE_DERIVED(NENodeFactory, Public)
};
