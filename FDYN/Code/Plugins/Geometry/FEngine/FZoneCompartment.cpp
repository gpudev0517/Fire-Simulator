#include <QJsonObject>

#include "FZoneCompartment.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Mesh/NEZoneCompartment.h"
#include "FZone.h"

class FZoneCompartmentPrivate
{
public:
    FZoneCompartmentPrivate(FZoneCompartment* s) : S(s)
    {
    }

    QString m_CompartmentName;

    FilePath m_ZoneFile;
    uint m_ConnectorId;
    QString m_Issues;

    int pId() const;
    void setPId(int pId);

    QString m_Model3DInfo;

private:
    FZoneCompartment* S;
};

static void initProperties()
{

}

FZoneCompartment::FZoneCompartment(NENodeFactory* factory) : NEZoneCompartment(factory)
{
    P = new FZoneCompartmentPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);

    SetPosition(vec3f(0.0, 0.5, 0.0));
    SetPivot(vec3f(-0, -0.5, -0));
    SetColor( QColor( 239, 67, 0 ) );

    SetOccludeShadows( false );

    setOutputs( outputs() | NE::kIOTriangleMesh );
}

FZoneCompartment::~FZoneCompartment()
{
    delete P;
}

uint FZoneCompartment::init(uint initMode)
{
    int ret = NEZoneCompartment::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    if(compartmentName().isEmpty())
        setCompartmentName(objectName());

    updateParentCompBB();

    return NE::kReturnSuccess;
}

void FZoneCompartment::initializeGUIActions()
{
    NEZoneCompartment::initializeGUIActions();

    //QObject* actionReloadObjFile = nullptr;
    //QObject* actionClearObjFile = nullptr;

#if 0
    QString stActionReloadObjFile("Reload Zone File");
    emit registerAction(stActionReloadObjFile, actionReloadObjFile);
    connect(actionReloadObjFile, SIGNAL(triggered()), this, SLOT(NESpatialMeshPrivate::reloadObjFile()));
    addAction(actionReloadObjFile);

    QString strActionClearObjFile("Clear Zone File");
    emit registerAction( stActionReloadObjFile, actionClearObjFile );
    connect(actionClearObjFile, SIGNAL(triggered()), this, SLOT(NESpatialMeshPrivate::clearObjFile()));
    addAction(actionClearObjFile);
#endif
}

const QString& FZoneCompartment::compartmentName()
{
    return P->m_CompartmentName;
}

uint FZoneCompartment::setCompartmentName(const QString& compartmentName)
{
    P->m_CompartmentName = compartmentName;

    return NE::kReturnSuccess;
}

void FZoneCompartment::resetProperties()
{
    NEZoneCompartment::resetProperties();
}

const QString& FZoneCompartment::model3DInfo()
{
    return P->m_Model3DInfo;
}

void FZoneCompartment::setModel3DInfo(const QString& model3dinfo)
{
    P->m_Model3DInfo = model3dinfo;
}

QString FZoneCompartment::nodeDescription()
{
    return Desc();
}

void FZoneCompartment::propertiesFromJSON(QString jsonStr)
{
    (void)jsonStr;
    return;
}

QString FZoneCompartment::propertiesToJSON()
{
    QJsonObject   jsonObject ;

    jsonObject["name"] = objectName();

    //jsonObject["model3DInfo"] = getJSONFromProperties( zone );

    QJsonDocument jsonDoc( jsonObject );

    QString jsonStr( jsonDoc.toJson() );
    //std::cout << "JSON SAVE " << jsonStr.toStdString() << std::endl;

    return jsonStr;
}

QString FZoneCompartment::categoryType()
{
    return "Boundaries";
}

void FZoneCompartment::updateParentCompBB()
{
    if(FZone* zone = qobject_cast<FZone*>(this->parent()))
        zone->computeCompBB();
}

void FZoneCompartment::updateTransform()
{
    NEZoneCompartment::updateTransform();

    updateParentCompBB();
}

FZoneCompartmentFactory::FZoneCompartmentFactory(NEManager* m) : NENodeFactory(m)
{
}

FZoneCompartmentFactory::~FZoneCompartmentFactory()
{
}

QString FZoneCompartmentFactory::nodeName()
{
    return "Fire Zone Boundary";
}

QStringList FZoneCompartmentFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("FZoneCompartment");

    return aNames;
}

uint FZoneCompartmentFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString FZoneCompartmentFactory::nodeInherits()
{
    return "Geometry";
}

QString FZoneCompartmentFactory::objectName()
{
    return "Boundary";
}

QString FZoneCompartmentFactory::nodeVersion()
{
    return "0.1";
}

QString FZoneCompartmentFactory::nodeVendor()
{
    return "FRI3D";
}

QString FZoneCompartmentFactory::nodeHelp()
{
    return "Describing a Fire Zone Container";
}

NEObject* FZoneCompartmentFactory::createInstance()
{
    return new FZoneCompartment(this);
}
