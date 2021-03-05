#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Core/Base/NEBase.h"
#include "Core/Base/NESceneManager.h"
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

    double m_SimTime;

    FilePath m_CompartmentData;
    FilePath m_WallData;
    FilePath m_SurfaceData;


    bool m_Visualize;
    NEColorRamp m_ColorTemp;
    QVector<QPair<qreal, QColor>> m_ColorList;

    bool m_XVisPlane;
    bool m_YVisPlane;
    bool m_ZVisPlane;

    double m_XVisPlanePos;
    double m_YVisPlanePos;
    double m_ZVisPlanePos;

private:
    FCFAST* S;
};

NE_PROP_DEF(double, FCFAST, SimTime, 3600.0)
NE_PROP_DEF(double, FCFAST, TextOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, SpreadSheetOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, SmokeViewOutputInt, 1.0)
NE_PROP_DEF(double, FCFAST, IntTemp, 20.0)
NE_PROP_DEF(double, FCFAST, ExtTemp, 20.0)
NE_PROP_DEF(double, FCFAST, Humidity, 15.0)
NE_PROP_DEF(double, FCFAST, Pressure, 101325.0)
NE_PROP_DEF(double, FCFAST, MaxTimeStep, 0.1)
NE_PROP_DEF(double, FCFAST, LowerO2Limit, 0.0)
NE_PROP_DEF(bool, FCFAST, Adiabatic, false)

NE_PROP_DEF(bool, FCFAST, Visualize, false)
NE_PROP_DEF(NEColorRamp, FCFAST, ColorTemp, NEColorRamp())

NE_PROP_DEF(bool, FCFAST, XVisPlane, false)
NE_PROP_DEF(bool, FCFAST, YVisPlane, false)
NE_PROP_DEF(bool, FCFAST, ZVisPlane, false)

NE_PROP_DEF(double, FCFAST, XVisPlanePos, 0.5)
NE_PROP_DEF(double, FCFAST, YVisPlanePos, 0.5)
NE_PROP_DEF(double, FCFAST, ZVisPlanePos, 0.5)


NE_PROP_DEF(FilePath, FCFAST, CompartmentData, FilePath("cfast_compartments.csv"))
NE_PROP_DEF(FilePath, FCFAST, WallData, FilePath("cfast_devices.csv"))
NE_PROP_DEF(FilePath, FCFAST, SurfaceData, FilePath("cfast_devices.csv"))

static void initProperties()
{
    NE_INIT_PROP(FCFAST, SimTime);
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

    NE_INIT_PROP(FCFAST, Visualize);

    NE_INIT_PROP(FCFAST, ColorTemp);
    NE_INIT_PROP(FCFAST, XVisPlane);
    NE_INIT_PROP(FCFAST, YVisPlane);
    NE_INIT_PROP(FCFAST, ZVisPlane);
    NE_INIT_PROP(FCFAST, XVisPlanePos);
    NE_INIT_PROP(FCFAST, YVisPlanePos);
    NE_INIT_PROP(FCFAST, ZVisPlanePos);

    NE_INIT_PROP(FCFAST, CompartmentData);
    NE_INIT_PROP(FCFAST, WallData);
    NE_INIT_PROP(FCFAST, SurfaceData);
}


FCFAST::FCFAST( NENodeFactory* factory ) : NESpatialGroup( factory )
{
    P = new FCFASTPrivate( this );

    CALL_ONCE(initProperties);
    P->m_SimTime = NE_DEFAULT(SimTime);
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

    P->m_CompartmentData = NE_DEFAULT(CompartmentData);
    P->m_WallData = NE_DEFAULT(WallData);
    P->m_SurfaceData = NE_DEFAULT(SurfaceData);

    P->m_ColorTemp = NE_DEFAULT(ColorTemp);

    P->m_Visualize = NE_DEFAULT(Visualize);
    P->m_XVisPlane = NE_DEFAULT(XVisPlane);
    P->m_YVisPlane = NE_DEFAULT(YVisPlane);
    P->m_ZVisPlane = NE_DEFAULT(ZVisPlane);


    P->m_XVisPlanePos = NE_DEFAULT(XVisPlanePos);
    P->m_YVisPlanePos = NE_DEFAULT(YVisPlanePos);
    P->m_ZVisPlanePos = NE_DEFAULT(ZVisPlanePos);

    setInputs( inputs()|NE::kIOExtent );
}

FCFAST::~FCFAST()
{
    delete P;
}

