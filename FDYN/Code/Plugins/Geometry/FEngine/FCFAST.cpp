#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Core/Base/NEBase.h"
#include "Core/Base/NESceneManager.h"
#include "FCFAST.h"
#include "FCFASTPlane.h"
#include "FEngine.h"
#include "FZone.h"


class FCFASTPrivate
{
public:
    FCFASTPrivate(FCFAST* s) : S(s)
    {
        m_cfastosd = nullptr;
        m_XPlane = nullptr;
        m_YPlane = nullptr;
        m_ZPlane = nullptr;
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

    FOSDCFAST* m_cfastosd;
    //Color OSD Data

    FCFASTPlane *m_XPlane;
    FCFASTPlane *m_YPlane;
    FCFASTPlane *m_ZPlane;


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
    if( P ){
        delete P;
        P= NULL;
    }
}

uint FCFAST::init( uint initMode )
{
    uint ret = NESpatialGroup::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    //FEngine* engine = qobject_cast<FEngine *>(this->parent());

    P->m_cfastosd = NESCENE.findChild<FOSDCFAST *>( "CFASTOSD" );
    if(!P->m_cfastosd)
    {
        P->m_cfastosd = qobject_cast< FOSDCFAST *>( NESCENE.createObject("CFASTOSD", "CFASTOSD",  this ) );
    }

    P->m_XPlane = NESCENE.findChild<FCFASTPlane *>( "X" );
    if(!P->m_XPlane)
    {
        P->m_XPlane = qobject_cast< FCFASTPlane *>(NESCENE.createObject("CFASTPlane", "X",  this));
        P->m_XPlane->SetPlaneDirection(FCFASTPlane::X);
    }

    P->m_YPlane = NESCENE.findChild<FCFASTPlane *>( "Y" );
    if(!P->m_YPlane)
    {
        P->m_YPlane = qobject_cast< FCFASTPlane *>( NESCENE.createObject("CFASTPlane", "Y",  this));
        P->m_YPlane->SetPlaneDirection(FCFASTPlane::Y);
    }

    P->m_ZPlane = NESCENE.findChild<FCFASTPlane *>( "Z" );
    if( !P->m_ZPlane )
    {
        P->m_ZPlane = qobject_cast< FCFASTPlane *>( NESCENE.createObject("CFASTPlane", "Z",  this));
        P->m_ZPlane->SetPlaneDirection(FCFASTPlane::Z);
    }



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
    return P->m_ZVisPlanePos;
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
    updateCompartmentData(val);
    return NE::kReturnSuccess;
}

const FilePath& FCFAST::WallData() const
{
    return P->m_WallData;
}

uint FCFAST::SetWallData( const FilePath& val )
{
    NE_PROP_SET_EVAL(WallData, P, val );
    updateWallData(val);
    return NE::kReturnSuccess;
}

const FilePath& FCFAST::SurfaceData() const
{
    return P->m_SurfaceData;
}

uint FCFAST::SetSurfaceData( const FilePath& val )
{
    NE_PROP_SET_EVAL(SurfaceData, P, val );
    updateSurfaceData(val);
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

void FCFAST::updateCompartmentData(const FilePath &val)
{
    if(FEngine* engine = qobject_cast<FEngine *>(this->parent()))
    {
        if(FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(engine->activeZoneName())))
        {
            QFile file(val.path());
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
               return;

            zone->clearCompartmentData();

            int upperColumn = 0;
            int lowerColumn = 0;
            int heightColumn = 0;

            QTextStream in(&file);
            QString line = in.readLine();
            int i = 1;
            while (!line.isNull()) {
                line = in.readLine();
                //qDebug() << line;
                QStringList lineList = line.split(",");
                if ( i == 1){
                    for (int  j = 0; j < lineList.size(); j++)
                    {
                        if (lineList[j] == "Upper Layer Temperature")
                            upperColumn = j;
                        else if(lineList[j] == "Lower Layer Temperature")
                            lowerColumn = j;
                        else if(lineList[j] == "Layer Height")
                            heightColumn = j;
                    }
                    qDebug() << "Column numbers" << upperColumn << lowerColumn << heightColumn;
                }
                else if (i == 2)
                {
//                    for (int  j = 0; j < rowNum.size(); j++)
//                        deviceNames.push_back(lineList[rowNum[j]]);
//                    break;
                }
                else if(i > 3 )
                {
                    if(lineList.size() > heightColumn)
                        zone->addLayerHeight(lineList[0].toDouble(), lineList[heightColumn].toDouble());

                    if(lineList.size() > upperColumn)
                        zone->addHigherLayerTemp(lineList[0].toDouble(), lineList[upperColumn].toDouble());

                    if(lineList.size() > lowerColumn)
                        zone->addLowerLayerTemp(lineList[0].toDouble(), lineList[lowerColumn].toDouble());
                }
                i += 1;
            }
        }
    }
}

void FCFAST::updateWallData(const FilePath &val)
{

}

void FCFAST::updateSurfaceData(const FilePath &val)
{

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


class FOSDCFASTPrivate
{
public:
    FOSDCFASTPrivate(FOSDCFAST* s) : S(s)
    {
    }
    int m_TickCount;
    QString m_TitleText;
    FOSDCFAST::FOSDCFASTOrientation m_OSDOrientation;

private:
    FOSDCFAST* S;
};

NE_PROP_DEF(int, FOSDCFAST, TickCount, 5)
NE_PROP_DEF(QString, FOSDCFAST, TitleText, QString("Temperature (C)"))
NE_PROP_DEF(FOSDCFAST::FOSDCFASTOrientation, FOSDCFAST, OSDOrientation, FOSDCFAST::FOSDCFASTOrientation::Left)

static void initPropertiesOSD()
{
    NE_INIT_PROP(FOSDCFAST, TickCount);
    NE_INIT_PROP(FOSDCFAST, TitleText);
    NE_INIT_PROP(FOSDCFAST, OSDOrientation);
}

FOSDCFAST::FOSDCFAST( NENodeFactory * factory): NEOSDBase( factory )
{
    P = new FOSDCFASTPrivate(this);
    CALL_ONCE(initPropertiesOSD);

    P->m_TickCount = NE_DEFAULT(TickCount);
    P->m_TitleText = NE_DEFAULT(TitleText);
    P->m_OSDOrientation = NE_DEFAULT(OSDOrientation);

    SetPosition( vec3f( 0.9, 0.2, 1.0) );
    SetScale( vec3f( 0.5, 0.75, 1.0 ) );
    SetTextOutline( true );
    SetFloatTypePrecision( 2 );
    SetTotalNumberLength( 6 );
    SetFontSize( 10 );
}

FOSDCFAST::~FOSDCFAST()
{
    delete P;
}

bool FOSDCFAST::isManipulatable(NEManipulator manipType)
{
    if (manipType == NEManipulator::kManipTranslate || manipType == NEManipulator::kManipScale)
        return true;
    return false;
}


const int& FOSDCFAST::TickCount() const
{
    return P->m_TickCount;
}

uint FOSDCFAST::SetTickCount(const int& val)
{
    NE_PROP_SET_AND_EVAL(TickCount, P, val);
    return NE::kReturnSuccess;
}

const QString& FOSDCFAST::TitleText() const
{
    return P->m_TitleText;
}



uint FOSDCFAST::SetTitleText(const QString& val)
{
    NE_PROP_SET_AND_EVAL(TitleText, P, val);
    return NE::kReturnSuccess;
}

const FOSDCFAST::FOSDCFASTOrientation& FOSDCFAST::OSDOrientation() const
{
    return P->m_OSDOrientation;
}

uint FOSDCFAST::SetOSDOrientation(const FOSDCFASTOrientation& val)
{
    NE_PROP_SET_EVAL(OSDOrientation, P, val);
    return NE::kReturnSuccess;
}


vec2f FOSDCFAST::osdSize()
{
    vec2f rectSize;

    switch(OSDOrientation())
    {
    case FOSDCFASTOrientation::Left:
    case FOSDCFASTOrientation::Right:
        {
        int wRes = OSD_PARTICLE_COLORBAR_WIDTH * Scalex(); //!< Width of color bar
        int hRes = OSD_PARTICLE_COLORBAR_HEIGHT * Scaley();
        int hTitle = fontHeight() * 2;
        int wMargin = OSD_PARTICLE_MEASURE_MARGIN * Scalex(); //!< Width of margin between color bar and measure
        int widthU = 0; //!< Maximize size of text width

        float minVal = 0;//m_volume->MinColoringRange();
        float maxVal = 1;//m_volume->MaxColoringRange();

        for (int i = 0; i <= TickCount(); i++)
        {
            float s = i / (float)TickCount();
            float val = minVal * (1-s) + maxVal * s;
            QString text;
            if( DisplayNotation() == NEOSDBase::Exponent )
            {
                text = QString("%1").arg( (double)val, 2, 'E', 2 );
            }
            else if( DisplayNotation() == NEOSDBase::FloatingPoint )
            {
                text = QString("%1").arg( (double)val, TotalNumberLength(), 'f', FloatTypePrecision() );
            }
            widthU = MAX(widthU, (float)text.length() * fontWidth() + fontWidth());
        }

        rectSize = vec2f(wRes + wMargin + widthU, hRes + fontHeight() + hTitle);
        }
        break;
    case FOSDCFASTOrientation::Top:
    case FOSDCFASTOrientation::Bottom:
        {
            int wRes = OSD_PARTICLE_COLORBAR_HEIGHT * Scalex(); //!< Width of color bar
            int hRes = OSD_PARTICLE_COLORBAR_WIDTH * Scaley();
            int hTitle = fontHeight() * 2;
            int hMargin = OSD_PARTICLE_MEASURE_MARGIN * Scaley(); //!< Height of margin between color bar and measure
            int widthU = 0; //!< Maximize size of text width

            rectSize = vec2f(wRes, hTitle + hRes + hMargin + fontHeight());
        }
        break;
    default:
        rectSize = NEOSDBase::osdSize();
        break;
    }

    return rectSize;
}


FOSDCFASTFactory::FOSDCFASTFactory(NEManager* m) : NENodeFactory(m) {}

FOSDCFASTFactory::~FOSDCFASTFactory() {}

QString FOSDCFASTFactory::nodeName() { return "CFASTOSD"; }

uint FOSDCFASTFactory::nodeType(){ return NE::kObjectOSD;}

QString FOSDCFASTFactory::nodeInherits() { return "Visuals";}

QString FOSDCFASTFactory::objectName() { return "CFASTOSD"; }

QString FOSDCFASTFactory::nodeVersion() { return "0.1"; }

QString FOSDCFASTFactory::nodeVendor() { return "FDYN"; }

QString FOSDCFASTFactory::nodeHelp() { return "CFASTOSD"; }

NEObject*FOSDCFASTFactory::createInstance() { return new FOSDCFAST(this); }

