#pragma once

#include "FZoneItem.h"
#include "Base/NENodeFactory.h"

class FFireDetectPrivate;
/*!
  Class defining a solid and its physical properties.
*/

class  FFireDetect : public FZoneItem
{
    Q_OBJECT
    NE_CLASS("Fire Detection")

public:

    enum FDetectType {HeatAlarm = 0, SmokeAlarm = 1, Sprinkler = 2};
    NE_PROP_R(QString, Compartment, "Compartment", "Compartment For Fire Detection", true )
    NE_PROP_RW(FDetectType, Alarm, "Alarm", "Alarm", true, )

    Q_ENUMS(FDetectType)

public:
    FFireDetect(NENodeFactory* factory = 0);
    ~FFireDetect();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    virtual QString categoryType() override;


signals:

private:
    NE_DECL_PRIVATE(FFireDetect)
};

class FFireDetectFactory : public NENodeFactory
{
public:
    FFireDetectFactory(NEManager* m);
    virtual ~FFireDetectFactory();
    NE_DERIVED(NENodeFactory, Public)
};
