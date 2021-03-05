#include "FMechVent.h"

class FMechVentPrivate : public QObject
{
public:
    FMechVentPrivate(FMechVent* s) : S(s)
    {
    }

    /* Functions */

    /* Properties */
    double m_FlowRate;
    double m_BeginDropOff;
    double m_ZeroAt;
    double m_FilterEfficiency;
    double m_FilterStart;

private:
    FMechVent* S;
};

FMechVentFactory::FMechVentFactory(NEManager* m) : NENodeFactory(m)
{
}

FMechVentFactory::~FMechVentFactory()
{
}

QString FMechVentFactory::nodeName()
{
    return "Fire Mech Vent";
}

uint FMechVentFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FMechVentFactory::nodeInherits()
{
    return "Physical";
}

QString FMechVentFactory::objectName()
{
    return "FMVent";
}

QString FMechVentFactory::nodeVersion()
{
    return "0.1";
}

QString FMechVentFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FMechVentFactory::nodeHelp()
{
    return "Mech Vent";
}

NEObject* FMechVentFactory::createInstance()
{
    return new FMechVent(this);
}

NE_PROP_DEF(double, FMechVent, FlowRate, 0.0)
NE_PROP_DEF(double, FMechVent, BeginDropOff, 0.0)
NE_PROP_DEF(double, FMechVent, ZeroAt, 0.0)
NE_PROP_DEF(double, FMechVent, FilterEfficiency, 0.0)
NE_PROP_DEF(double, FMechVent, FilterStart, 0.0)

static void initProperties()
{
    NE_INIT_PROP(FMechVent, FlowRate);
    NE_INIT_PROP(FMechVent, BeginDropOff);
    NE_INIT_PROP(FMechVent, ZeroAt);
    NE_INIT_PROP(FMechVent, FilterEfficiency);
    NE_INIT_PROP(FMechVent, FilterStart);
}

FMechVent::FMechVent(NENodeFactory* factory) : FVent(factory)
{
    P = new FMechVentPrivate(this);

    addNodeTypeToNodeGraphSceneType("Physical", NENode::SceneType::Simulation);

    CALL_ONCE(initProperties);
    P->m_FlowRate = NE_DEFAULT(FlowRate);
    P->m_BeginDropOff = NE_DEFAULT(BeginDropOff);
    P->m_ZeroAt = NE_DEFAULT(ZeroAt);
    P->m_FilterEfficiency = NE_DEFAULT(FilterEfficiency);
    P->m_FilterStart = NE_DEFAULT(FilterStart);

    SetColor( QColor( 255, 255, 0, 0 ) );
    SetSolidWireframeColor( QColor( 0, 0, 0, 0 ) );
    SetDrawSolidWireframe( true );
}

FMechVent::~FMechVent()
{
    delete P;
}

void FMechVent::resetProperties()
{
    FVent::resetProperties();

    ResetFlowRate();
    ResetBeginDropOff();
    ResetZeroAt();
    ResetFilterEfficiency();
    ResetFilterStart();
}

const double& FMechVent::FlowRate() const
{
    return P->m_FlowRate;
}

uint FMechVent::SetFlowRate(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(FlowRate, P, val);

    return NE::kReturnSuccess;
}

const double& FMechVent::BeginDropOff() const
{
    return P->m_BeginDropOff;
}

uint FMechVent::SetBeginDropOff(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(BeginDropOff, P, val);

    return NE::kReturnSuccess;
}

const double& FMechVent::ZeroAt() const
{
    return P->m_ZeroAt;
}

uint FMechVent::SetZeroAt(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(ZeroAt, P, val);

    return NE::kReturnSuccess;
}

const double& FMechVent::FilterEfficiency() const
{
    return P->m_FilterEfficiency;
}

uint FMechVent::SetFilterEfficiency(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(FilterEfficiency, P, val);

    return NE::kReturnSuccess;
}

const double& FMechVent::FilterStart() const
{
    return P->m_FilterStart;
}

uint FMechVent::SetFilterStart(const double& val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;

    NE_PROP_SET_EVAL(FilterStart, P, val);

    return NE::kReturnSuccess;
}
