#include "Manager/NEMeshVolGeometryManager.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Spatial/NESpatialGroup.h"
#include "Base/NENodeFactory.h"

#include "Mesh/NEMeshConstructor.h"

#include "Mesh/NEZone.h"
#include "Mesh/NESpatialMesh.h"

#if NE_VOLUME
#include "Volume/Core/NEVolumeNode.h"
#endif


#include "Utility/neutrino_config.h"

class NEMeshVolGeometryManagerPrivate
{
public:
    NEMeshVolGeometryManagerPrivate(NEMeshVolGeometryManager* s) : S(s)
    {

    }
    void loadPlugins();
    void loadStatics();
    //! Deletes the given folder recursively
    bool recRmDir(const QString &aDirName);
    //! List of all mesh constructor.
    QList<NEMeshConstructor*> m_MeshConstructor;
    //! List of all mesh constructors
    QList<NEVolumeNode*> m_Volumes;
private:
    NEMeshVolGeometryManager * S;
};

/*!
        Description.
*/
uint NEMeshVolGeometryManager::init()
{
    P->loadStatics();
    P->loadPlugins();
    return NE::kReturnSuccess;
}

void NEMeshVolGeometryManager::deleteChildNode(NENode* node)
{
    if(NESpatialGroup* gr = qobject_cast<NESpatialGroup*>(node)){
        for(QObject* obj : gr->children()){
            if(NENode* child = qobject_cast<NENode*>(obj))
            {
                deleteChildNode(child);
                objects().remove(obj->objectName());
                delete child;
            }
        }
    }
}

void NEMeshVolGeometryManager::deInit()
{

    //commented by gustaw, cause crash when closing program.
//    for(NENode* node : objects()){
//        deleteChildNode(node);
//        delete node;
//    }
//    objects().clear();

//    for(NENodeFactory* factory : factories())
//    {
//        delete factory;
//    }
//    factories().clear();
}

NEMeshVolGeometryManager::NEMeshVolGeometryManager(QObject* scene) : NEGeometryManager(scene)
{
    P = new NEMeshVolGeometryManagerPrivate(this);
    QObject::connect( &NESCENE, &NESceneManager::deInitMeshVolGeometryManager, this, &NEMeshVolGeometryManager::deInit, Qt::DirectConnection );
}

NEMeshVolGeometryManager::~NEMeshVolGeometryManager()
{
    delete P;
}
/*!
        Description.
*/
NENode* NEMeshVolGeometryManager::createObject(QString type, QString name, const NEObject* parent)
{
    // Call the parent NEManager::createObject function first
    NENode* tmpObject = NEManager::createObject(type, name, parent);
    if( tmpObject == NULL )
        return tmpObject;

//    // Add object to objects() map
//    objects().insert(name, tmpObject);

    if( NEMeshConstructor* meshConstructor = qobject_cast< NEMeshConstructor* > (tmpObject) )
    {
        P->m_MeshConstructor.append( meshConstructor );
    }
    if( NEVolumeNode* volume = qobject_cast< NEVolumeNode* >( tmpObject ))
    {
        P->m_Volumes.append( volume );
    }
    // Return the object;
    return tmpObject;
}

/*!
        Description.
*/
NENode* NEMeshVolGeometryManager::deleteObject(const QString& type , const QString& name, uint trashIndex)
{
    // remove the object from the objects() map and get the object
    NENode* object = NEManager::deleteObject(type, name, trashIndex);

    if(NEMeshConstructor* meshConstructor = qobject_cast<NEMeshConstructor*> (object))
    {
        P->m_MeshConstructor.removeAll(meshConstructor);

        // Deletes the directory of the node from the disk
        TrashDir(NESCENE.simDataSaveDirectory() + "/Geometries/Meshes/" + name, trashIndex);
    }
    if( NEVolumeNode* volume = qobject_cast< NEVolumeNode *>( object ))
    {
        P->m_Volumes.removeAll( volume );
    }

    return object;
}

NENode* NEMeshVolGeometryManager::deleteObject(const QString& type , const QString& name)
{
    // remove the object from the objects() map and get the object
    NENode* object = NEManager::deleteObject(type, name);

    if(NEMeshConstructor* meshConstructor = qobject_cast<NEMeshConstructor*> (object))
    {
        P->m_MeshConstructor.removeAll(meshConstructor);
    }
    if( NEVolumeNode* volume = qobject_cast< NEVolumeNode *>( object ))
    {
        P->m_Volumes.removeAll( volume );
    }

    return object;
}


/*!
        Description.
*/
uint NEMeshVolGeometryManager::loadFrameData(QString dataPath)
{
    QString meshPath( dataPath + "/Geometries/Meshes" );

    Q_FOREACH(NEMeshConstructor* meshConstructor, P->m_MeshConstructor)
    {
        if(meshConstructor->Behavior() == NENode::Cache || (!NESCENE.solveMode() && meshConstructor->Behavior() != NENode::Inactive))
        {
            meshConstructor->loadFrameData(meshPath + "/" + meshConstructor->objectName());
            meshConstructor->updateDrawingBuffers();
        }
    }

    Q_FOREACH(NEVolumeNode* volNode, P->m_Volumes)
    {
        QString volPath(dataPath + "/Geometries/Volumes");

        if(volNode->Behavior() == NENode::Active)
        {
            NEBASE.printEntryInfo( "NEMeshVolGeometryManager", "update" );
            volNode->loadFrameData( volPath + "/" + volNode->objectName(), NESCENE.CurrentFrame() );
        }
    }

    return NE::kReturnSuccess;
}

