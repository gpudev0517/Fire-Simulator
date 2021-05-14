#include "Core/Base/NEBase.h"
#include "Core/Base/NESceneManager.h"
#include "Physics/Manager/NESimulationManager.h"
#include "FCFAST.h"

class FCFASTPrivate
{
public:
    FCFASTPrivate(FCFAST* s) : S(s)
    {
    }

    /* Properties */
    double m_TextOutputInt;
    double m_SpreadSheetOutputInt;
    double m_SmokeViewOutputInt;
    double m_IntTemp;
    double m_ExtTemp;
    double m_Humidity;
    double m_Pressure;
    double m_MaxTimeStep;
    double m_LowerO2Limit;
    bool   m_Adiabatic;

private:
    FCFAST* S;
};

NE_PROP_DEF(double, FCFAST, TextOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, SpreadSheetOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, SmokeViewOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, IntTemp, 293.0)
NE_PROP_DEF(double, FCFAST, ExtTemp, 293.0)
NE_PROP_DEF(double, FCFAST, Humidity, 15.0)
NE_PROP_DEF(double, FCFAST, Pressure, 101325.0)
NE_PROP_DEF(double, FCFAST, MaxTimeStep, 0.1)
NE_PROP_DEF(double, FCFAST, LowerO2Limit, 0.0)
NE_PROP_DEF(bool, FCFAST, Adiabatic, false)

static void initProperties()
{
    NE_INIT_PROP(FCFAST, TextOutputInt);
    NE_INIT_PROP(FCFAST, SpreadSheetOutputInt);
    NE_INIT_PROP(FCFAST, SmokeViewOutputInt);
    NE_INIT_PROP(FCFAST, IntTemp);
    NE_INIT_PROP(FCFAST, ExtTemp);
    NE_INIT_PROP(FCFAST, Humidity);
    NE_INIT_PROP(FCFAST, Pressure);
    NE_INIT_PROP(FCFAST, MaxTimeStep);
    NE_INIT_PROP(FCFAST, LowerO2Limit);
    NE_INIT_PROP(FCFAST, Adiabatic);
}

FCFAST::FCFAST( NENodeFactory* factory ) : NEDataCollector( factory )
{
    P = new FCFASTPrivate( this );

    CALL_ONCE(initProperties);
    P->m_TextOutputInt = NE_DEFAULT(TextOutputInt);
    P->m_SpreadSheetOutputInt = NE_DEFAULT(SpreadSheetOutputInt);
    P->m_SmokeViewOutputInt = NE_DEFAULT(SmokeViewOutputInt);
    P->m_IntTemp = NE_DEFAULT(IntTemp);
    P->m_ExtTemp = NE_DEFAULT(ExtTemp);
    P->m_Humidity = NE_DEFAULT(Humidity);
    P->m_Pressure = NE_DEFAULT(Pressure);
    P->m_MaxTimeStep = NE_DEFAULT(MaxTimeStep);
    P->m_LowerO2Limit = NE_DEFAULT(LowerO2Limit);
    P->m_Adiabatic = NE_DEFAULT(Adiabatic);

    setInputs( inputs()|NE::kIOExtent );
}

FCFAST::~FCFAST()
{
    delete P;
}

uint FCFAST::init( uint initMode )
{
    uint ret = NEDataCollector::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FCFAST::resetProperties()
{
    NEDataCollector::resetProperties();

    ResetTextOutputInt();
    ResetSpreadSheetOutputInt();
    ResetSmokeViewOutputInt();
    ResetIntTemp();
    ResetExtTemp();
    ResetHumidity();
    ResetPressure();
    ResetMaxTimeStep();
    ResetLowerO2Limit();
    ResetAdiabatic();
}

const double& FCFAST::TextOutputInt() const
{
    return P->m_TextOutputInt;
}

uint FCFAST::SetTextOutputInt( const double &val )
{
    NE_PROP_SET_EVAL(TextOutputInt, P, val);

    return NE::kReturnSuccess;
}


const double& FCFAST::SpreadSheetOutputInt() const
{
    return P->m_SpreadSheetOutputInt;
}

uint FCFAST::SetSpreadSheetOutputInt( const double &val )
{
    NE_PROP_SET_EVAL(SpreadSheetOutputInt, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::SmokeViewOutputInt() const
{
    return P->m_SmokeViewOutputInt;
}

uint FCFAST::SetSmokeViewOutputInt( const double &val )
{
    NE_PROP_SET_EVAL(SmokeViewOutputInt, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::IntTemp() const
{
    return P->m_IntTemp;
}

uint FCFAST::SetIntTemp( const double &val )
{
    NE_PROP_SET_EVAL(IntTemp, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::ExtTemp() const
{
    return P->m_ExtTemp;
}

uint FCFAST::SetExtTemp( const double &val )
{
    NE_PROP_SET_EVAL(ExtTemp, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::Humidity() const
{
    return P->m_Humidity;
}

uint FCFAST::SetHumidity( const double &val )
{
    NE_PROP_SET_EVAL(Humidity, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::Pressure() const
{
    return P->m_Pressure;
}

uint FCFAST::SetPressure( const double &val )
{
    NE_PROP_SET_EVAL(Pressure, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::MaxTimeStep() const
{
    return P->m_MaxTimeStep;
}

uint FCFAST::SetMaxTimeStep( const double &val )
{
    NE_PROP_SET_EVAL(MaxTimeStep, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::LowerO2Limit() const
{
    return P->m_LowerO2Limit;
}

uint FCFAST::SetLowerO2Limit( const double &val )
{
    NE_PROP_SET_EVAL(LowerO2Limit, P, val);

    return NE::kReturnSuccess;
}

const bool& FCFAST::Adiabatic() const
{
    return P->m_Adiabatic;
}

uint FCFAST::SetAdiabatic( const bool &val )
{
    NE_PROP_SET_EVAL(Adiabatic, P, val);

    return NE::kReturnSuccess;
}

FCFASTFactory::FCFASTFactory(NEManager* m) : NENodeFactory(m) {}

FCFASTFactory::~FCFASTFactory(){}

QString FCFASTFactory::nodeName() { return "CFAST"; }

uint FCFASTFactory::nodeType(){ return NE::kObjectDataCollector;}

QString FCFASTFactory::nodeInherits() { return "Data";}

QString FCFASTFactory::objectName() { return "CFAST"; }

QString FCFASTFactory::nodeVersion() { return "0.1"; }

QString FCFASTFactory::nodeVendor() { return "F Dynamics"; }

QString FCFASTFactory::nodeHelp() { return "CFAST Simulation Node"; }

NEObject*FCFASTFactory::createInstance() { return new FCFAST( this );	}
