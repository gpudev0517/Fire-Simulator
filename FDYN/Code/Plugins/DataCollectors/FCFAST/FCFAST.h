#pragma once

#include "Physics/globals_Physics.h"
#include "Physics/Spatial/NEDataCollector.h"
#include "Core/Utility/FilePath.h"
#include "Base/NENodeFactory.h"

class FCFASTPrivate;

class FDYNCEXP_PLUGINS FCFAST: public NEDataCollector
{
    Q_OBJECT

    NE_PROP_RW(double, TextOutputInt, "Text Output Interval", "Text output interval", true, )
    NE_PROP_RW(double, SpreadSheetOutputInt, "Spreadsheet Output Interval", "Spreadsheet output interval", true, )
    NE_PROP_RW(double, SmokeViewOutputInt, "SmokeView Output Interval", "Smokeview output interval", true, )
    NE_PROP_RW(double, IntTemp, "Int. Temperature","Interior temperature", true, )
    NE_PROP_RW(double, ExtTemp, "Ext. Temperature","Exterior temperature", true, )
    NE_PROP_RW(double, Humidity, "Humidity", "Humidity", true, )
    NE_PROP_RW(double, Pressure, "Pressure", "Pressure", true, )
    NE_PROP_RW(double, MaxTimeStep, "Max. Time Step", "Maximum time step", true, )
    NE_PROP_RW(double, LowerO2Limit, "Lower Oxygen Limit", "Lower oxygen limit", true, )
    NE_PROP_RW(bool, Adiabatic, "Adiabatic Surfaces", "Adiabatic surfaces", true, )

public:

    FCFAST( NENodeFactory* factory  );
    virtual ~FCFAST();

    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

private:
    NE_DECL_PRIVATE(FCFAST)
};

class FCFASTFactory : public NENodeFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "NENodeFactory")
    Q_INTERFACES(NENodeFactory)

public:
    FCFASTFactory(NEManager* m = 0);
    virtual ~FCFASTFactory();

    NE_DERIVED(NENodeFactory, Public)
};

Q_DECLARE_INTERFACE(FCFAST, "CFAST")