/*!
        Description.
*/
uint NEMeshVolGeometryManager::saveFrameData(QString dataPath)
{
    QDir dataDir(dataPath);

    QString geoPath(dataPath + "/Geometries");
    dataDir.mkdir("Geometries");
    dataDir.setPath(geoPath);
    dataDir.mkdir("Meshes");
    QString meshPath(geoPath + "/Meshes");
    QDir meshDataDir(meshPath);

    Q_FOREACH(NEMeshConstructor* meshConstructor, P->m_MeshConstructor)
    {
        if(meshConstructor->Behavior() == NENode::Active)
        {
            meshDataDir.mkdir( meshConstructor->objectName() );
            meshConstructor->saveFrameData( meshPath + "/" + meshConstructor->objectName());
            meshConstructor->updateFrameCachingInformation( NESCENE.CurrentFrame() );
        }
    }


    dataDir.mkdir( "Volumes");
    QString volPath( geoPath + "/Volumes" );
    QDir volDataDir( volPath );

    Q_FOREACH( NEVolumeNode* volNode, P->m_Volumes )
    {
        if(volNode->Behavior() == NENode::Active)
        {
            volDataDir.mkdir( volNode->objectName() );
            volNode->saveFrameData( volPath + "/" + volNode->objectName() , NESCENE.CurrentFrame() );
            volNode->updateFrameCachingInformation( NESCENE.CurrentFrame() );
        }
    }

    return NE::kReturnSuccess;
}



uint NEMeshVolGeometryManager::update()
{
    Q_FOREACH(NEVolumeNode* volNode, P->m_Volumes)
    {
        if(volNode->Behavior() == NENode::Active)
        {
            NEBASE.printEntryInfo("NEMeshVolGeometryManager", "update");
            volNode->update();
        }
    }

    Q_FOREACH(NEMeshConstructor* meshConstructor, P->m_MeshConstructor)
    {
        if(meshConstructor->Behavior() == NENode::Active)
        {
            NEBASE.printEntryInfo("NEMeshVolGeometryManager", "update");
            meshConstructor->update();
        }
    }

    return NE::kReturnSuccess;
}


void NEMeshVolGeometryManagerPrivate::loadStatics()
{
    EXEC_IF(1, S->addStaticToMaps(new NESpatialGroupFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NESpatialMeshFactory(S)));
    EXEC_IF(1, S->addStaticToMaps(new NEZoneFactory(S)));
}

void NEMeshVolGeometryManagerPrivate::loadPlugins()
{
#ifdef _DEBUG
    S->addDynamicsToMaps( "Plugins/Debug/Spatial/Geometry" );
#else
    S->addDynamicsToMaps( "Plugins/Release/Spatial/Geometry" );
#endif
}

/*!
        Description.
*/
uint NEMeshVolGeometryManager::cleanSavedData(QString dataPath)
{
    QString path = dataPath + "/Geometries";

    Q_FOREACH(NEMeshConstructor* meshConstructor, P->m_MeshConstructor)
    {
        meshConstructor->SetFrameInfo(-1, 0, 0);
    }

    if(P->recRmDir(dataPath + "/Geometries"))
        return NE::kReturnSuccess;
    else
        return NE::kReturnFail;

}

uint NEMeshVolGeometryManager::cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame)
{

    uint returnValue = NE::kReturnSuccess;

    Q_FOREACH(NEMeshConstructor* meshConstructor, P->m_MeshConstructor)
    {
        QStringList dataPaths;
        dataPaths << dataPath + "/Geometries/Meshes/" + meshConstructor->objectName();
        dataPaths << dataPath + "/Geometries/Meshes/" + meshConstructor->objectName() + "/MotionVector";
        dataPaths << dataPath + "/Geometries/Meshes/" + meshConstructor->objectName() + "/TriMesh";

        //crop cache information
        meshConstructor->cropFrameCachingInformation(fromFrame, toFrame, moveToZeroFrame);

        //remove and rename cache files
        Q_FOREACH(QString dirName, dataPaths)
        {
            returnValue = meshConstructor->cropCacheFiles(dirName, fromFrame, toFrame, moveToZeroFrame);
        }
    }

    return returnValue;
}

bool NEMeshVolGeometryManagerPrivate::recRmDir(const QString& aDirName)
{
    QDir dirName(aDirName);

    if(dirName.isRelativePath(aDirName))      //If not absolute path, make it absolute
        dirName.makeAbsolute();

    QFileInfoList fList = dirName.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

    for(int i = 0; i < fList.count(); ++i)
    {
        QFileInfo file(fList.at(i));

        if(file.isDir())
            recRmDir(file.absoluteFilePath());
        else
            QFile::remove(file.absoluteFilePath());
    }

    dirName.cdUp();

    return dirName.rmdir(aDirName);
}

bool NEMeshVolGeometryManager::hasActiveGeometryConstructors()
{
    Q_FOREACH(NEMeshConstructor* m, P->m_MeshConstructor)
    {
        if(m->Behavior() == NENode::NENodeState::Active)
            return true;
    }

    return false;
}
