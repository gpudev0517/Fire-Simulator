#pragma once

#include "NEOSDBase.h"

class NEOSDGlobalInformationPrivate;
//
// Global information related OSD code here

class NEOSDGlobalInformation: public NEOSDBase
{
    Q_OBJECT
    NE_CLASS("Global Information OSD")

public:
    NE_PROP_RW(QString, AlternativeText, "Alternative Text", "Alternative Text", true,)

public:
    NEOSDGlobalInformation( NENodeFactory* factory );
    ~NEOSDGlobalInformation();

    virtual vec2f osdSize();
    QString OsdText();

public slots:
    //! Resets all properties totheir default values
    virtual void resetProperties();

protected:
    //
    // OSD Related functions here...
    //

private:
    NE_DECL_PRIVATE(NEOSDGlobalInformation)
};

Q_DECLARE_INTERFACE(NEOSDGlobalInformation, "NEOSDGlobalInformation")

#include "Base/NENodeFactory.h"

class NEOSDGlobalInformationFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEOSDGlobalInformationFactory(NEManager* m);
    ~NEOSDGlobalInformationFactory();
    NE_DERIVED(NENodeFactory,Public)

};
