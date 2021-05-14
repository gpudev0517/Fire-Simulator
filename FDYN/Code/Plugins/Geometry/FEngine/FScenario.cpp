#include "FZone.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "FScenario.h"
#include "FZone.h"
#include "FZoneItem.h"

NE_PROP_DEF(FilePath, FScenario, ScenarioFile, FilePath(FilePath::kRefeFileType))
NE_PROP_DEF(uint, FScenario, ConnectorID, 0)
NE_PROP_DEF(QString, FScenario, Issues, QString(""))

class FScenarioPrivate
{
public:
    FScenarioPrivate(FScenario* s) : S(s)
    {
    }

    QString m_ScenarioName;
    QList< QString > m_Zones;
    QList< QString > m_ScenarioItems;
    QList< QString > m_SubItems;

    FilePath m_ScenarioFile;
    uint m_ConnectorID;
    QString m_Issues;

    int pId() const;
    void setPId(int pId);

    QString m_Model3DInfo;

private:
    FScenario* S;
};

static void initProperties()
{
    NE_INIT_PROP(FScenario, ScenarioFile);
    NE_INIT_PROP(FScenario, ConnectorID );
    NE_INIT_PROP(FScenario, Issues );
}

FScenario::FScenario(NENodeFactory* factory) : NEScenario(factory)
{
    P = new FScenarioPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_ScenarioFile = NE_DEFAULT(ScenarioFile);
    P->m_ConnectorID = NE_DEFAULT(ConnectorID);
    P->m_Issues = NE_DEFAULT(Issues);

    setOutputs( outputs() | NE::kIOTriangleMesh );
}

FScenario::~FScenario()
{
    P->m_Zones.clear();

    delete P;
}

uint FScenario::init(uint initMode)
{
    int ret = NEScenario::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    P->m_ScenarioFile.setFilter(ReferenceFilter());
    P->m_ScenarioFile.setDialogType(FilePath::kRefeFileType);

    if(P->m_ScenarioFile.path() != "")
        loadScenarioFile(P->m_ScenarioFile.path());

    return NE::kReturnSuccess;
}

const FilePath& FScenario::ScenarioFile() const
{
    return P->m_ScenarioFile;
}

uint FScenario::SetScenarioFile(const FilePath& val)
{
    NE_PROP_SET_EVAL(ScenarioFile, P, val);
    if (P->m_ScenarioFile.dialogType() != FilePath::kRefeFileType
     && P->m_ScenarioFile.dialogType() != FilePath::kImpoFileType)
        P->m_ScenarioFile.setDialogType(FilePath::kRefeFileType);

    if (!(QFile(P->m_ScenarioFile.path()).exists()))
        return NE::kReturnFileNotFound;

    loadScenarioFile(P->m_ScenarioFile);

    return NE::kReturnSuccess;
}

const uint& FScenario::ConnectorID() const
{
    return P->m_ConnectorID;
}

uint FScenario::SetConnectorID(const uint& val)
{
    NE_PROP_SET_EVAL(ConnectorID, P, val);

    return NE::kReturnSuccess;
}

const QString& FScenario::Issues() const
{
    return P->m_Issues;
}

uint FScenario::SetIssues(const QString& val)
{
    NE_PROP_SET_EVAL(Issues, P, val);

    return NE::kReturnSuccess;
}

void FScenario::initializeGUIActions()
{
    NEScenario::initializeGUIActions();

    QObject* actionReloadObjFile = NULL;
    QObject* actionClearObjFile = NULL;
#if 0
    QString stActionReloadObjFile("Reload Scenario File");
    emit registerAction(stActionReloadObjFile, actionReloadObjFile);
    connect(actionReloadObjFile, SIGNAL(triggered()), this, SLOT(NESpatialMeshPrivate::reloadObjFile()));
    addAction(actionReloadObjFile);


    QString strActionClearObjFile("Clear Scenario File");
    emit registerAction( stActionReloadObjFile, actionClearObjFile );
    connect(actionClearObjFile, SIGNAL(triggered()), this, SLOT(NESpatialMeshPrivate::clearObjFile()));
    addAction(actionClearObjFile);
#endif
}

QString FScenario::scenarioName()
{
    return P->m_ScenarioName;
}

uint FScenario::setScenarioName(const QString scenarioName)
{
    P->m_ScenarioName = scenarioName;

    return NE::kReturnSuccess;
}

void FScenario::resetProperties()
{
    NEScenario::resetProperties();

    ResetScenarioFile();
    ResetConnectorID();
    ResetIssues();

    reloadScenarioFile();
}

void FScenario::reloadScenarioFile()
{
    loadScenarioFile( P->m_ScenarioFile );
}

QList<QString> &FScenario::zones()
{
    return P->m_Zones;
}

void FScenario::clearScenarioFile()
{
    P->m_ScenarioFile = FilePath("");
    P->m_ScenarioFile.setFilter(ReferenceFilter());
    P->m_ScenarioFile.setDialogType(FilePath::kRefeFileType);
}

void FScenario::loadScenarioFile(const QString& filePath)
{
}

QString FScenario::model3DInfo()
{
    return P->m_Model3DInfo;
}

void FScenario::setModel3DInfo(QString model3dinfo)
{
    P->m_Model3DInfo = model3dinfo;
}

QString FScenario::nodeDescription()
{
    return Desc();
}

FScenarioFactory::FScenarioFactory(NEManager* m) : NENodeFactory(m)
{

}

FScenarioFactory::~FScenarioFactory()
{

}

QString FScenarioFactory::nodeName()
{
    return "Fire Scenario";
}

QStringList FScenarioFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("FRI3D");

    return aNames;
}

uint FScenarioFactory::nodeType()
{
    return NE::kObjectParticleRigid;
}

QString FScenarioFactory::nodeInherits()
{
    return "Groups";
}

QString FScenarioFactory::objectName()
{
    return "FScenario";
}

QString FScenarioFactory::nodeVersion()
{
    return "0.1";
}

QString FScenarioFactory::nodeVendor()
{
    return "FRI3D";
}

QString FScenarioFactory::nodeHelp()
{
    return "FRI3D";
}

NEObject* FScenarioFactory::createInstance()
{
    return new FScenario(this);
}
