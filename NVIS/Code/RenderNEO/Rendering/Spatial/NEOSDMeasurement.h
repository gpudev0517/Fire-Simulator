#pragma once

#include "Rendering/Spatial/NEOSDBase.h"

class NEMeasurementField;
//
// Move all particle fluid related rendering/visualization code here..
//

class NEOSDMeasurement: public NEOSDBase
{
    Q_OBJECT
    NE_CLASS("Measurement OSD")

public:
    NEOSDMeasurement(NENodeFactory *factory);
    ~NEOSDMeasurement();

    const NEMeasurementField* measure() const;
    virtual vec2f osdSize();

protected:

    virtual bool addSource( NENode* node, NEConn& conn );
    virtual void removeSource( NENode* node, uint type, uint sIndex, uint tIndex );

    NEMeasurementField* m_measure;
    //
    // OSD Related functions here...
    //

};
Q_DECLARE_INTERFACE(NEOSDMeasurement, "NEOSDMeasurement")


#include "Base/NENodeFactory.h"

class NEOSDMeasurementFactory : public NENodeFactory
{
	Q_OBJECT
	Q_INTERFACES(NENodeFactory)

public:
	NEOSDMeasurementFactory(NEManager* m);
	~NEOSDMeasurementFactory();

	QString nodeName();
	uint nodeType();
	QString nodeInherits();
	QString objectName();
	QString nodeVersion();
	QString nodeVendor();
	QString nodeHelp();

private:
	NEObject* createInstance();
};
