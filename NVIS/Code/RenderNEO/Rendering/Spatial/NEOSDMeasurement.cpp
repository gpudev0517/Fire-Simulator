#include "Physics/Spatial/NEMeasurementField.h"
#include "NEOSDMeasurement.h"
#include "Rendering/Manager/NEGLManager.h"

#define COLORBAR_WIDTH 431
#define COLORBAR_HEIGHT 20

NEOSDMeasurement::NEOSDMeasurement( NENodeFactory * factory): NEOSDBase( factory )
{
    setInputs( inputs()|NE::kIOMeasurement );
    m_measure = NULL;
}

NEOSDMeasurement::~NEOSDMeasurement()
{

}

bool NEOSDMeasurement::addSource( NENode *node, NEConn &conn )
{
    switch( conn.type )
    {
    case NE::kIOMeasurement:
        {
            NEMeasurementField* measure = qobject_cast< NEMeasurementField* >( node );
            if( measure )
            {
                m_measure = measure;
            }
        }
        break;
    default:
        return false;
    }
    return NENode::addSource( node, conn );
}


void NEOSDMeasurement::removeSource(NENode *node, uint type, uint sIndex, uint tIndex)
{
    switch (type)
    {
    case NE::kIOMeasurement:
        m_measure = NULL;
        break;
    default:
        return;
    }

    NENode::removeSource( node, type, sIndex, tIndex );
}

const NEMeasurementField* NEOSDMeasurement::measure() const
{
    return m_measure;
}

vec2f NEOSDMeasurement::osdSize()
{
    if (!m_measure) return NEOSDBase::osdSize();

    QStringList osdMessage;
    osdMessage << "" << "" // Title
               << QString("Height: %1").arg(m_measure->avgFluidHeight())
               << QString("Pressure: %1").arg(m_measure->avgPressure())
               << QString("Vel: %1 %2 %3").arg(m_measure->avgVel().x()).arg(m_measure->avgVel().y()).arg(m_measure->avgVel().z())
               << QString("FlowRate: %1").arg(m_measure->flowRate())
               << QString("velocity (m/s)")
               << QString("0");

    uint widthU = 0;

    for (int i = osdMessage.size() - 1; i >= 0; i--)
    {
        QString text = osdMessage[i];
        uint texLen = (float)text.length() * fontWidth() + fontWidth();
        widthU = MAX(COLORBAR_WIDTH, texLen);
    }

    return vec2f(widthU, fontHeight() * osdMessage.size() + COLORBAR_HEIGHT);
}

NEOSDMeasurementFactory::NEOSDMeasurementFactory(NEManager* m) : NENodeFactory(m) {}

NEOSDMeasurementFactory::~NEOSDMeasurementFactory() {}

QString NEOSDMeasurementFactory::nodeName() { return "Measurement OSD"; }

uint NEOSDMeasurementFactory::nodeType(){ return NE::kObjectOSD;}

QString NEOSDMeasurementFactory::nodeInherits() { return "OSDs";}

QString NEOSDMeasurementFactory::objectName() { return "MeasurementOSD"; }

QString NEOSDMeasurementFactory::nodeVersion() { return "0.1"; }

QString NEOSDMeasurementFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEOSDMeasurementFactory::nodeHelp() { return "Measurement OSD"; }

NEObject*NEOSDMeasurementFactory::createInstance() { return new NEOSDMeasurement(this); }
