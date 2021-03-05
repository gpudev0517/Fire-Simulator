#include "FFireDetect.h"

class FFireDetectPrivate : public QObject
{
public:
    FFireDetectPrivate(FFireDetect* s) : S(s)
    {
    }

    /* Functions */

    /* Properties */
    QString     m_Alarm;
    QString     m_Zone;

private:
    FFireDetect* S;
};

FFireDetectFactory::FFireDetectFactory(NEManager* m) : NENodeFactory(m)
{
}

FFireDetectFactory::~FFireDetectFactory()
{
}

QString FFireDetectFactory::nodeName()
{
    return "Fire Detection";
}

uint FFireDetectFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FFireDetectFactory::nodeInherits()
{
    return "Physical";
}

QString FFireDetectFactory::objectName()
{
    return "Detection";
}

QString FFireDetectFactory::nodeVersion()
{
    return "0.1";
}

QString FFireDetectFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FFireDetectFactory::nodeHelp()
{
    return "Fire Definition";
}

NEObject* FFireDetectFactory::createInstance()
{
    return new FFireDetect(this);
}

NE_PROP_DEF(QString, FFireDetect, Alarm, QString("FALARM_1"))

static void initProperties()
{
    NE_INIT_PROP(FFireDetect, Alarm);
}

FFireDetect::FFireDetect(NENodeFactory *factory) : FZoneItem(factory)
{
    P = new FFireDetectPrivate(this);

    addNodeTypeToNodeGraphSceneType("Physical", NENode::SceneType::Simulation);

    CALL_ONCE(initProperties);
    P->m_Alarm   = NE_DEFAULT(Alarm);
    P->m_Zone = QString("1");
}

FFireDetect::~FFireDetect()
{
    delete P;
}

const QString& FFireDetect::Alarm() const
{
    return P->m_Alarm;
}

const QString& FFireDetect::Compartment() const
{
    return P->m_Zone;
}

uint FFireDetect::SetAlarm(const QString& val)
{
    NE_PROP_SET_EVAL(Alarm, P, val);

    return NE::kReturnSuccess;
}

uint FFireDetect::init(uint initMode)
{
    uint ret = NENode::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FFireDetect::resetProperties()
{
    NENode::resetProperties();

    ResetAlarm();
}

bool FFireDetect::addSource(NENode* node, NEConn& conn)
{

    return NENode::addSource(node, conn);
}

void FFireDetect::removeSource(NENode* node, uint type, uint sIndex, uint tIndex)
{

    return NENode::removeSource(node, type, sIndex, tIndex);
}

QString FFireDetect::categoryType()
{
    return "Simulation Items/Detection";
}
