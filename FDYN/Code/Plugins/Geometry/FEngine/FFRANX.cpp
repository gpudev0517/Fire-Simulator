#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "Core/Base/NEBase.h"
#include "Core/Base/NESceneManager.h"
#include "FFRANX.h"


class FFRANXPrivate
{
public:
    FFRANXPrivate(FFRANX* s) : S(s)
    {
    }

    /* Properties */


    FilePath m_FRANXFile;
    QList< FRANXProperties > m_FRANXProperties;

private:
    FFRANX* S;
};



NE_PROP_DEF(FilePath, FFRANX, FRANXFile, FilePath(FilePath::kRefeFileType))

static void initProperties()
{
    NE_INIT_PROP(FFRANX, FRANXFile);

}

FFRANX::FFRANX( NENodeFactory* factory ) : NESpatialGroup( factory )
{
    P = new FFRANXPrivate( this );

    CALL_ONCE(initProperties);
    P->m_FRANXFile = NE_DEFAULT(FRANXFile);


    setInputs( inputs()|NE::kIOExtent );
}

FFRANX::~FFRANX()
{
    delete P;
}

uint FFRANX::init( uint initMode )
{
    CALL_ONCE(initProperties);

    P->m_FRANXFile.setFilter(ReferenceFilter());
    P->m_FRANXFile.setDialogType(FilePath::kRefeFileType);

    uint ret = NESpatialGroup::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FFRANX::resetProperties()
{
    NESpatialGroup::resetProperties();


}

uint FFRANX::SetFRANXFile(const FilePath& val)
{
    NE_PROP_SET_EVAL(FRANXFile, P, val);

    P->m_FRANXFile = val;


    if (!(QFile(P->m_FRANXFile.path()).exists()))
        return NE::kReturnFileNotFound;



    return NE::kReturnSuccess;
}

const FilePath& FFRANX::FRANXFile() const
{
    return P->m_FRANXFile;
}



QString FFRANX::toJson()
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


    QJsonDocument simInfoJsonDoc( simInfoJson );
    QString sSimInfoJson = simInfoJsonDoc.toJson();

    return sSimInfoJson;
}


void FFRANX::fromJson(QString jsonStr)
{

}

FFRANXFactory::FFRANXFactory(NEManager* m) : NENodeFactory(m) {}

FFRANXFactory::~FFRANXFactory(){}

QString FFRANXFactory::nodeName() { return "FFRANX"; }

uint FFRANXFactory::nodeType(){ return NE::kObjectGroup;}

QString FFRANXFactory::nodeInherits() { return "Data";}

QString FFRANXFactory::objectName() { return "FRANX"; }

QString FFRANXFactory::nodeVersion() { return "0.1"; }

QString FFRANXFactory::nodeVendor() { return "F Dynamics"; }

QString FFRANXFactory::nodeHelp() { return "FRANX Simulation Node"; }

NEObject*FFRANXFactory::createInstance() { return new FFRANX( this );	}