uint FCFAST::init( uint initMode )
{
    uint ret = NESpatialGroup::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FCFAST::resetProperties()
{
    NESpatialGroup::resetProperties();

    ResetSimTime();
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

const double& FCFAST::SimTime() const
{
    return P->m_SimTime;
}

uint FCFAST::SetSimTime( const double &val )
{
    NE_PROP_SET_EVAL(SimTime, P, val);

    return NE::kReturnSuccess;
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

const bool& FCFAST::Visualize() const
{
    return P->m_Visualize;
}

uint FCFAST::SetVisualize( const bool &val )
{
    NE_PROP_SET_EVAL(Visualize, P, val);

    return NE::kReturnSuccess;
}

uint FCFAST::SetColorTemp(const NEColorRamp& val)
{
//    NE_PROP_SET_EVAL(ColorTemp, P, val );
    P->m_ColorList = val.getColorRamp();

    return NE::kReturnSuccess;
}

const NEColorRamp& FCFAST::ColorTemp() const
{
    return( P->m_ColorTemp );
}

QVector<QPair<qreal, QColor>> FCFAST::getColorList() const
{
    return P->m_ColorList;
}

const bool& FCFAST::XVisPlane() const
{
    return P->m_XVisPlane;
}


uint FCFAST::SetXVisPlane( const bool &val )
{
    NE_PROP_SET_EVAL(XVisPlane, P, val);

    return NE::kReturnSuccess;
}

const bool& FCFAST::YVisPlane() const
{
    return P->m_YVisPlane;
}

uint FCFAST::SetYVisPlane( const bool &val )
{
    NE_PROP_SET_EVAL(YVisPlane, P, val);

    return NE::kReturnSuccess;
}

uint FCFAST::SetZVisPlane( const bool &val )
{
    NE_PROP_SET_EVAL(ZVisPlane, P, val);

    return NE::kReturnSuccess;
}


const bool& FCFAST::ZVisPlane() const
{
    return P->m_ZVisPlane;
}

const double& FCFAST::XVisPlanePos() const
{
    return P->m_XVisPlanePos;
}

uint FCFAST::SetXVisPlanePos( const double &val )
{
    NE_PROP_SET_EVAL(XVisPlanePos, P, val);

    return NE::kReturnSuccess;
}

const double& FCFAST::YVisPlanePos() const
{
    return P->m_YVisPlanePos;
}

uint FCFAST::SetYVisPlanePos( const double &val )
{
    NE_PROP_SET_EVAL(YVisPlanePos, P, val);

    return NE::kReturnSuccess;
}


const double& FCFAST::ZVisPlanePos() const
{
    return P->m_XVisPlanePos;
}

uint FCFAST::SetZVisPlanePos( const double &val )
{
    NE_PROP_SET_EVAL(ZVisPlanePos, P, val);

    return NE::kReturnSuccess;
}

const FilePath& FCFAST::CompartmentData() const
{
    return P->m_CompartmentData;
}

uint FCFAST::SetCompartmentData( const FilePath& val )
{
    NE_PROP_SET_EVAL(CompartmentData, P, val );
    return NE::kReturnSuccess;
}

const FilePath& FCFAST::WallData() const
{
    return P->m_WallData;
}

uint FCFAST::SetWallData( const FilePath& val )
{
    NE_PROP_SET_EVAL(WallData, P, val );
    return NE::kReturnSuccess;
}

const FilePath& FCFAST::SurfaceData() const
{
    return P->m_SurfaceData;
}

uint FCFAST::SetSurfaceData( const FilePath& val )
{
    NE_PROP_SET_EVAL(SurfaceData, P, val );
    return NE::kReturnSuccess;
}



QString FCFAST::toJson()
{
    // {
    //            "simulation time": 900,
    //            "pressure": 101325,
    //            "humidity": 50,
    //            "interior temperature": 20,
    //            "exterior temperature": 20,
    //            "cfast props":
    //            {
    //                "text output interval": 50,
    //                "spreadsheet output interval": 10,
    //                "smokeview output interval": 10,
    //                "maximum time step": 2,
    //                "lower oxygen limit": 0.15,
    //                "adiabatic surfaces": "false"
    //            },
    //            "fds props": []}
    // }
    QJsonObject simInfoJson;
    simInfoJson["simulation time"] = SimTime();
    qDebug() << "SimTime " << SimTime();
    simInfoJson["pressure"] = Pressure();
    simInfoJson["humidity"] = Humidity();
    simInfoJson["interior temperature"] = IntTemp();
    simInfoJson["exterior temperature"] = ExtTemp();

    QJsonObject cfastJson;
    cfastJson[ "text output interval" ] = TextOutputInt();
    cfastJson[ "spreadsheet output interval" ] = SpreadSheetOutputInt();
    cfastJson[ "smokeview output interval" ] = SmokeViewOutputInt();
    cfastJson[ "maximum time step" ] = MaxTimeStep();
    cfastJson[ "lower oxygen limit" ] = LowerO2Limit();
    cfastJson[ "adiabatic surfaces" ] = Adiabatic();

    simInfoJson["cfast props" ] = cfastJson;

    QJsonArray fdsJson;

    simInfoJson["fds props" ] = fdsJson;

    QJsonDocument simInfoJsonDoc( simInfoJson );
    QString sSimInfoJson = simInfoJsonDoc.toJson();

    return sSimInfoJson;
}


void FCFAST::fromJson(QString jsonStr)
{
    (void) jsonStr;
}

FCFASTFactory::FCFASTFactory(NEManager* m) : NENodeFactory(m) {}

FCFASTFactory::~FCFASTFactory(){}

QString FCFASTFactory::nodeName() { return "CFAST"; }

uint FCFASTFactory::nodeType(){ return NE::kObjectGroup;}

QString FCFASTFactory::nodeInherits() { return "Data";}

QString FCFASTFactory::objectName() { return "CFAST"; }

QString FCFASTFactory::nodeVersion() { return "0.1"; }

QString FCFASTFactory::nodeVendor() { return "F Dynamics"; }

QString FCFASTFactory::nodeHelp() { return "CFAST Simulation Node"; }

NEObject*FCFASTFactory::createInstance() { return new FCFAST( this );	}
