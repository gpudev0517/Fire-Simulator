#include <QFileInfo>
#include <QStandardPaths>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>

#include "FEngine.h"
#include "FEngineWorker.h"
#include "pythonqt/src/PythonQtObjectPtr.h"

#include "FZone.h"
#include "FZoneItem.h"
#include "FScenario.h"
#include "FZoneCompartment.h"
#include "FFire.h"
#include "FMechVent.h"
#include "FVent.h"
#include "FFireDetect.h"
#include "FCables.h"

#include "globals_Core.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Manager/NEGeometryManager.h"
#include "Base/NEGUICallInterface.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Spatial/NECamera.h"

#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFireMaterial.h"
#include "Utility/NECustomVariants.h"
#include "Utility/NECableProperty.h"

#include "FCFAST.h"

NE_PROP_DEF(FilePath, FEngine, DBFile, FilePath(FilePath::kRefeFileType))

//
// Failure Items
//

struct FailedItems
{
    QList<int> scenarioID;
    QList<int> failedItemID;
    QList<double> failedTime;
    QList<QString> failOutput;

    void clear()
    {
        scenarioID.clear();
        failedItemID.clear();
        failedTime.clear();
        failOutput.clear();
    }
};

///
/// \brief The FEnginePrivate class
///
class FEnginePrivate
{
public:

    friend class FEngineWorker;

    FEnginePrivate(FEngine* s) : S(s)
    {
        m_activeScenarioName = QString("");
        m_activeZoneName = QString("");

        m_JsonPlantTopItemLib[ "sources" ]          = FZoneItem::Sources;
        m_JsonPlantTopItemLib[ "trays" ]            = FZoneItem::Trays;
        m_JsonPlantTopItemLib[ "conduits" ]         = FZoneItem::Conduits;
        m_JsonPlantTopItemLib[ "other raceways" ]   = FZoneItem::OtherRaceways;
        m_JsonPlantTopItemLib[ "equipment" ]        = FZoneItem::Equipments;
        m_JsonPlantTopItemLib[ "unknown" ]          = FZoneItem::Unknowns ;
        m_JsonPlantTopItemLib[ "physical only" ]    = FZoneItem::Physical;
        m_JsonPlantSubItemLib[ "cables" ]           = FZoneItem::Cable;

        m_plantItemLib[ FZoneItem::Cable ]            = "piCable";
        m_plantItemLib[ FZoneItem::Sources ]          = "piSource";
        m_plantItemLib[ FZoneItem::Trays ]            = "piTray";
        m_plantItemLib[ FZoneItem::Conduits ]         = "piConduit";
        m_plantItemLib[ FZoneItem::Component ]        = "piEquipment";
        m_plantItemLib[ FZoneItem::Equipments ]       = "piEquipment";
        m_plantItemLib[ FZoneItem::Unknowns ]         = "piUnknown";
        m_plantItemLib[ FZoneItem::Physical ]         = "piEquipment";
        m_plantItemLib[ FZoneItem::OtherRaceways ]    = "piOtherRaceway";
        m_plantItemLib[ FZoneItem::Vent ]             = "siVent";

        m_Recover                                     = false;
        m_WorkingDir                                  = "";
        m_SaveUnsaved                                 = false;
        m_UIDir                                       = "";
        m_saveUI                                      = false;
        m_loadUI                                      = false;
    }

    FilePath m_DBFile;
    FilePath m_FRANXFile;

    std::map< QString, FZoneItem::FItemType > m_JsonPlantTopItemLib;
    std::map< QString, FZoneItem::FItemType > m_JsonPlantSubItemLib;

    std::map< FZoneItem::FItemType, QString > m_plantItemLib;

    // Map for compartments for zone
    QMap< uint, QMap< uint, std::shared_ptr< FZoneCompartment > > > m_ZoneCompartments;
    // Map for vents for zone
    QMap< uint, QMap< uint, std::shared_ptr< FVent > > > m_ZoneVents;
    // Map for Top items for zone
    QMap< uint, QMap< uint, std::shared_ptr< FZoneItem > > > m_ZoneItems;
    // Map for vents for scenario
    QMap< uint, QMap< uint, std::shared_ptr< FVent > > > m_ScenarioVents;
    // Map for Top items for scenario
    QMap< uint, QMap< uint, std::shared_ptr< FZoneItem > > > m_ScenarioItems;
    // Map for sub items for zone
    QMap< uint, QMap< uint, std::shared_ptr< FZoneItem > > > m_SubSceneZoneItems;
    // Map for sub items for scenario
    QMap< uint, QMap< uint, std::shared_ptr< FZoneItem > > > m_SubSceneScenarioItems;

    QList<QPair<int, QString>> m_ZoneIdNamePairs;
    QList<QPair<int, QString>> m_ScnIdNamePairs;

    QList<NESurfaceMaterial> m_SurfaceMaterials;
    QList<NESurfaceMaterial> m_DefaultSurfaceMaterials;
    QList<int> m_DefaultSurfaceMaterialAssignedIndexes;
    QList<NESurfaceMaterial> m_SurfaceMaterialsWithAssignedDefaults;
    QList<NEFireMaterial> m_FireMaterials;
    QList<NEFailureCriteria> m_FailCriteriaList;
    QList<NEVentCriteria> m_VentCriteriaList;
    QList<NECableProperty> m_CablePropertyList;
    std::vector<QList<QString>> m_CableOptions;

    QList<FRANXProperties> m_FRANXScenarios;

    QString     m_activeZoneName;
    QString     m_activeScenarioName;

    QLibrary    m_pLibrary;

    FailedItems m_FailedItems;

    bool        m_Recover;
    QString     m_WorkingDir;
    QString     m_UIDir;
    bool        m_loadUI;
    bool        m_saveUI;

    bool        m_SaveUnsaved;

public:

    uint loadDBFile( const QString& file );

    //
    // Zone Related
    //
    std::shared_ptr<FZone> createZone(QString zoneName, QString jsonData);
    void createZoneProperties(std::shared_ptr<FZone> zone, QJsonObject& jsonObject  );
    FZone *createSceneZone( NESpatialGroup* parent, std::shared_ptr< FZone >& zone );
    std::shared_ptr<FZone> createZoneFromJSON(QString jsonData);

    QString createJSONFromSceneZone( FZone *zone );
    //void   saveSceneZone( QString zoneName );

    //
    // Scenario Related
    //
    std::shared_ptr<FScenario> createScenario(QString scenarioName,
                                              QString jsonData);
    void createScenarioProperties(std::shared_ptr<FScenario> zone, QJsonObject &jsonObject);
    FScenario* createSceneScenario(NESpatialGroup* parent, std::shared_ptr<FScenario> zone );

    std::shared_ptr<FScenario> createScenarioFromJSON(QString &jsonData);
    QString createJSONFromSceneScenario( FScenario* scn );
    QString createJSONFromScenario(FScenario *scn);

    //void saveSceneScenario(QString scnName );

    //
    //  Item Related
    //
    void createSceneZoneItem(NESpatialGroup* parent, std::shared_ptr<FZoneItem>& item, FZone* zone );
    void createZoneItems(std::shared_ptr<FZone> zone,
                         QJsonObject &jsonObject);
    void createScenarioItems(std::shared_ptr<FScenario> scenario,
                             QJsonObject &jsonObject);
    void createSceneScenarioItem(FScenario* scenario,
                                    std::shared_ptr<FZoneItem> &item );
    int saveModelItem(QString modelItemName );
    void saveModelItems(QString modelItemName = QString("") );

    void createZoneItemsFromJSON(std::shared_ptr<FZone> zone,
                                 NESpatialGroup* parent,
                                 QJsonValueRef &compsref,
                                 FZoneItem::FItemType itemType);
    void createScenarioItemsFromJSON(std::shared_ptr<FScenario> scenario,
                                     NESpatialGroup* parent,
                                     QJsonValueRef &compsref,
                                     FZoneItem::FItemType itemType);

    void createSimItemsFromJson(std::shared_ptr<NESpatialGroup> parent, const QJsonObject& jsonObject);
    void createVentFromJson(std::shared_ptr<NESpatialGroup> parent, const QJsonObject& jsonObject);

    QString createJSONFromSceneItemName( QString zoneItemName );
    QString createJSONFromSceneItem(FZoneItem* zoneItem);

    //
    // Compartment Related JSON
    //
    QString createJSONFromCompartment(FZoneCompartment* compartment);

    QString createJSONFromVent(FVent* vent);

    //
    // Material related
    //
    //
    QString createJSONFromSurfaceMaterial( NESurfaceMaterial sm );
    NESurfaceMaterial createSurfaceMaterialFromJSON( QString matJson );

    //
    // Fire Material related
    //
    QString createJSONFromFireMaterial( NEFireMaterial fm );
    NEFireMaterial createFireMaterialFromJSON( QString fmJson );

    //
    // Failure criteria related
    //
    QString createJSONFromFailureCriteria(NEFailureCriteria fail);
    NEFailureCriteria createFailureCriteriaFromJSON(QString jsonStr);

    //
    // Vent criteria related
    //
    QString createJSONFromVentCriteria(NEVentCriteria criterion);
    NEVentCriteria createVentCriteriaFromJSON(QString jsonStr);

    //
    // Cable Properties
    //
    QString createJSONFromCableProperties(NECableProperty prop);
    NECableProperty createCablePropertiesFromJSON(QString cableJson);

    //
    // Base function for creating dynamic properties
    //
    void createSpatialProperties(std::shared_ptr<NESpatial> spatial, QJsonObject &jsonObject);

    //
    // FRANX Related
    //
    uint loadFRANXFile(const QString &filePath);
    void createFRANXZoneItems(NESpatialGroup *parent, QJsonObject &jsonObject);
    FZoneItem *createNewFRANXItems(NESpatialGroup *parent, QJsonObject &jsonObject);
    void createFRANXScenarioItems(std::shared_ptr<NESpatialGroup > zone, QJsonObject &jsonObject);

    //
    // PDMS Plant Database
    //
    uint loadPDMSFile(const QString &filePath);

    //
    // Utiltiies for DB interaction
    //

    QStringList GetScenarioListForZone(int zoneID);
    QStringList GetZoneListForScenario(int scnID);

    QList<QPair<int, QString>> getZoneIdNamePairs() const;
    QStringList zoneNames() const;

    QList<QPair<int, QString>> getScnIdNamePairs(QList<FRANXProperties>& franxScns) const;
    QStringList scnNames() const;
    int indexOfScenario(QString scenarioName) const;


    //
    // Delete the newly created physical only item from the zone
    //
    void DeleteItemFromZone(QString itemName);

    //
    //
    void    RemoveItemFromAllScenarios(QString itemName);

    FailedItems getFailedItems(uint scenarioID);
    void updateItemsWithFailureInfo(const FailedItems& failureInfo);

    void updateActiveZoneWithMaterialInfo();
    void updateItemsWithMaterialInfo();

    void updateItemsWithCompartmentInfo();
    void updateItemWithCompartmentInfo(FZoneItem* item);

    void updateVentsWithZoneInfo();
    void updateVentWithZoneInfo(FVent* vent);

    void updateVentsWithCriterionInfo();
    void updateVentWithCriterionInfo(FVent* vent);

    QString GetScenarioFailureOutputDB(int scenarioID);

    void    createSceneScnItem(FScenario *scenario,
                               NESpatialGroup* parent,
                               std::shared_ptr<FZoneItem> &item);

    //
    // All Database related methods/ queries
    //

    bool        NewDB(const char* newdbcstr);
    bool        OpenDB(const char* newdbcstr, bool recover = false);
    bool        SaveProjectAsDB( QString filename );
    bool        SetTempWorkLocDB( QString dirName );
    QString     GetTempWorkLocDB();
    bool        SaveTempWorkDB();
    bool        ProjectHasRecoverDB(QString path);

    bool        ImportPlantProjectsDB( QString projpath, QString franxpath, QString pdmspath );

    QString     GetZonesDB();
    QString     GetZonesForScenarioDB( int scenarioID );
    QString     GetPlantZoneTreeDB( int zoneID );
    QString     GetPlantScenarioTreeDB( int scnID );

    QString     GetScenariosForZoneDB( int zoneID );
    QString     GetScenariosDB();

    int         NewModelItemDB( const QString jsonStr, int zoneID, int parentID );
    int         SaveModelItemDB( const QString jsonStr, int zoneID );
    void        SaveModelItemsDB( const QString jsonStrArray, int zoneID );

    void        DeleteItemDB( QString jsonStr );
    void        RemoveItemDB( QString jsonStr, int parentID );
    int         ItemRefCntDB( QString jsonStr );

    void        AddItemToScenarioDB( QString jsonStr, int scenarioID, int parentID );
    void        RemoveItemFromScenarioDB( QString jsonStr, int connectionID, int parentID );
    void        RemoveItemFromAllScenariosDB(QString jsonStr);

    void        SaveScenarioAsDB( QString jsonStr, char *newName ); // Question about newname
    int         NewScenarioDB( QString jsonStr, int zoneID );
    void        DeleteScenarioDB( QString jsonStr );

    QString     GetFRANXTreeDB( int zoneID, int scenarioID ); // Question
    void        ExcludeFRANXItemDB( QString jsonStr, int scenarioID, int parentID ); // Question
    void        IncludeFRANXItemDB( QString jsonStr, int scenarioID, int parentID ); // Question

    QString     GetBaseFRANXModelLocDB();
    QString     GetFranxMasterLocDB();
    bool        SetFranxMasterLocDB(QString franxloc );

    bool        GetFranxMasterChangedDB();

    void    RunFranxScenarioDB(int scenarioID, QString basefranx, QString saveAs);
    QString RunFranxScenariosDB(QString jsonStr);

    ///
    /// \brief
    /// \return
    ///
    QString GetDefaultMaterialPropsDB();

    QString GetMaterialPropsDB();
    QString SaveMaterialPropsDB( QString jsonStr );
    int     SaveSingleMaterialPropsDB( QString jsonStr );
    QString DeleteMaterialPropsDB( QString jsonStr );

    QString GetFireMaterialPropsDB();
    QString SaveFireMaterialPropsDB( QString jsonStr );
    int     SaveSingleFireMaterialPropsDB( QString jsonStr );
    QString DeleteFireMaterialPropsDB( QString jsonStr );

    QString GetFailureCriteriaPropsDB();
    QString SaveFailureCriteriaPropsDB( QString jsonStr );
    int     SaveSingleFailureCriteriaPropsDB( QString jsonStr );
    QString DeleteFailureCriteriaPropsDB( QString jsonStr );

    QString GetCriteriaPropsDB();
    QString SaveCriteriaPropsDB( QString jsonStr );
    int     SaveSingleCriteriaPropsDB( QString jsonStr );
    QString DeleteCriteriaPropsDB( QString jsonStr );

    QString GetSimInfoPropsDB();
    int     SaveSimInfoPropsDB(QString simInfoProps);

    QString GetAlarmPropsDB();
    QString SaveAlarmPropsDB( QString jsonStr );
    int     SaveSingleAlarmPropsDB( QString jsonStr );
    QString DeleteAlarmPropsDB( QString jsonStr );

    QString SaveSimulationItemsDB( QString jsonStr, int zoneID );
    int     SaveSingleSimulationItemDB(QString jsonStr, int zoneID);
    QString GetSimulationItemsInZoneDB(int zoneID);
    QString GetSimulationItemsInZoneForTypeDB(int zoneID, QString category);

    void    SetCFASTLocDB( QString loc );

    int     RunNewCFASTSimulationDB(QString jsonStr, int zoneID, QString sr , QString si);
    int     RunExistingCFASTSimulationDB( QString jsonStr, int zoneID, QString si );

    QString  GetCableSpecsListDB( int category, int gauge, int conductors, int index, int count );
    int      AddCableSpecDB( QString cableInfoJsonStr );
    QString  GetCableSpecOptionsDB();

public:

    QThread*        m_currentThread;
    FEngineWorker*  m_worker;

private:
    FEngine* S;
};

///
/// \brief initProperties
///

static void initProperties()
{
    //NE_INIT_PROP(FEngine, DBFile);
}

FEngineWorker::FEngineWorker(FEnginePrivate *_P, QString jsonScn,
                             QString jsonSrc, QString simInfoSrc, QString scnName, QString zoneName)
{
    m_workerThread = new QThread;
    m_jsonScn      = jsonScn;
    m_jsonSrc      = jsonSrc;
    m_simInfo      = simInfoSrc;
    m_scnName      = scnName;
    m_zoneName     = zoneName;
    m_WorkerType   = CFAST;
    m_WorkerResult = 0;

    P = _P;
    this->moveToThread( m_workerThread );

    //connect( m_workerThread, SIGNAL( started()  ), this, SLOT( run()) );
    //connect( this, SIGNAL( finished() ), m_workerThread, SLOT( quit()) );
    connect( this, SIGNAL( finished() ), this, SLOT( deleteLater()) );
    connect( m_workerThread, SIGNAL( finished() ), m_workerThread, SLOT( deleteLater()) );
    //connect( this, SIGNAL(processData(json_t*)), P->S, SLOT(processData(json_t*)), Qt::DirectConnection );
}

void FEngineWorker::setWorkerType(FEngineWorker::FWorkerType ftype)
{
    m_WorkerType = ftype;
}

FEngineWorker::FWorkerType FEngineWorker::workerType()
{
    return m_WorkerType;
}

FEngineWorker::~FEngineWorker()
{
    P->S->moveToThread( P->m_currentThread );
    delete m_workerThread;
}

// Start processing data.

void FEngineWorker::run()
{
    int retval = 0;

    if( m_WorkerType == CFAST )
    {
        FZone* czone = qobject_cast< FZone *>( NESCENE.findChildNode( m_zoneName ) );
        QString cfastloc = QStandardPaths::findExecutable( "cfast" );

        if( cfastloc == "" )
        {
            cfastloc = QCoreApplication::applicationDirPath() + "/cfast.exe";
        }


        P->SetCFASTLocDB( cfastloc );
        FScenario *cscn = qobject_cast< FScenario* >( NESCENE.findChildNode( m_scnName ) );

        if( cscn )
        {
            std::cout << "Json Scn " << m_jsonScn.toStdString() << std::endl;
            if( czone )
                retval = P->RunExistingCFASTSimulationDB( m_jsonScn, czone->ID(), m_simInfo );
        }
        else
        {
            if( czone )
                retval = P->RunNewCFASTSimulationDB( m_jsonScn, czone->ID(), m_jsonSrc, m_simInfo );
        }
    }
    else if( m_WorkerType == FRANX )
    {
        QString errStr = P->RunFranxScenariosDB( m_jsonScn );
        if( errStr != "" )
        {
            emit P->S->sendShowWarningMessage( errStr );
        }
    }
    m_WorkerResult = retval;
}

///
/// \brief FEngine::FEngine
/// \param factory
///

FEngine::FEngine(NENodeFactory* factory) : NESpatialEngine(factory)
{
    P = new FEnginePrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);

    P->m_pLibrary.setFileName("FireEngineDLL");
    if( !P->m_pLibrary.load() )
    {
        std::cerr << P->m_pLibrary.errorString().toStdString() << std::endl;
        exit(0);
    }
    std::cerr << "Loaded FireEngine Library...." << std::endl;;

    setOutputs( outputs() | NE::kIOTriangleMesh );
//    SetPivot(vec3f(-0.5, -0.5, -0.5));
//    SetPosition(vec3f(0.0, 0.5, 0.0));

    //hideProperties("NESpatial");

//    QMap< QString, QStringList > propertyMap;

//    // Create properties per class mapping for the initialObject.
//    const QMetaObject *mo =metaObject();
//    do
//    {
//        QStringList properties;
//        for(int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
//            properties << QString::fromLatin1(mo->property(i).name());
//        }
//        propertyMap[mo->className()] = properties;

//    } while (mo = mo->superClass());

//    QStringList properties = propertyMap["NESpatial"];

//    for( int i = 0 ; i < properties.size(); ++i )
//    {
//        hideProperty( properties[i].toStdString().c_str(), true );
//    }

    SetRenderMode( NESpatial::Invisible );
}

///
/// \brief FEngine::~FEngine
///

FEngine::~FEngine()
{
    delete P;
}

///
/// \brief FEngine::init
/// \param initMode
/// \return
///

uint FEngine::init(uint initMode)
{
    int ret = NESpatialGroup::init(initMode);

    QString g_FRANXFilter("All (*.*);;MS Access(*.accdb);;MS Access(*.franx);");

    P->m_FRANXFile.setFilter( g_FRANXFilter );
    P->m_FRANXFile.setDialogType( FilePath::kRefeFileType );
    if (ret != NE::kReturnSuccess)
        return ret;

    P->m_DBFile.setFilter( "All (*.*);;MS Access(*.sqlite);;MS Access(*.f3d);" );
    P->m_DBFile.setDialogType( FilePath::kRefeFileType );
    if (ret != NE::kReturnSuccess)
        return ret;

    FZone *nullZone = NULL;
    FScenario *nullScn = NULL;

    FEngine *curengine = this;

    NENode* lightnode = NESCENE.findChildNode( "DefaultLight" );
    if(NESpatial* ln = qobject_cast<NESpatial*>(lightnode))
    {
        ln->SetRenderMode( NESpatial::Invisible );
    }
    foreach (NECamera *cam, NERENDERMAN.cameras())
    {
        cam->SetRenderMode( NESpatial::Invisible );
    }

    QString workLoc = P->GetTempWorkLocDB();

    workLoc = workLoc.replace('\\', '/');
    updateWorkingFolder( workLoc );
    emit sendUpdateWorkingFolder( workLoc );

    updateUIFolder( workLoc );
    emit sendUpdateUIFolder( workLoc );

    emit activeZone((QObject* &)curengine, (QObject* &)nullZone);
    emit activeScenario((QObject* &)curengine, (QObject* &)nullScn);

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::DBFile
/// \return
///

const FilePath& FEngine::DBFile() const
{
    return P->m_DBFile;
}

///
/// \brief FEngine::SetDBFile
/// \param val
/// \return
///

uint FEngine::SetDBFile(const FilePath& val)
{
    NE_PROP_SET_EVAL(DBFile,P,val);
    if (P->m_DBFile.dialogType() != FilePath::kRefeFileType
            && P->m_DBFile.dialogType() != FilePath::kImpoFileType)
        P->m_DBFile.setDialogType(FilePath::kRefeFileType);

    if (!(QFile(P->m_DBFile.path()).exists()))
        return NE::kReturnFileNotFound;
    P->loadDBFile( P->m_DBFile );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::initializeGUIActions
///

void FEngine::initializeGUIActions()
{
    NESpatial::initializeGUIActions();

#if 0
    QObject* actionImportFRANXFile = NULL;
    QString stActionImportFRANXFile("Import FRANX Model");
    emit registerAction(stActionImportFRANXFile, actionImportFRANXFile);
    connect(actionImportFRANXFile, SIGNAL(triggered()), this, SLOT(importFRANX()));
    addAction(actionImportFRANXFile);
#endif

    QObject* actionLoadDB = NULL;
    QString stactionLoadDB("Load Database");
    emit registerAction(stactionLoadDB, actionLoadDB);
    connect(actionLoadDB, SIGNAL(triggered()), this, SLOT(loadDBFile()));
    //addAction(actionLoadDB);

    QString stActionReloadDataFile("Reload Database");
    QObject* actionReloadDataFile = NULL;
    emit registerAction(stActionReloadDataFile, actionReloadDataFile);
    connect(actionReloadDataFile, SIGNAL(triggered()), this, SLOT(reloadDBFile()));
    //addAction(actionReloadDataFile);

    QString strActionClearDataFile("Clear Database");
    QObject* actionClearDataFile = NULL;
    emit registerAction( strActionClearDataFile, actionClearDataFile );
    connect(actionClearDataFile, SIGNAL(triggered()), this, SLOT(clear()));
    //addAction(actionClearDataFile);
}

///
/// \brief FEngine::nodeRenamed
/// \param node
/// \param name
/// Works only for items and scenarios
///

void FEngine::nodeRenamed( NENode *node, QString name )
{
    // Check if same name already exists.
    if(findChild<QObject *>(name))
    {
        emit sendShowWarningMessage("Name already exists.");
        return;
    }

    if(FZone *zone = qobject_cast<FZone *>(node))
    {
        if (zone->objectName() == P->m_activeZoneName)
            P->m_activeZoneName = name;

        zone->renameInItemNameTree(node->objectName(), name);

        if (FScenario* activeScenario = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName())))
        {
            int index = activeScenario->zones().indexOf(zone->objectName());
            if (index != -1)
                activeScenario->zones().replace(index, name);

            emit relatedZones(activeScenario->zones());
        }

        int index = P->m_ZoneIdNamePairs.indexOf(QPair<int, QString>(zone->ID(), zone->objectName()));
        if (index != -1)
            P->m_ZoneIdNamePairs.replace(index, QPair<int, QString>(zone->ID(), name));

        //
        // Now the node rename and Database actions
        //
        zone->setOriginalName(name);
        zone->setObjectName(name);
        zone->setZoneName(name);

        QString json = P->createJSONFromSceneZone(zone);
        P->SaveModelItemDB(json, zone->ID());

        QStringList zoneNameList = P->zoneNames();
        emit allZones(zoneNameList);
    }
    else if( FZoneItem* item = qobject_cast< FZoneItem *>( node ) )
    {
        if( FZone* activeZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) ) )
            activeZone->renameInItemNameTree(node->objectName(), name);
        if( FScenario* activeScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) ) )
            activeScn->renameInItemNameTree(node->objectName(), name);

        //
        // Now the node rename and Database actions
        //
        item->setOriginalName(name);
        item->setObjectName( name );
        item->setZoneItemName(name);

        QString retJson = P->createJSONFromSceneItem( item );
        if (NEZone* zoneParent = qobject_cast< NEZone *>( node->parent() ))
            P->SaveModelItemDB( retJson, zoneParent->ID() );
    }
    else if(FZoneCompartment *comp = qobject_cast<FZoneCompartment *>(node))
    {
        if( FZone* activeZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) ) )
            activeZone->renameInItemNameTree(comp->objectName(), name);

        comp->setObjectName(name);
        comp->setOriginalName(name);
        comp->setCompartmentName(name);

        QString json = P->createJSONFromCompartment(comp);
        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
            P->SaveModelItemDB(json, activeZone->ID());
    }
    else if (FVent* vent = qobject_cast<FVent*>(node))
    {
        if( FZone* activeZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) ) )
            activeZone->renameInItemNameTree(vent->objectName(), name);
        if( FScenario* activeScenario = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) ) )
            activeScenario->renameInItemNameTree(vent->objectName(), name);

        vent->setObjectName(name);
        vent->setOriginalName(name);
        vent->setName(name);

        QString json = P->createJSONFromVent(vent);
        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
            P->SaveSingleSimulationItemDB(json, activeZone->ID());
    }
    else if( FScenario* scn = qobject_cast< FScenario *>( node ) )
    {
        if (scn->objectName() == P->m_activeScenarioName)
            P->m_activeScenarioName = name;

        scn->renameInItemNameTree(node->objectName(), name);

        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
        {
            // Hack: the scenario-name list of the active zone somehow got cleared at this point
            activeZone->scenarios() = P->GetScenarioListForZone(activeZone->ID());

            int index = activeZone->scenarios().indexOf(scn->objectName());
            if (index != -1)
                activeZone->scenarios().replace(index, name);

            emit relatedScenarios(activeZone->scenarios());
        }

        int index = P->m_ScnIdNamePairs.indexOf(QPair<int, QString>(scn->ID(), scn->objectName()));
        if (index != -1)
            P->m_ScnIdNamePairs.replace(index, QPair<int, QString>(scn->ID(), name));

        scn->setOriginalName(name);
        scn->setObjectName(name);
        scn->setScenarioName(name);

        QString json = P->createJSONFromSceneScenario(scn);
        P->SaveScenarioAsDB(json, name.toUtf8().data());

        QStringList scnNameList = P->scnNames();
        emit allScenarios(scnNameList);
    }
}

///
/// \brief FEngine::saveZone
/// \param zonename
/// \param filename
///

void FEngine::saveZone(QString zonename, QString filename)
{
    FZone* cZone = qobject_cast< FZone *>( NESCENE.findChildNode( zonename ) );

    //
    // Create a JSON String from the properties of this zone
    //
    QString zoneJson = P->createJSONFromSceneZone( cZone );

    qDebug().noquote() << "Save Zone " << zoneJson;
    int tid = P->SaveModelItemDB( zoneJson, cZone->ID() );

    return;
}

///
/// \brief FEngine::saveScenario
/// \param scenarioname
/// \param filename
///

void FEngine::saveScenario( QString scenarioname, QString name )
{
    QString jsonStr;

    FScenario* scn = qobject_cast< FScenario *>( NESCENE.findChildNode( scenarioname ) );
    if( scn )
    {
        jsonStr = P->createJSONFromSceneScenario( scn );
    }

    P->SaveScenarioAsDB( jsonStr, name.toUtf8().data() );
}

///
/// \brief FEngine::compute
/// \return
///

uint FEngine::compute()
{
    QString fileName;

    emit NEBASE.showSceneLoadingWidget();
    emit NEBASE.setCurrentSubOperation("Computing FRANX...");
    /*
    FScenario* cactiveScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) );
    if( cactiveScn )
    {
        P->RunFranxScenarioDB( cactiveScn->ID(), fileName, "" );
    }
    */

    QJsonArray sources;
    QJsonObject jsonObject;

    for( int i = 0; i < P->m_FRANXScenarios.size(); ++i )
    {
        if( P->m_FRANXScenarios[i].m_Selected == true )
            sources.append( QJsonValue( P->m_FRANXScenarios[i].m_ID ) );
    }
    jsonObject["IDs"] = sources;

    QJsonDocument sdoc( jsonObject );

    QString jsonStr( sdoc.toJson() );

    P->m_worker = new FEngineWorker(P, jsonStr, "", "", "", "" );
    P->m_worker->setWorkerType( FEngineWorker::FRANX );

    connect( P->m_worker, SIGNAL(finished()), this, SLOT(finishedCompute()) );
    P->m_worker->start();

    return NE::kReturnSuccess;
}


uint FEngine::finishedCompute()
{
    NEBASE.hideSceneLoadingWidget();
    return NE::kReturnSuccess;
}

///
/// \brief FEngine::computeLocation
/// \return
///

void FEngine::computeOptions()
{
//    QString basefranx = P->GetBaseFRANXModelLocDB();

//    if( NEBASE.getGUICallInterface() )
//    {
//        FilePath filePath = FilePath("Test.accdb");
//        QStringList fileNames = NEBASE.getGUICallInterface()->fileImportBrowser(filePath, "Fire Dynamics", "FRI3D", "lastOpenImportFRANXPath");

//        if (fileNames.size() > 0)
//            basefranx = fileNames.front();
//    }

    return;
}

///
/// \brief FEngine::addFire
/// \param zoneItem
///

void FEngine::addFire(NENode *zoneItem)
{
    if( FFire* fireItem = qobject_cast< FFire *>( zoneItem ) )
    {
        FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );

        P->updateItemWithCompartmentInfo(fireItem);

        fireItem->SetItemType(FZoneItem::Sources);
        fireItem->SetType( "foPlantItem" );
        fireItem->setPlantItemType( "piSource" );
        fireItem->setOriginalName(zoneItem->objectName());

        //
        // Add to the name to scene database - the node is already present in the scene database
        //
        cactiveZone->addToItemNameTree(fireItem->objectName(), cactiveZone->objectName());

        // Create a JSON String from the properties of this zone item - fire
        //
        QString json = P->createJSONFromSceneItem( fireItem );
        //int tid = P->SaveModelItemDB( json, cactiveZone->ID() );
        int tid = P->NewModelItemDB( json, cactiveZone->ID(), cactiveZone->ID() );

        fireItem->SetID( tid );
        fireItem->SetConnectorID( tid );

        //
        // Add to the local database
        //
        std::shared_ptr< FFire > newFire = std::shared_ptr< FFire >( new FFire );
        newFire->SetID( fireItem->ID() );
        newFire->SetConnectorID( fireItem->ConnectorID() );
        newFire->SetType( fireItem->Type() );
        newFire->SetItemType( fireItem->ItemType() );
        newFire->setPlantItemType( fireItem->PlantItemType() );
        newFire->SetDesc( fireItem->Desc() );
        newFire->SetIssues( fireItem->Issues() );
        newFire->setOriginalName(fireItem->originalName());
        newFire->setZoneItemName( fireItem->objectName() );
        newFire->setModel3DInfo( fireItem->model3DInfo() );
        newFire->SetCompartmentID( fireItem->CompartmentID() );

        P->m_ZoneItems[cactiveZone->ID()].insert( newFire->ID(), newFire );

//        emit NEBASE.updateSceneInspector();
    }
}

void FEngine::convertToSource(NENode *prevNode, NENode *newNode)
{
    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));
    FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName()));

    if (activeZone == nullptr || activeScn == nullptr)
        return;

    if (FFire* newFire = qobject_cast<FFire*>(newNode))
    {
        prevNode->setObjectName(prevNode->objectName()+"_deprecated");
        QString prevObjectName = prevNode->objectName().remove( "_deprecated" );

        // Check whether the previous node was added to the scenario
        bool isAddedToScn;
        if( qobject_cast<NEZone*>(prevNode->parent()) )
            isAddedToScn = activeScn->itemNameTreeContains( prevNode->objectName(), activeScn->objectName() );
        else
            isAddedToScn = activeScn->itemNameTreeContains( prevNode->objectName(), prevNode->parent()->objectName() );

        // Copy properties and values from the previous node to the new vent
        QJsonArray propList = getJSONFromProperties( prevNode );
        QString json = QString( QJsonDocument(propList).toJson() );
        setPropertiesFromJSON( newFire, json );

        // Copy values from the previous node
        if (FZoneItem* prevZoneItem = qobject_cast<FZoneItem*>(prevNode))
        {
            newFire->setObjectName(prevZoneItem->objectName());

            // Reset the ID of new vent
            newFire->SetID(0);

            json = P->createJSONFromSceneItem(prevZoneItem);

            if (isAddedToScn)
            {
                // Remove the previous node from the scenario in the database
                uint parentID = 0;
                if (qobject_cast<NEZone*>(newFire->parent()))
                    parentID = activeZone->ID();
                else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newFire->parent()))
                    parentID = parentItem->ID();
                P->RemoveItemFromScenarioDB(json, activeScn->ID(), parentID);

                // Remove the previous node from the scenario tree
                activeScn->removeAllFromItemNameTree(prevNode->objectName());
                P->m_ScenarioItems[activeScn->ID()].remove(prevZoneItem->ID());
            }

            // Delete the previous node from the database
            P->DeleteItemDB(json);

            // Remove the previous node from the zone tree
            activeZone->removeAllFromItemNameTree(prevNode->objectName());
            P->m_ZoneItems[activeZone->ID()].remove(prevZoneItem->ID());
        }

        // Save the new  to the database
        json = P->createJSONFromSceneItem(newFire);
        //int id = P->SaveModelItemDB(json, activeZone->ID());
        int id = P->NewModelItemDB(json, activeZone->ID(), activeZone->ID() );
        newFire->SetID(id);

        std::shared_ptr<FFire> fire = std::shared_ptr<FFire>(new FFire);
        fire->copyValues( newFire );

        // Add the new fire to the zone tree
        activeZone->addToItemNameTree(newFire->objectName(), activeZone->objectName());
        P->m_ZoneItems[activeZone->ID()].insert(newFire->ID(), fire);

        if (isAddedToScn)
        {
            // Recreate the json so that is has the right ID
            json = P->createJSONFromSceneItem(newFire);

            // Add the new vent to the scenario in the database
            uint parentID = 0;
            if (qobject_cast<NEZone*>(newFire->parent()))
                parentID = activeZone->ID();
            else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newFire->parent()))
                parentID = parentItem->ID();
            P->AddItemToScenarioDB(json, activeScn->ID(), parentID);

            // Add the new vent to the scenario tree
            if (qobject_cast<NEZone*>(newFire->parent()))
                activeScn->addToItemNameTree(newFire->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(newFire->objectName(), newFire->parent()->objectName());
            P->m_ScenarioItems[activeScn->ID()].insert(newFire->ID(), fire);
        }
    }
}



void FEngine::convertToItem(NENode *prevNode, NENode *newNode)
{
    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));
    FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName()));

    if (activeZone == nullptr || activeScn == nullptr)
        return;

    if (FZoneItem* newItem = qobject_cast<FZoneItem*>(newNode))
    {
        prevNode->setObjectName(prevNode->objectName()+"_deprecated");
        QString prevObjectName = prevNode->objectName().remove( "_deprecated" );

        // Check whether the previous node was added to the scenario
        bool isAddedToScn;
        if( qobject_cast<NEZone*>(prevNode->parent()) )
            isAddedToScn = activeScn->itemNameTreeContains( prevObjectName, activeScn->objectName() );
        else
            isAddedToScn = activeScn->itemNameTreeContains( prevObjectName, prevNode->parent()->objectName() );

        // Copy properties and values from the previous node to the new vent
        QJsonArray propList = getJSONFromProperties( prevNode );
        QString json = QString(QJsonDocument( propList).toJson()) ;

        // Copy values from the previous node to the new
        if (FZoneItem* prevFire = qobject_cast<FZoneItem*>(prevNode))
        {
            newItem->setObjectName(prevObjectName);
            newItem->copyValues(prevFire);
        }
        //
        // We need two set properties due to some bug which is yet to be figured out in which there's an offset
        // if done just once
        //
        setPropertiesFromJSON( newItem, json );
        setPropertiesFromJSON( newItem, json );

        // Reset the ID of new vent
        newItem->SetID(0);

        // Delete the previous node from the database and trees
        if (FZoneItem* prevNodeItem = qobject_cast<FZoneItem*>(prevNode))
        {
            json = P->createJSONFromSceneItem(prevNodeItem);

            if (isAddedToScn)
            {
                // Remove the previous node from the scenario in the database
                uint parentID = 0;
                if (qobject_cast<NEZone*>(newItem->parent()))
                    parentID = activeZone->ID();
                else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newItem->parent()))
                    parentID = parentItem->ID();
                P->RemoveItemFromScenarioDB(json, activeScn->ID(), parentID);

                // Remove the previous node from the scenario tree
                activeScn->removeAllFromItemNameTree(prevObjectName);
                P->m_ScenarioItems[activeScn->ID()].remove(prevNodeItem->ID());
            }

            // Delete the previous node from the database
            P->DeleteItemDB(json);

            // Remove the previous node from the zone tree
            activeZone->removeAllFromItemNameTree(prevObjectName);
            P->m_ZoneItems[activeZone->ID()].remove(prevNodeItem->ID());
        }

        if (FZoneItem* prevFire = qobject_cast<FZoneItem*>(prevNode))
        {
            if( prevFire->Type() == "foPlantItem" )
            {
                newItem->SetItemType( FZoneItem::Component );
                newItem->SetType( "foComponent" );
                newItem->setPlantItemType( "piEquipment" );
            }
            else
            {
                newItem->SetType( "foPlantItem" );
                newItem->SetItemType( FZoneItem::Physical );
                newItem->setPlantItemType( P->m_plantItemLib[ FZoneItem::Physical ] );
            }
        }

        newItem->SetColor( QColor( 0, 0, 255 ) );

        // Save the new vent to the database
        json = P->createJSONFromSceneItem( newItem );
        //int id = P->SaveModelItemDB( json, activeZone->ID() );
        int id = P->NewModelItemDB(json, activeZone->ID(), activeZone->ID() );

        newItem->SetID( id );
        newItem->SetConnectorID( id );

        std::shared_ptr<FZoneItem> zoneitem = std::shared_ptr<FZoneItem>(new FZoneItem);
        zoneitem->copyValues( newItem );
        zoneitem->setPlantItemType( P->m_plantItemLib[ newItem->ItemType() ] );

        // Add the new item to the zone tree
        activeZone->addToItemNameTree( newItem->objectName(), activeZone->objectName() );
        P->m_ZoneItems[activeZone->ID()].insert(newItem->ID(), zoneitem);

        if (isAddedToScn)
        {
            // Recreate the json so that is has the right ID
            json = P->createJSONFromSceneItem( newItem );

            // Add the new vent to the scenario in the database
            uint parentID = 0;
            if( qobject_cast<NEZone*>(newItem->parent()) )
                parentID = activeZone->ID();
            else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newItem->parent()))
                parentID = parentItem->ID();
            P->AddItemToScenarioDB( json, activeScn->ID(), parentID );

            // Add the new vent to the scenario tree
            if (qobject_cast<NEZone*>(newItem->parent()))
                activeScn->addToItemNameTree(newItem->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(newItem->objectName(), newItem->parent()->objectName());
            P->m_ScenarioItems[activeScn->ID()].insert(newItem->ID(), zoneitem);
        }
    }
}

///
/// \brief FEngine::addVent
/// \param zoneItem
///
void FEngine::addVent(NENode *zoneItem)
{
    FVent* vent = qobject_cast< FVent *>( zoneItem );
    FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );

    if (vent == nullptr || cactiveZone == nullptr)
        return;

    vent->SetType( "foSimulationItem" );

    vent->setOriginalName(zoneItem->objectName());
    for (QPair<int, QString>& compIdNamePair : P->m_ZoneIdNamePairs)
        vent->addCompartment(compIdNamePair.second);
    vent->setSecondCompartmentFromName(activeZoneName(), activeZoneName());

    QString jsonStr = P->createJSONFromVent(vent);

    int tid = P->SaveSingleSimulationItemDB( jsonStr, cactiveZone->ID() );
    vent->SetID( tid );

    std::shared_ptr< FVent > newVent = std::shared_ptr< FVent >( new FVent );
    newVent->SetID( vent->ID() );
    newVent->SetType( vent->Type() );
    newVent->SetDesc( vent->Desc() );
    newVent->SetIssues( vent->Issues() );
    newVent->SetFirstCompartment( vent->FirstCompartment() );
    newVent->SetSecondCompartment( vent->SecondCompartment() );
    newVent->SetCriterion( vent->Criterion() );
    newVent->setOriginalName(vent->originalName());
    newVent->setName( vent->objectName() );
    newVent->setModel3DInfo( vent->model3DInfo() );
    newVent->setFirstCompartmentId( vent->firstCompartmentId() );
    newVent->setSecondCompartmentId( vent->secondCompartmentId() );

    P->m_ZoneVents[cactiveZone->ID()].insert( newVent->ID(), newVent );
    cactiveZone->addToItemNameTree(vent->objectName(), cactiveZone->objectName());

//    emit NEBASE.updateSceneInspector();
}



void FEngine::turnIntoVent(NENode* prevNode, NENode* newNode)
{
    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));
    FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName()));

    if (activeZone == nullptr || activeScn == nullptr)
        return;

    if (FVent* newVent = qobject_cast<FVent*>(newNode))
    {
        QString name = prevNode->objectName();
        prevNode->setObjectName(name+"_deprecated");

        // Check whether the previous node was added to the scenario
        bool isAddedToScn;
        if (qobject_cast<NEZone*>(prevNode->parent()))
            isAddedToScn = activeScn->itemNameTreeContains(name, activeScn->objectName());
        else
            isAddedToScn = activeScn->itemNameTreeContains(name, prevNode->parent()->objectName());

        // Copy properties and values from the previous node to the new vent
        QJsonArray propList = getJSONFromProperties(prevNode);
        QString json = QString(QJsonDocument(propList).toJson());
        setPropertiesFromJSON(newVent, json);

        // Copy values from the previous node to the new vent
        if (FMechVent* prevMechVent = qobject_cast<FMechVent*>(prevNode))
        {
            newVent->setObjectName(name);
            newVent->copyValues(prevMechVent);
        }

        // Reset the ID of new vent
        newVent->SetID(0);

        // Delete the previous node from the database and trees
        if (FMechVent* prevMechVent = qobject_cast<FMechVent*>(prevNode))
        {
            json = P->createJSONFromVent(prevMechVent);

            if (isAddedToScn)
            {
                // Remove the previous node from the scenario in the database
                uint parentID = 0;
                if (qobject_cast<NEZone*>(newVent->parent()))
                    parentID = activeZone->ID();
                else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newVent->parent()))
                    parentID = parentItem->ID();
                P->RemoveItemFromScenarioDB(json, activeScn->ID(), parentID);

                // Remove the previous node from the scenario tree
                activeScn->removeAllFromItemNameTree(name);
                P->m_ScenarioVents[activeScn->ID()].remove(prevMechVent->ID());
            }

            // Delete the previous node from the database
            P->DeleteItemDB(json);

            // Remove the previous node from the zone tree
            activeZone->removeAllFromItemNameTree(name);
            P->m_ZoneVents[activeZone->ID()].remove(prevMechVent->ID());
        }

        // Save the new vent to the database
        json = P->createJSONFromVent(newVent);
        int id = P->SaveSingleSimulationItemDB(json, activeZone->ID());
        newVent->SetID(id);

        std::shared_ptr<FVent> vent = std::shared_ptr<FVent>(new FVent);
        vent->SetID(newVent->ID());
        vent->SetType(newVent->Type());
        vent->SetDesc(newVent->Desc());
        vent->SetIssues(newVent->Issues());
        vent->SetFirstCompartment(newVent->FirstCompartment());
        vent->SetSecondCompartment(newVent->SecondCompartment());
        vent->SetCriterion(newVent->Criterion());
        vent->setOriginalName(newVent->originalName());
        vent->setName(newVent->objectName());
        vent->setModel3DInfo(newVent->model3DInfo() );
        vent->setFirstCompartmentId(newVent->firstCompartmentId());
        vent->setSecondCompartmentId(newVent->secondCompartmentId());

        // Add the new vent to the zone tree
        activeZone->addToItemNameTree(newVent->objectName(), activeZone->objectName());
        P->m_ZoneVents[activeZone->ID()].insert(newVent->ID(), vent);

        if (isAddedToScn)
        {
            // Recreate the json so that is has the right ID
            json = P->createJSONFromVent(newVent);

            // Add the new vent to the scenario in the database
            uint parentID = 0;
            if (qobject_cast<NEZone*>(newVent->parent()))
                parentID = activeZone->ID();
            else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newVent->parent()))
                parentID = parentItem->ID();
            P->AddItemToScenarioDB(json, activeScn->ID(), parentID);

            // Add the new vent to the scenario tree
            if (qobject_cast<NEZone*>(newVent->parent()))
                activeScn->addToItemNameTree(newVent->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(newVent->objectName(), newVent->parent()->objectName());
            P->m_ScenarioVents[activeScn->ID()].insert(newVent->ID(), vent);
        }
    }
}

void FEngine::turnIntoMechVent(NENode* prevNode, NENode* newNode)
{
    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));
    FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName()));

    if (activeZone == nullptr || activeScn == nullptr)
        return;

    if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newNode))
    {
        QString name = prevNode->objectName();
        prevNode->setObjectName(name+"_deprecated");

        // Check whether the previous node was added to the scenario
        bool isAddedToScn;
        if (qobject_cast<NEZone*>(prevNode->parent()))
            isAddedToScn = activeScn->itemNameTreeContains(name, activeScn->objectName());
        else
            isAddedToScn = activeScn->itemNameTreeContains(name, prevNode->parent()->objectName());

        // Copy properties and values from the previous node to the new vent
        QJsonArray propList = getJSONFromProperties(prevNode);
        QString json = QString(QJsonDocument(propList).toJson());
        setPropertiesFromJSON(newMechVent, json);

        // Copy values from the previous node to the new vent
        if (FVent* prevVent = qobject_cast<FVent*>(prevNode))
        {
            newMechVent->setObjectName(name);
            newMechVent->copyValues(prevVent);
        }

        // Reset the ID of new vent
        newMechVent->SetID(0);

        // Delete the previous node from the database and trees
        if (FVent* prevVent = qobject_cast<FVent*>(prevNode))
        {
            json = P->createJSONFromVent(prevVent);

            if (isAddedToScn)
            {
                // Remove the previous node from the scenario in the database
                uint parentID = 0;
                if (qobject_cast<NEZone*>(newMechVent->parent()))
                    parentID = activeZone->ID();
                else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newMechVent->parent()))
                    parentID = parentItem->ID();
                P->RemoveItemFromScenarioDB(json, activeScn->ID(), parentID);

                // Remove the previous node from the scenario tree
                activeScn->removeAllFromItemNameTree(name);
                P->m_ScenarioVents[activeScn->ID()].remove(prevVent->ID());
            }

            // Delete the previous node from the database
            P->DeleteItemDB(json);

            // Remove the previous node from the zone tree
            activeZone->removeAllFromItemNameTree(name);
            P->m_ZoneVents[activeZone->ID()].remove(prevVent->ID());
        }

        // Save the new vent to the database
        json = P->createJSONFromVent(newMechVent);
        int id = P->SaveSingleSimulationItemDB(json, activeZone->ID());
        newMechVent->SetID(id);

        std::shared_ptr<FMechVent> mechVent = std::shared_ptr<FMechVent>(new FMechVent);
        mechVent->SetID(newMechVent->ID());
        mechVent->SetType(newMechVent->Type());
        mechVent->SetDesc(newMechVent->Desc());
        mechVent->SetIssues(newMechVent->Issues());
        mechVent->SetFirstCompartment(newMechVent->FirstCompartment());
        mechVent->SetSecondCompartment(newMechVent->SecondCompartment());
        mechVent->SetCriterion(newMechVent->Criterion());
        mechVent->setOriginalName(newMechVent->originalName());
        mechVent->setName(newMechVent->objectName());
        mechVent->setModel3DInfo(newMechVent->model3DInfo() );
        mechVent->setFirstCompartmentId(newMechVent->firstCompartmentId());
        mechVent->setSecondCompartmentId(newMechVent->secondCompartmentId());
        mechVent->SetFlowRate(newMechVent->FlowRate());
        mechVent->SetBeginDropOff(newMechVent->BeginDropOff());
        mechVent->SetZeroAt(newMechVent->ZeroAt());
        mechVent->SetFilterEfficiency(newMechVent->FilterEfficiency());
        mechVent->SetFilterStart(newMechVent->FilterStart());

        // Add the new vent to the zone tree
        activeZone->addToItemNameTree(newMechVent->objectName(), activeZone->objectName());
        P->m_ZoneVents[activeZone->ID()].insert(newMechVent->ID(), mechVent);

        if (isAddedToScn)
        {
            // Recreate the json so that is has the right ID
            json = P->createJSONFromVent(newMechVent);

            // Add the new vent to the scenario in the database
            uint parentID = 0;
            if (qobject_cast<NEZone*>(newMechVent->parent()))
                parentID = activeZone->ID();
            else if (NEZoneItem* parentItem = qobject_cast<NEZoneItem*>(newMechVent->parent()))
                parentID = parentItem->ID();
            P->AddItemToScenarioDB(json, activeScn->ID(), parentID);

            // Add the new vent to the scenario tree
            if (qobject_cast<NEZone*>(newMechVent->parent()))
                activeScn->addToItemNameTree(newMechVent->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(newMechVent->objectName(), newMechVent->parent()->objectName());
            P->m_ScenarioVents[activeScn->ID()].insert(newMechVent->ID(), mechVent);
        }
    }
}

///
/// \brief FEngine::addFireDetection
/// \param scnItem
///
void FEngine::addFireDetection(NENode *scnItem)
{
    if( FFireDetect* fireDetectItem = qobject_cast< FFireDetect *>( scnItem ) )
    {
        FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );
        cactiveZone->addToItemNameTree(fireDetectItem->objectName(), cactiveZone->objectName());

        fireDetectItem->setOriginalName(fireDetectItem->objectName());

        QString fireDetectItemStr = P->createJSONFromSceneItem( fireDetectItem );

        //
        // Create a JSON String from the properties of this zone item - fire
        //
        QString zoneItemJson = P->createJSONFromSceneItem( fireDetectItem );
        //
        // Create the additional JSon entries specific to fire detection node
        //


        //
        // Then set the model info string
        //
        fireDetectItem->setModel3DInfo( zoneItemJson );

        //
        // Add to the local database
        //
        std::shared_ptr< FFireDetect > newFire = std::shared_ptr< FFireDetect >( new FFireDetect );
        newFire->SetID( fireDetectItem->ID() );
        newFire->SetConnectorID( fireDetectItem->ConnectorID() );
        newFire->SetType( fireDetectItem->Type() );
        newFire->setPlantItemType( fireDetectItem->PlantItemType() );
        newFire->SetItemType( FZoneItem::Sources );
        newFire->SetDesc( fireDetectItem->Desc() );
        newFire->SetIssues( fireDetectItem->Issues() );
        newFire->setOriginalName(fireDetectItem->originalName());
        newFire->setZoneItemName( fireDetectItem->objectName() );
        newFire->setModel3DInfo( fireDetectItem->model3DInfo() );

        auto& zoneItems = P->m_ZoneItems[ cactiveZone->ID() ];
        zoneItems[ newFire->ID() ] = std::shared_ptr< FZoneItem >( newFire );

//        emit NEBASE.updateSceneInspector();
    }
}

void FEngine::loadDefaultSurfaceMaterials()
{
    // Reset and increment the indexes of non-default surface materials
    int i = 1; // index 0 is for the null material
    for (NESurfaceMaterial& sm : P->m_SurfaceMaterials)
    {
        sm.setIndex(i+P->m_DefaultSurfaceMaterials.size());
        i++;
    }

    // Preprend the default surface materials
    QList<NESurfaceMaterial> allSurfaceMaterials = P->m_SurfaceMaterials;
    for (auto itr = P->m_DefaultSurfaceMaterials.rbegin(); itr != P->m_DefaultSurfaceMaterials.rend(); itr++)
        allSurfaceMaterials.prepend(*itr);

    emit sendUpdateSurfaceMaterials(allSurfaceMaterials);
}

///
/// \brief FEngine::getDefaultSurfaceMaterials
/// \return
///

QList<NESurfaceMaterial> FEngine::getDefaultSurfaceMaterials()
{
    QList<NESurfaceMaterial> list;

    QString matList = P->GetDefaultMaterialPropsDB();

    QJsonDocument doc = QJsonDocument::fromJson( matList.toUtf8() );
    QJsonArray surfaceMats = doc.array();
    for( int i = 0; i < surfaceMats.size(); ++i )
    {
        QJsonObject jObj = surfaceMats[i].toObject();
        QJsonDocument doc(jObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);
        NESurfaceMaterial sm = P->createSurfaceMaterialFromJSON( strJson );
        sm.setIndex(i+1); // index 0 is for the null material
        list.append( sm );
    }

    return list;
}

///
/// \brief FEngine::getSurfaceMaterials
/// \return
///

QList<NESurfaceMaterial> FEngine::getSurfaceMaterials()
{
    QList<NESurfaceMaterial> list;

    QString matList = P->GetMaterialPropsDB();

    QJsonDocument doc = QJsonDocument::fromJson( matList.toUtf8() );
    QJsonArray surfaceMats = doc.array();
    for( int i = 0; i < surfaceMats.size(); ++i )
    {
        QJsonObject jObj = surfaceMats[i].toObject();
        QJsonDocument doc(jObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);
        NESurfaceMaterial sm = P->createSurfaceMaterialFromJSON( strJson );
        sm.setIndex(i+1); // index 0 is for the null material
        list.append( sm );
    }

    return list;
}

///
/// \brief FEngine::addSurfaceMaterial
/// \param sm
///

void FEngine::addSurfaceMaterial(NESurfaceMaterial sm)
{
    QString jsonStr = P->createJSONFromSurfaceMaterial( sm );

    int materialID = P->SaveSingleMaterialPropsDB( jsonStr.toUtf8() );
    sm.setUniqueID(materialID);

    P->m_SurfaceMaterials.push_back( sm );

    emit sendUpdateSurfaceMaterial( sm );
}

///
/// \brief FEngine::removeSurfaceMaterial
/// \param sm
///

void FEngine::removeSurfaceMaterial(NESurfaceMaterial sm)
{
    QString jsonStr = P->createJSONFromSurfaceMaterial( sm );

    P->m_SurfaceMaterials.removeAll( sm );
    //
    // Add Array
    //
    jsonStr.insert( 0, "[");
    jsonStr.append( ']' );

    P->DeleteMaterialPropsDB( jsonStr.toUtf8() );
}

///
/// \brief FEngine::assignSurfaceMaterial
/// \param zoneitem
/// \param sm
/// \param matKind
///

void FEngine::assignSurfaceMaterial(NENode *node, NESurfaceMaterial sm, QString matKind)
{
    if( FZoneItem *item = qobject_cast< FZoneItem *>( node ) )
    {
        P->saveModelItem( item->objectName() );
    }
    else if( FZone *zone = qobject_cast< FZone*>( node ) )
    {
        QString json = P->createJSONFromSceneZone( zone );
        P->SaveModelItemDB( json, zone->ID() );
    }
}

///
/// \brief FEngine::changeSurfaceMaterial
/// \param sm
///

void FEngine::changeSurfaceMaterial(NESurfaceMaterial sm)
{
    QString jsonStr = P->createJSONFromSurfaceMaterial( sm );

    int materialID = P->SaveSingleMaterialPropsDB( jsonStr.toUtf8() );
    sm.setUniqueID(materialID);

    for (int i = 0; i != P->m_SurfaceMaterials.size(); i++)
    {
        if (P->m_SurfaceMaterials[i].uniqueID() == sm.uniqueID())
            P->m_SurfaceMaterials[i] = sm;
    }

    if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
    {
        if (activeZone->CeilingMaterial().uniqueID() == sm.uniqueID())
            activeZone->SetCeilingMaterial(sm);
        if (activeZone->FloorMaterial().uniqueID() == sm.uniqueID())
            activeZone->SetFloorMaterial(sm);
        if (activeZone->WallMaterial().uniqueID() == sm.uniqueID())
            activeZone->SetWallMaterial(sm);

        activeZone->updateThicknesses();
    }
}

///
/// \brief FEngine::getFireMaterials
/// \return
///

QList<NEFireMaterial> FEngine::getFireMaterials()
{
    QList<NEFireMaterial> fmats;

    QString matList = P->GetFireMaterialPropsDB();

    QJsonDocument doc = QJsonDocument::fromJson( matList.toUtf8() );
    QJsonArray fireMats = doc.array();
    for( int i = 0; i < fireMats.size(); ++i )
    {
        QJsonObject jObj = fireMats[i].toObject();
        QJsonDocument doc(jObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);
        NEFireMaterial fm = P->createFireMaterialFromJSON( strJson );
        fm.setIndex(i+1); // index 0 is for the null material
        fmats.append( fm );
    }

    return fmats;
}

///
/// \brief FEngine::addFireMaterial
/// \param fm
///

void FEngine::addFireMaterial(NEFireMaterial fm)
{
    QString dataToString = P->createJSONFromFireMaterial( fm );

    int id = P->SaveSingleFireMaterialPropsDB( dataToString.toUtf8() );
    fm.setUniqueID(id);

    P->m_FireMaterials.push_back( fm );

    emit sendUpdateFireMaterial( fm );
}

///
/// \brief FEngine::removeFireMaterial
/// \param fm
///

void FEngine::removeFireMaterial(NEFireMaterial fm)
{
    QString dataToString = P->createJSONFromFireMaterial( fm );
    QJsonDocument doc = QJsonDocument::fromJson( dataToString.toUtf8() );
    QJsonObject obj = doc.object();

    QJsonArray aobj;
    aobj.append( QJsonValue( obj ) );
    QJsonDocument jsonDoc( aobj );

    QString jsonStr( jsonDoc.toJson() );

    P->m_FireMaterials.removeAll( fm );

    P->DeleteFireMaterialPropsDB( jsonStr.toUtf8() );
}

///
/// \brief FEngine::assignFireMaterial
/// \param node
/// \param fm
///

void FEngine::assignFireMaterial(NENode *node, NEFireMaterial fm)
{
    FFire *fire = qobject_cast< FFire *>( node );
    if( fire )
    {
        //fire->SetFireMaterial(fm);
        P->saveModelItem(fire->objectName());
    }
}

///
/// \brief FEngine::getFailureCriteria
/// \return
///

QList<NEFailureCriteria> FEngine::getFailureCriteria()
{
    QList<NEFailureCriteria> list;

    QString failList = P->GetFailureCriteriaPropsDB();

    QJsonDocument doc = QJsonDocument::fromJson( failList.toUtf8() );
    QJsonArray criteria = doc.array();
    for( int i = 0; i < criteria.size(); ++i )
    {
        QJsonObject jObj = criteria[i].toObject();
        QJsonDocument doc(jObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);
        NEFailureCriteria fail = P->createFailureCriteriaFromJSON( strJson );
        fail.setIndex(i+1); // index 0 is for the null failure criterion
        list.append( fail );
    }

    return list;
}

void FEngine::addFailureCriteria(NEFailureCriteria fail)
{
    QString jsonString = P->createJSONFromFailureCriteria( fail );

    int id = P->SaveSingleFailureCriteriaPropsDB( jsonString.toUtf8() );
    fail.setID(id);
    P->m_FailCriteriaList.push_back( fail );

    emit sendUpdateFailureCriteria( fail );
}

void FEngine::removeFailureCriteria(NEFailureCriteria fail)
{
    QString jsonString = P->createJSONFromFailureCriteria( fail );

    P->m_FailCriteriaList.removeAll( fail );

    //
    // Add Array
    //
    jsonString.insert( 0, "[");
    jsonString.append( ']' );

    P->DeleteFailureCriteriaPropsDB( jsonString.toUtf8() );
}

void FEngine::assignFailureCriteria(NENode *node, NEFailureCriteria fail)
{
    FZoneItem *zitem = qobject_cast< FZoneItem *>( node );
    if( zitem )
    {
        P->saveModelItem( zitem->objectName() );
    }
}

QList<NECableProperty> FEngine::getCableProperty()
{
    QList<NECableProperty> list;

    QString cablelist = P->GetCableSpecsListDB(-1, -1, -1, 0, 0);

    QJsonDocument doc = QJsonDocument::fromJson( cablelist.toUtf8() );
    QJsonArray cables = doc.array();
    for( int i = 0; i < cables.size(); ++i )
    {
        QJsonObject jObj = cables[i].toObject();
        QJsonDocument doc(jObj);
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);
        NECableProperty cp = P->createCablePropertiesFromJSON( strJson );
        cp.setIndex(i+1); // index 0 is for the null cable property
        list.append( cp );
    }

    return list;
}

std::vector<QList<QString>> FEngine::getCableOptions()
{
    QString jsonStr = P->GetCableSpecOptionsDB();
    QJsonObject jsonObj = QJsonDocument::fromJson(jsonStr.toUtf8()).object();

    QJsonObject categories = jsonObj["categories"].toObject();
    QList<QPair<int, QString>> categoryValKeyPairs;
    for (auto& key : categories.keys())
        categoryValKeyPairs.append(QPair<int, QString>(categories[key].toInt(), key));
    std::sort(std::begin(categoryValKeyPairs), std::end(categoryValKeyPairs));
    QList<QString> categoryOptions;
    for (auto& valKeyPair : categoryValKeyPairs)
        categoryOptions.append(valKeyPair.second);

    QJsonObject insulation = jsonObj["insulation"].toObject();
    QList<QPair<int, QString>> insulationValKeyPairs;
    for (auto& key : insulation.keys())
        insulationValKeyPairs.append(QPair<int, QString>(insulation[key].toInt(), key));
    std::sort(std::begin(insulationValKeyPairs), std::end(insulationValKeyPairs));
    QList<QString> insulationOptions;
    for (auto& valKeyPair : insulationValKeyPairs)
        insulationOptions.append(valKeyPair.second);

    QJsonArray gauges = jsonObj["guages"].toArray();
    QList<QString> gaugeOptions;
    for (int i = 0; i != gauges.size(); i++)
        gaugeOptions.push_back(QString::number(gauges.at(i).toInt()));

    QJsonArray conductors = jsonObj["conductors"].toArray();
    QList<QString> conductorOptions;
    for (int i = 0; i != conductors.size(); i++)
        conductorOptions.push_back(QString::number(conductors.at(i).toInt()));

    std::vector<QList<QString>> cableOptions;
    cableOptions.push_back(categoryOptions);
    cableOptions.push_back(insulationOptions);
    cableOptions.push_back(gaugeOptions);
    cableOptions.push_back(conductorOptions);

    return cableOptions;
}

void FEngine::addCableProperty(NECableProperty cable)
{
    QString jsonStr = P->createJSONFromCableProperties( cable );

    int cableSpecsID = P->AddCableSpecDB( jsonStr.toUtf8() );
    cable.setUniqueID(cableSpecsID);

    P->m_CablePropertyList.push_back( cable );

    emit sendUpdateCableProperty( cable );

}
void FEngine::removeCableProperty(NECableProperty cable)
{
    QString jsonStr = P->createJSONFromCableProperties( cable );

    //
    // Add Array
    //
    jsonStr.insert( 0, "[");
    jsonStr.append( ']' );

//    P->DeleteCableSpecDB( jsonStr.toUtf8() );

    P->m_CablePropertyList.removeAll( cable );
}

void FEngine::assignCableProperty(NENode* node, NECableProperty cable)
{
    if( FCables *cable = qobject_cast< FCables *>( node ) )
        P->saveModelItem( cable->objectName() );
}

QList<NEVentCriteria> FEngine::getVentCriteria(uint zoneId)
{
    QList<NEVentCriteria> list;

    QString criteriaList = P->GetSimulationItemsInZoneForTypeDB(zoneId, "siCriterion");

    QJsonDocument doc = QJsonDocument::fromJson(criteriaList.toUtf8());
    QJsonArray criteria = doc.array();
    for (int i = 0; i < criteria.size(); ++i)
    {
        QJsonDocument doc(criteria[i].toObject());
        QByteArray docByteArray = doc.toJson(QJsonDocument::Compact);
        QString jsonStr = QLatin1String(docByteArray);
        NEVentCriteria criterion = P->createVentCriteriaFromJSON(jsonStr);
        criterion.setIndex(i+1); // index 0 is for the null vent criterion
        list.append(criterion);
    }

    return list;
}

void FEngine::addVentCriteria(NEVentCriteria criterion)
{
    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));

    if (activeZone == nullptr)
        return;

    QString jsonStr = P->createJSONFromVentCriteria(criterion);
    int id = P->SaveSingleSimulationItemDB(jsonStr.toUtf8(), activeZone->ID());
    criterion.setID(id);

    P->m_VentCriteriaList.push_back(criterion);

    emit sendUpdateVentCriteria(criterion);
}

void FEngine::removeVentCriteria(NEVentCriteria criterion)
{
    P->m_VentCriteriaList.removeAll(criterion);

    QString jsonStr = P->createJSONFromVentCriteria(criterion);
    jsonStr.insert(0, "[");
    jsonStr.append(']');

    P->DeleteCriteriaPropsDB(jsonStr.toUtf8());
}

void FEngine::assignVentCriteria(NENode *node, NEVentCriteria criterion, QString criterionKind)
{
    if (FVent *vent = qobject_cast<FVent*>(node))
       if (FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(vent->parent()->objectName())))
       {
           vent->SetCriterion(criterion);

           QString jsonStr = P->createJSONFromVent(vent);
           P->SaveSingleSimulationItemDB(jsonStr, zone->ID());
       }
}

void FEngine::setDynamicCombo(NENode* node, NEDynamicCombo dynamicCombo, QString propName)
{
    if (FVent *vent = qobject_cast<FVent*>(node))
    {
        // Enforce the active zone to be either one of the two compartments
        if (propName == "FirstCompartment")
            vent->setFirstCompartmentFromName(vent->FirstCompartment().curName(), activeZoneName());
        else if (propName == "SecondCompartment")
            vent->setSecondCompartmentFromName(vent->SecondCompartment().curName(), activeZoneName());
    }
}

void FEngine::changeColorOfZoneItem(double elapsedTime)
{
    P->updateItemsWithFailureInfo(P->m_FailedItems);
}

FailedItems& FEngine::currentFailedItems()
{
    return P->m_FailedItems;
}

void FEngine::updateComputeScenarios(FRANXProperties fp)
{
    P->m_FRANXFile = FilePath( fp.m_FranxFile );

    for( int i = 0 ; i <  P->m_FRANXScenarios.size(); ++i )
    {
        if( P->m_FRANXScenarios[i].m_ID == fp.m_ID)
        {
            P->m_FRANXScenarios[i] = fp;
//            qDebug() << "Set " << fp.m_Name << " " << fp.m_Selected;
            break;
        }
    }

    if( fp.m_ID < 0 )
    {
        if( P->GetFranxMasterChangedDB() == true )
        {
            emit sendShowWarningMessage("Items in the FRANX file have been removed/added,"
                                        "there may be errors or results may be incorrect");
        }
        qDebug() << "Setting FRANX Master Loc to " << P->m_FRANXFile.path();
        P->SetFranxMasterLocDB( P->m_FRANXFile.path() );
    }
    return;
}

///
/// \brief FEngine::simulateScenario
/// \param scnName
/// \param desc
/// \param simEngineNodeName
///

void FEngine::simulateScenario(QString scnName, QString desc, QString simEngineNodeName)
{
    FCFAST *cfastengine = qobject_cast< FCFAST *>(NESCENE.findChildNode(simEngineNodeName));

    QString simScenarioName = scnName;

    if( cfastengine )
    {
        P->m_currentThread = this->thread();

        if( simScenarioName == "" )
            simScenarioName = activeScenarioName();

        FScenario *cscn = qobject_cast< FScenario* >( NESCENE.findChildNode( simScenarioName ) );
        QString sceneScnJson;
        QString jsonStr;

        QString simInfoProp;

        simInfoProp = cfastengine->toJson();
        QJsonDocument simInfoJsonDoc = QJsonDocument::fromJson( simInfoProp.toUtf8() );
        QJsonObject sobj = simInfoJsonDoc.object();

        QJsonObject simprops;
        simprops[ "scenarioID" ] = 0;
        simprops[ "jsonFields" ] = sobj;

        simInfoJsonDoc.setObject( simprops );
        QString sSimInfoJson = simInfoJsonDoc.toJson();

        qDebug().noquote() << sSimInfoJson ;
        //P->SaveSimInfoPropsDB( sSimInfoJson );
        if( cscn )
        {
            sceneScnJson = P->createJSONFromSceneScenario( cscn );
        }
        else
        {
            // Scenario Props
            // {
            //          "name": "SimAdded99",
            //          "desc": "Pump RPSMDP1A Fire Scenario"
            // }
            QJsonObject scnJson;
            scnJson ["name"] = simScenarioName;
            scnJson ["desc"] = desc;
            QJsonDocument scnJsonDoc( scnJson );
            sceneScnJson = scnJsonDoc.toJson();

            QJsonArray sources;
            auto s = NESCENE.selectedObject();
            if( FFire *fireSource = qobject_cast< FFire *>( s ) )
            {
                QString jsonsource = P->createJSONFromSceneItem( fireSource );
                QJsonDocument jsonResponse = QJsonDocument::fromJson( jsonsource.toUtf8() );
                QJsonObject jsonObject = jsonResponse.object();
                sources.append( QJsonValue( jsonObject ) );
            }
            QJsonDocument sdoc( sources );

            jsonStr =  sdoc.toJson() ;
        }


        emit NEBASE.showSceneLoadingWidget();
        emit NEBASE.setCurrentSubOperation("Computing CFAST...");

        P->m_worker = new FEngineWorker(P, sceneScnJson, jsonStr, sSimInfoJson, simScenarioName, activeZoneName() );
        emit sendUpdateSimTime(cfastengine->SimTime());

        connect( P->m_worker, SIGNAL(finished()), this, SLOT(finishedSimulateScenario()) );
        P->m_worker->start();
    }
}

void FEngine::finishedSimulateScenario()
{
    if( P->m_worker )
    {
        if( P->m_worker->m_WorkerResult != 0 )
        {
            //std::cout << "Scenario " << activeScenarioName().toStdString() << std::endl;

            P->m_FailedItems = P->getFailedItems( P->m_worker->m_WorkerResult );
            P->updateItemsWithFailureInfo( P->m_FailedItems );
            emit allFailedItems( P->m_FailedItems.failedItemID, P->m_FailedItems.failedTime,
                                 P->m_FailedItems.scenarioID, P->m_FailedItems.failOutput);

            P->m_ScnIdNamePairs = P->getScnIdNamePairs(P->m_FRANXScenarios);
            QStringList scnNameList = P->scnNames();
            if (scnNameList.indexOf( P->m_worker->m_scnName ) != -1)
            {
                setActiveScenario(P->m_worker->m_scnName);
                emit allScenarios(scnNameList);
            }

            emit sendUpdateComputeScenarios(P->m_FRANXScenarios);

        }
        else
        {
            // Popup error dialog.
        }
    }
    finishedCompute();

    delete P->m_worker;
}

///
/// \brief FEngine::DBName
/// \return
///

QString FEngine::DBName()
{
    return P->m_DBFile.path();
}

///
/// \brief FEngine::importFRANX
/// \return
///

uint FEngine::importFRANX(QString fileName)
{

    P->loadFRANXFile( fileName );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::importPDMS
/// \param fileName
/// \return
///

uint FEngine::importPDMS(QString fileName)
{
    P->loadPDMSFile( fileName );

    return NE::kReturnSuccess;
}


///
/// \brief FEngine::importPlantData
/// \param franxFile
/// \param pdmsFile
/// \return
///

uint FEngine::importPlantData(QString projPath, QString franxPath, QString pdmsPath)
{
    QFileInfo dbfile = QFileInfo(projPath);

    QFileInfo file1 = QFileInfo(franxPath);
    QFileInfo file2 = QFileInfo(pdmsPath);

    bool retval = false;
    if( dbfile.exists() && file1.exists() && file2.exists() )
    {
        retval = P->ImportPlantProjectsDB( projPath, franxPath, pdmsPath );
    }

    return (uint) retval;
}

///
/// \brief FEngine::saveAs
/// \param node
/// \param name
///

void FEngine::saveAs( NENode *node, QString name )
{
    FScenario *scn = qobject_cast<FScenario *>(node);
    QString jsonStr = P->createJSONFromSceneScenario(scn);

    P->SaveScenarioAsDB(jsonStr, name.toUtf8().data());
}

///
/// \brief FEngine::loadDBFile
/// \return
///

uint FEngine::loadDBFile()
{
    QStringList fileNames;

    if( NEBASE.getGUICallInterface() )
    {
        fileNames = NEBASE.getGUICallInterface()->fileImportBrowser( P->m_DBFile,
                                                                     "F Dynamics",
                                                                     "FRI3D",
                                                                     "lastOpenDBPath" );
    }

    for( int i = 0; i < fileNames.size(); ++i )
    {
        clear();
        P->loadDBFile( fileNames[i] );
        P->m_DBFile = FilePath( fileNames[i] );
    }

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::resetProperties
///

void FEngine::resetProperties()
{
    NESpatialGroup::resetProperties();

    ResetDBFile();
}

///
/// \brief FEngine::activeZoneName
/// \return
///

QString FEngine::activeZoneName()
{
    return P->m_activeZoneName;
}

///
/// \brief FEngine::activeScenarioName
/// \return
///

QString FEngine::activeScenarioName()
{
    return P->m_activeScenarioName;
}

void FEngine::addNewZoneAndScenario(NEZone* zone, NEScenario* scenario)
{
    if (FZone* newFZone = qobject_cast<FZone*>(zone))
    {
        if (FScenario* newFScn = qobject_cast<FScenario*>(scenario))
        {
            newFZone->setOriginalName(newFZone->objectName());
            newFZone->setZoneName(newFZone->objectName());
            newFZone->SetType( "foZone" );
            newFZone->SetID(0);
            newFZone->SetConnectorID(0);

            QString zoneJson = P->createJSONFromSceneZone( newFZone );

            int zoneId = P->SaveModelItemDB( zoneJson, 0 );
            newFZone->SetID(zoneId);
            newFZone->SetConnectorID(zoneId);

            newFScn->setOriginalName(newFScn->objectName());
            newFScn->setScenarioName( newFScn->objectName() );
            newFScn->SetType( "foScenario" );
            newFScn->SetID(0);
            newFScn->SetConnectorID(0);

            QString scnJson = P->createJSONFromSceneScenario( newFScn );

            int scenarioId = P->NewScenarioDB( scnJson, zoneId );
            newFScn->SetID(scenarioId);
            newFScn->SetConnectorID(scenarioId);

            setActiveZone( newFZone->zoneName() );
        }
    }
}

void FEngine::addNewScenario(NEScenario* scenario)
{
    scenarioCreated(scenario);
}

int FEngine::zoneCreated(NEZone *newZone)
{
    if( FZone* newFZone = qobject_cast< FZone *>( newZone ) )
    {
        newFZone->setOriginalName(newFZone->objectName());
        newFZone->setZoneName(newFZone->objectName());
        newFZone->SetType( "foZone" );
        newFZone->SetID(0);
        newFZone->SetConnectorID(0);

        QString zoneJson = P->createJSONFromSceneZone( newFZone );

        int id = P->SaveModelItemDB( zoneJson, 0 );
        newFZone->SetID(id);
        newFZone->SetConnectorID(id);

        setActiveZone(newFZone->objectName());

        return id;
    }

    return 0;
}

///
/// \brief FEngine::zoneDeleted
/// \param zone
///

int FEngine::zoneDeleted(NEZone *zone)
{
    return 0;
}

///
/// \brief FEngine::scenarioCreated
/// \param newScn
///

int FEngine::scenarioCreated(NEScenario *newScn)
{
    if( FScenario* newFScn = qobject_cast< FScenario *>( newScn ) )
        if (FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) ))
        {
            newFScn->setOriginalName(newFScn->objectName());
            newFScn->setScenarioName( newFScn->objectName() );
            newFScn->SetType( "foScenario" );
            newFScn->SetID(0);
            newFScn->SetConnectorID(0);

            QString json = P->createJSONFromSceneScenario( newFScn );

            int id = P->NewScenarioDB( json, cactiveZone->ID() );
            newFScn->SetID(id);
            newFScn->SetConnectorID(id);

            setActiveScenario(newFScn->objectName());

            return id;
        }

    return 0;
}

///
/// \brief FEngine::scenarioDeleted
/// \param newScn
///

int FEngine::scenarioDeleted(NEScenario *newScn)
{
    FScenario* newFScn = qobject_cast< FScenario *>( newScn );

    qDebug() << "Deleting Scenario " ;
    if( newFScn )
    {
        QString sceneScnJson = P->createJSONFromSceneScenario( newFScn );
        P->DeleteScenarioDB( sceneScnJson );
    }

    return 0;
}

int FEngine::componentCreated(NEZoneItem *newZoneItem, NENode *parent)
{
    FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( newZoneItem );

    if( !newNodeItem )
        return 0;

    newNodeItem->SetItemType( FZoneItem::Component );
    newNodeItem->SetType( "foComponent" );
    newNodeItem->setPlantItemType( "piEquipment" );
    newNodeItem->setOriginalName(newZoneItem->objectName());

    createPlantItem( newNodeItem, parent );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::itemCreated
/// \param newZoneItem
/// \param parent
///
/// Called when a zone item is created in the scene
///

int FEngine::itemCreated( NEZoneItem *newZoneItem, NENode *parent )
{
    FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( newZoneItem );

    if( !newNodeItem )
        return 0;

    newNodeItem->SetItemType( FZoneItem::Physical );
    newNodeItem->SetType( "foPlantItem" );
    newNodeItem->setPlantItemType( "piEquipment" );
    newNodeItem->setOriginalName(newNodeItem->objectName());

    createPlantItem( newZoneItem, parent );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::trayCreated
/// \param newZoneItem
/// \param parent
/// \return
///

int FEngine::trayCreated( NEZoneItem *newZoneItem, NENode* parent )
{
    FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( newZoneItem );

    if( !newNodeItem )
        return 0;

    newNodeItem->SetItemType( FZoneItem::Trays );
    newNodeItem->SetType( "foRaceway" );
    newNodeItem->setPlantItemType( "piTray" );
    newNodeItem->setOriginalName( newNodeItem->objectName() );

    createPlantItem( newZoneItem, parent );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::cableCreated
/// \param newZoneItem
/// \param parent
/// \return
///

int FEngine::cableCreated( NEZoneItem *newZoneItem, NENode* parent )
{
    FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( newZoneItem );

    if( !newNodeItem )
        return 0;

    newNodeItem->SetItemType( FZoneItem::Cable );
    newNodeItem->SetType( "foCable" );
    newNodeItem->setPlantItemType( "piCable" );
    newNodeItem->setOriginalName(newNodeItem->objectName());

    createPlantItem( newZoneItem, parent );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::createPlantItem
/// \param newZoneItem
/// \param parent
/// \return
///

int FEngine::createPlantItem( NEZoneItem *newZoneItem, NENode *parent )
{
    FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( newZoneItem );

    FZoneItem* parentZoneItem   = qobject_cast< FZoneItem *>( parent );
    FZone*     parentZone       = qobject_cast< FZone *>( parent );
    FScenario* parentScn        = qobject_cast< FScenario *>( parent );

    QString itemJson = P->createJSONFromSceneItemName( newZoneItem->objectName() );

    std::shared_ptr< FZoneItem > newZItem = std::shared_ptr< FZoneItem >( new FZoneItem );
    newZItem->SetID( newNodeItem->ID() );
    newZItem->SetConnectorID( newNodeItem->ConnectorID() );
    newZItem->SetType( newNodeItem->Type() );
    newZItem->setPlantItemType( newNodeItem->PlantItemType() );
    newZItem->SetItemType( newNodeItem->ItemType() );
    newZItem->SetDesc( newNodeItem->Desc() );
    newZItem->SetIssues( newNodeItem->Issues() );
    newZItem->setOriginalName(newNodeItem->originalName());
    newZItem->setZoneItemName( newNodeItem->objectName() );
    newZItem->setModel3DInfo( newNodeItem->model3DInfo() );
    newZItem->SetCompartmentID( newNodeItem->CompartmentID() );

    if( parentZone )
    {
        P->m_ZoneItems[ parentZone->ID() ].insert( newZItem->ID(), newZItem );
        parentZone->addToItemNameTree( newZoneItem->objectName(), parentZone->objectName() );
    }
    else if( parentScn )
    {
        P->m_ScenarioItems[ parentScn->ID() ].insert( newZItem->ID(), newZItem );
        parentScn->addToItemNameTree( newZoneItem->objectName(), parentScn->objectName() );
    }
    else if( parentZoneItem )
    {
        P->m_SubSceneZoneItems[ parentZoneItem->ID() ].insert( newZItem->ID(), newZItem );
        if( FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())) )
            activeZone->addToItemNameTree( newZoneItem->objectName(), parentZoneItem->objectName() );
        P->m_SubSceneScenarioItems[ parentZoneItem->ID() ].insert( newZItem->ID(), newZItem );
        if( FScenario* activeScenario = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName())) )
            activeScenario->addToItemNameTree( newZoneItem->objectName(), parentZoneItem->objectName());
    }

    //
    // Add code to add zone item to database
    // ...
    int newConnId = -1;
    int parentId = -1;

    if( parentZoneItem )
        parentId = parentZoneItem->ID();
    else if( parentZone )
        parentId = parentZone->ID();
    else if( parentScn )
        parentId = parentScn->ID();

    FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));

    if( activeZone )
        newConnId = P->NewModelItemDB( itemJson, activeZone->ID(), parentId );

    if(newConnId > 0)
    {
        newZItem->SetID(newConnId);
        newZItem->SetConnectorID(newZItem->ID());

        newNodeItem->SetID( newZItem->ID() );
        newNodeItem->SetConnectorID( newZItem->ConnectorID() );

//        emit NEBASE.updatePropertyEditor();
    }

    return newConnId;
}

///
/// \brief FEngine::itemDeleted
/// \param newZoneItem
///

int FEngine::itemDeleted(NEZoneItem *zoneItem, bool force)
{
    QString itemJson;
    int parentZoneId = 0;

    if(FZoneItem* item = qobject_cast< FZoneItem *>( zoneItem ))
    {
        itemJson = P->createJSONFromSceneItemName(zoneItem->objectName());

        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
            P->m_ZoneItems[activeZone->ID()].remove(item->ID());
        if (FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName())))
            P->m_ScenarioItems[activeScn->ID()].remove(item->ID());

        if( FZoneItem* parentZoneItem = qobject_cast< FZoneItem *>(zoneItem->parent()) )
        {
            P->m_SubSceneZoneItems[ parentZoneItem->ID() ].remove(zoneItem->ID() );
            P->m_SubSceneScenarioItems[ parentZoneItem->ID() ].remove( zoneItem->ID() );
            parentZoneId = parentZoneItem->ID();
        }
    }
    else if (FZoneCompartment* comp = qobject_cast<FZoneCompartment*>(zoneItem))
    {
        itemJson = P->createJSONFromCompartment(comp);

        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
            P->m_ZoneCompartments[activeZone->ID()].remove(comp->ID());
    }
    else if (FVent* vent = qobject_cast<FVent*>(zoneItem))
    {
        itemJson = P->createJSONFromVent(vent);

        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
            P->m_ZoneVents[activeZone->ID()].remove(vent->ID());
        if (FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName())))
            P->m_ScenarioVents[activeScn->ID()].remove(vent->ID());
    }

    if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
        activeZone->removeAllFromItemNameTree(zoneItem->objectName());
    if (FScenario* activeScn = qobject_cast<FScenario*>(NESCENE.findChildNode(activeScenarioName())))
        activeScn->removeAllFromItemNameTree(zoneItem->objectName());

    if( force == true )
        P->DeleteItemDB(itemJson);
    else
        P->RemoveItemDB(itemJson, parentZoneId );

    NESCENE.deleteObject(zoneItem, true);

    emit NEBASE.updateSceneInspector();

    return 0;
}

///
/// \brief FEngine::duplicateObject
/// \param item
///

void FEngine::duplicateObject(NEZoneItem *item)
{
    NEZoneItem *nitem = NULL;
    NENode* parentNode = qobject_cast< NENode *>( item->parent() );

    if( item->Type() == "foZoneBoundary" )
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject( "Fire Zone Boundary",
                                      NESCENE.geometryManager().get()->nextObjectName( "Fire Zone Boundary", "" ),
                                      parentNode ) );
        if( nitem )
            addCompartment( nitem, parentNode );

    }
    else if( item->Type() == "foSimulationItem" )
    {
        if( qobject_cast< FVent *>( item ) )
            nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject( "Fire Vent",
                                             NESCENE.geometryManager().get()->nextObjectName("Fire Vent", ""),
                                             parentNode ) );
        if( nitem )
            addVent( nitem );
    }
    else if(item->Type() == "foRaceway")
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                            NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                            parentNode ) );
        if( nitem )
        {
            trayCreated( nitem, parentNode );
        }
    }
    else if(item->Type() == "foCable")
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject("Cables",
                                            NESCENE.geometryManager().get()->nextObjectName("Cables", ""),
                                            parentNode ) );
        if( nitem )
            cableCreated( nitem, parentNode );
    }
    else if(item->Type() == "foComponent")
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                            NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                            NESCENE.selectedNode() ) );
        if( nitem )
            itemCreated( nitem, parentNode );
    }
    else if(item->Type() == "foBasicEvent")
    {
    }
    else if( item->Type() == "foFire" )
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject( "Fire Definition",
                                                     NESCENE.geometryManager().get()->nextObjectName("Fire Definition", ""),
                                                     parentNode ) );
        if( nitem )
            addFire( nitem );
    }
    else if( item->Type() == "foPlantItem" )
    {
        nitem = qobject_cast< NEZoneItem *>( NESCENE.createObject("Fire Plant Item",
                                            NESCENE.geometryManager().get()->nextObjectName("Fire Plant Item", ""),
                                            parentNode ) );

        FZoneItem* newNodeItem      = qobject_cast< FZoneItem *>( nitem );

        if( newNodeItem )
        {
            newNodeItem->SetItemType( FZoneItem::Physical );
            newNodeItem->SetType( "foPlantItem" );
            newNodeItem->setPlantItemType( "piEquipment" );
            createPlantItem( newNodeItem, parentNode );
        }
    }
    if( item )
    {
        copyProperties( item );
        std::cout << item->Position().x() << " " << item->Position().y() << " " << item->Position().z() << std::endl;
    }
    if( nitem )
    {
        QList< NEZoneItem *> itemList;
        itemList.append( nitem );
        pasteProperties( itemList, "" );
        if( item->Behavior() == NENode::Active )
            addItemToScenario( nitem, parentNode );
    }
}

///
/// \brief FEngine::copyProperties
/// \param item
///

void FEngine::copyProperties(NEZoneItem *item)
{
//    const QMetaObject *metaObj = item->metaObject();

    QJsonArray propList = getJSONFromProperties( item );
    QJsonDocument jsonDoc;
    jsonDoc.setArray( propList );

    QMimeData *mimeData = new QMimeData;
    mimeData->setData( "application/javascript", jsonDoc.toJson() );

    QClipboard *cb = QApplication::clipboard();

    cb->setMimeData( mimeData );
    cb->setText( QString( jsonDoc.toJson( ) ) );
    //std::cout << "Item Data " << jsonDoc.toJson().toStdString() << std::endl;

//    for (int i = 0; i < metaObj->propertyCount(); ++i)
//    {
//        QMetaProperty p = metaObj->property(i);
//        QVariant v = p.read( item );
//    }
}

///
/// \brief FEngine::pasteProperties
/// \param items
///

void FEngine::pasteProperties(QList<NEZoneItem *> items, QString filter)
{
    QClipboard *cb = QApplication::clipboard();
    const QMimeData *mimeData = cb->mimeData();

    QByteArray bdata = mimeData->data("application/javascript");
    QString stdata = mimeData->text();
    //std::cout << "Data ---- " << stdata.toStdString() << std::endl;


    QByteArray jbdata = stdata.toUtf8();
    QJsonDocument jsonDoc;
    jsonDoc.fromJson( jbdata );

    for (auto &item : items)
    {
        //std::cout << "Pasting " << item->objectName().toStdString() << std::endl;
        NENode* nodeitem = qobject_cast< NENode *>( item );
        //std::cout << "NULLL1  " << stdata.toStdString() <<  std::endl;

        QString stdata2 = filterProperties( nodeitem, stdata, filter );
        //std::cout << "json" << stdata2.toStdString() << std::endl;;
        setPropertiesFromJSON( nodeitem, QString( stdata2 ) );
        //std::cout << "Paste " << item->Position().x() << " " << item->Position().y() << " " << item->Position().z() << std::endl;
        // Duplicating paste due to a bug - short term fix
        setPropertiesFromJSON( nodeitem, QString( stdata2 ) );
    }
}


QString classForProperty(QObject *object, const char *name)
{
    if (object)
    {
        const QMetaObject *mo = object->metaObject();
        const int index = mo->indexOfProperty(name);
        if (index != -1)
        {
            while (mo && mo->propertyOffset() > index)
                mo = mo->superClass();
            return QString::fromLatin1(mo->className());
        }
    }
    return QString();
}


QString FEngine::filterProperties( NENode* node, QString model3DInfoStr, QString filterStr)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson( model3DInfoStr.toUtf8() );

    QJsonArray jsonArray = jsonResponse.array();
    QJsonArray jsonArray2;

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject    = r.toObject();
        QString propertyName        = jsonObject["name"].toString();
        //std::cout << "Prop " << propertyName.toStdString() << std::endl;
        QString propertyTypeStr     = jsonObject["typeName"].toString();
        //uint    propertyType        = jsonObject["type"].toInt();
        QVariant v = jsonObject["value"].toVariant();

        if( filterStr != "" )
        {

            QString propclass = classForProperty(node, propertyName.toStdString().c_str());
    //        std::cout << "Class Property " <<  propclass.toStdString() <<
    //                     std::endl;
            QStringList filterStrList = filterStr.split(QLatin1Char('|'), Qt::SkipEmptyParts);

            QJsonObject jsonObject2;
            if( filterStrList.contains( propclass ) )
            {
                jsonObject2 = r.toObject();
                jsonArray2.append( jsonObject2 );
            }
        }
        else
        {
            QJsonObject jsonObject2;
            jsonObject2 = r.toObject();
            jsonArray2.append( jsonObject2 );
        }

    }
    QJsonDocument jsonDoc;
    jsonDoc.setArray( jsonArray2 );
    QString retJson = jsonDoc.toJson();

    return retJson;
}

///
/// \brief FEngine::addCompartment
/// \param node
/// \param parent
///

int FEngine::addCompartment(NENode *node, NENode *parent)
{
    FZoneCompartment* comp = qobject_cast< FZoneCompartment *>( node );
    FZone* parentZone = qobject_cast< FZone *>( parent );

    if( comp == nullptr )
        return 0;

    if( parent == nullptr )
    {
        if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
        {
            parentZone = activeZone;
            node->setParent( parentZone );
        }
        else
            return 0;
    }

    comp->SetType( "foZoneBoundary" );
    comp->setOriginalName(node->objectName());

    QString compJson = P->createJSONFromCompartment( comp );

    //int id = P->SaveModelItemDB( compJson, static_cast<int>(parentZone->ID()) );
    int id = P->NewModelItemDB( compJson, static_cast<int>(parentZone->ID()), static_cast<int>(parentZone->ID())  );
    comp->SetID( id );

    std::shared_ptr< FZoneCompartment > newComp = std::shared_ptr< FZoneCompartment >( new FZoneCompartment );
    newComp->setOriginalName(comp->originalName());
    newComp->setCompartmentName( comp->objectName() );
    newComp->SetDesc( comp->Desc() );
    newComp->SetID( comp->ID() );
    newComp->SetType( comp->Type() );
    newComp->setItemType( comp->itemType() );
    newComp->SetThickness( comp->Thickness() );

    newComp->setModel3DInfo( comp->model3DInfo() );

    P->m_ZoneCompartments[parentZone->ID()].insert( newComp->ID(), newComp );
    parentZone->addToItemNameTree(comp->objectName(), parentZone->objectName());

    P->updateItemsWithCompartmentInfo();
    P->updateVentsWithZoneInfo();

//    emit NEBASE.updateSceneInspector();

    return id;
}

///
/// \brief FEngine::addItemToScenario
/// \param item
/// \param parent
///

void FEngine::addItemToScenario(NEZoneItem *zoneItem, NENode *parent)
{
    if ( zoneItem == nullptr || parent == nullptr )
        return;

    if (FScenario* activeScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) ))
    {
        uint parentID = 0;
        if( NEZone* zone = qobject_cast< NEZone *>( parent ) )
            parentID = zone->ID();
        else if( NEZoneItem* parentZoneItem = qobject_cast< NEZoneItem *>( parent ) )
            parentID = parentZoneItem->ID();
        else
            return;

        if (FZoneItem* item = qobject_cast< FZoneItem *>( zoneItem ))
        {
            std::shared_ptr< FZoneItem > zItem;
            if (qobject_cast<FFire*>(item))
                zItem = std::shared_ptr<FFire>(new FFire);
            else
                zItem = std::shared_ptr<FZoneItem>(new FZoneItem);

            zItem->SetID( item->ID() );
            zItem->SetConnectorID( item->ConnectorID() );
            zItem->SetType( item->Type() );
            zItem->SetItemType( item->ItemType () );
            zItem->setPlantItemType( item->PlantItemType() );
            zItem->SetDesc( item->Desc() );
            zItem->SetIssues( item->Issues() );
            zItem->setOriginalName(item->originalName());
            zItem->setZoneItemName( item->objectName() );
            zItem->setModel3DInfo( item->model3DInfo() );

            if (FFire* zFireSource = qobject_cast<FFire*>(zItem.get()))
            {
                if (FFire* fireSource = qobject_cast<FFire*>(item))
                {
                    zFireSource->SetFireIgnition(fireSource->FireIgnition());
                    zFireSource->SetFireMaterial(fireSource->FireMaterial());
                }
            }

            if (qobject_cast<NEZone*>(parent))
                activeScn->addToItemNameTree(item->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(item->objectName(), parent->objectName());
            P->m_ScenarioItems[ activeScn->ID() ].insert( item->ID(), zItem );

            QString jsonStr = P->createJSONFromSceneItemName( item->objectName() );
            P->AddItemToScenarioDB( jsonStr, activeScn->ID(), parentID );
        }
        else if (FVent* vent = qobject_cast< FVent *>( zoneItem ))
        {
            std::shared_ptr< FVent > newVent;
            if (qobject_cast<FMechVent*>(vent))
                newVent = std::shared_ptr<FMechVent>(new FMechVent);
            else
                newVent = std::shared_ptr<FVent>(new FVent);

            newVent->SetID( vent->ID() );
            newVent->SetType( vent->Type() );
            newVent->SetDesc( vent->Desc() );
            newVent->SetIssues( vent->Issues() );
            newVent->SetFirstCompartment( vent->FirstCompartment() );
            newVent->SetSecondCompartment( vent->SecondCompartment() );
            newVent->SetCriterion( vent->Criterion() );
            newVent->setOriginalName(vent->originalName());
            newVent->setName( vent->objectName() );
            newVent->setModel3DInfo( vent->model3DInfo() );
            newVent->setFirstCompartmentId( vent->firstCompartmentId() );
            newVent->setSecondCompartmentId( vent->secondCompartmentId() );

            if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newVent.get()))
            {
                if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent))
                {
                    newMechVent->SetFlowRate(mechVent->FlowRate());
                    newMechVent->SetBeginDropOff(mechVent->BeginDropOff());
                    newMechVent->SetZeroAt(mechVent->ZeroAt());
                    newMechVent->SetFilterEfficiency(mechVent->FilterEfficiency());
                    newMechVent->SetFilterStart(mechVent->FilterStart());
                }
            }
            if (qobject_cast<NEZone*>(parent))
                activeScn->addToItemNameTree(vent->objectName(), activeScn->objectName());
            else
                activeScn->addToItemNameTree(vent->objectName(), parent->objectName());
            P->m_ScenarioVents[ activeScn->ID() ].insert( vent->ID(), newVent );

            QString jsonStr = P->createJSONFromVent( vent );
            P->AddItemToScenarioDB( jsonStr, activeScn->ID(), parentID );
        }
    }
}

///
/// \brief FEngine::removeItem
/// \param zoneItem
/// \param parentscn
///

void FEngine::removeItemFromScenario(NEZoneItem *zoneItem, NENode *parent)
{
    if( zoneItem == nullptr || parent == nullptr )
        return;

    if( FScenario* activeScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) ) )
    {
        uint parentID = 0;
        if( NEZone* zone = qobject_cast< NEZone *>( parent ) )
            parentID = zone->ID();
        else if( NEZoneItem* parentZoneItem = qobject_cast< NEZoneItem *>( parent ) )
            parentID = parentZoneItem->ID();
        else if (qobject_cast<NEScenario*>(parent))
        {
            if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())))
                parentID = activeZone->ID();
        }

        if (FZoneItem* item = qobject_cast< FZoneItem *>( zoneItem ))
        {
            QString jsonStr = P->createJSONFromSceneItem( item );
            P->RemoveItemFromScenarioDB( jsonStr, activeScn->ID(), parentID );

            if (qobject_cast<NEZone*>(parent))
                activeScn->removeFromItemNameTree(item->objectName(), activeScn->objectName());
            else
                activeScn->removeFromItemNameTree(item->objectName(), parent->objectName());
            P->m_ScenarioItems[ activeScn->ID() ].remove( item->ID()  );
        }
        else if (FVent* vent = qobject_cast< FVent *>( zoneItem ))
        {
            QString jsonStr = P->createJSONFromVent( vent );
            P->RemoveItemFromScenarioDB( jsonStr, activeScn->ID(), parentID );

            if (qobject_cast<NEZone*>(parent))
                activeScn->removeFromItemNameTree(vent->objectName(), activeScn->objectName());
            else
                activeScn->removeFromItemNameTree(vent->objectName(), parent->objectName());
            P->m_ScenarioVents[ activeScn->ID() ].remove( vent->ID() );
        }
    }
}

void FEngine::removeItemFromScenarios(NEZoneItem *zoneItem, QList<NENode *> scnlist)
{
    if( scnlist.size() == 0 )
    {
        // Remove from local databases which only store current
        if( FScenario* activeScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) ) )
        {
            if ( FZoneItem* item = qobject_cast< FZoneItem *>( zoneItem ) )
            {
                if (qobject_cast<NEZone*>(item->parent()))
                    activeScn->removeFromItemNameTree(item->objectName(), activeScn->objectName());
                else
                    activeScn->removeFromItemNameTree(item->objectName(), item->parent()->objectName());
                P->m_ScenarioItems[ activeScn->ID() ].remove( item->ID() );

                //
                // Database call to remove from all Scenarios
                //
                QString jsonStr = P->createJSONFromSceneItem( item );
                P->RemoveItemFromAllScenariosDB( jsonStr );
            }
            else if ( FVent* vent = qobject_cast< FVent *>( zoneItem ) )
            {
                if (qobject_cast<NEZone*>(vent->parent()))
                    activeScn->removeFromItemNameTree(vent->objectName(), activeScn->objectName());
                else
                    activeScn->removeFromItemNameTree(vent->objectName(), vent->parent()->objectName());
                P->m_ScenarioVents[ activeScn->ID() ].remove( vent->ID() );

                //
                // Database call to remove from all Scenarios
                //
                QString jsonStr = P->createJSONFromVent( vent );
                P->RemoveItemFromAllScenariosDB( jsonStr );
            }
        }
    }
}

///
/// \brief FEngine::setActiveZone
/// \param zoneName
///

void FEngine::setActiveZone( QString zoneName )
{
    emit NEBASE.selectObject(NULL);
    emit NEBASE.closeAllPropertyEditors();

    emit NEBASE.showSceneLoadingWidget();
    emit NEBASE.setCurrentOperation( QString( "Switching Zone to " ) + zoneName );
    std::shared_ptr< FZone > zone = nullptr;

    emit NEBASE.setCurrentSubOperation( "Saving item's model data" );
    P->saveModelItems();

    emit NEBASE.setCurrentSubOperation( "Deleting active zone" );
    FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );

    if( cactiveZone )
    {
        //
        // Save the current zone - this includes model3D info which might have cameras etc
        //
        saveZone( cactiveZone->zoneName(), "" );

        //
        // Clearing internal storage
        //
        P->m_ZoneItems.clear();
        P->m_SubSceneZoneItems.clear();
        P->m_ZoneCompartments.clear();
        P->m_ZoneVents.clear();
        //
        // Clear Scene Storage
        //
        NESCENE.deleteObject( (NENode *) cactiveZone, true );
    }
    NESCENE.cleanup();

    emit NEBASE.setCurrentSubOperation("Deleting active scenario");
    FScenario* cactiveScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) );
    if( cactiveScn )
    {
        //
        // Clearing internal storage
        //
        P->m_ScenarioItems.clear();
        P->m_ScenarioVents.clear();
        P->m_SubSceneScenarioItems.clear();
        //
        // Clear Scene Storage
        //
        NESCENE.deleteObject( cactiveScn, true );
    }

    P->m_DefaultSurfaceMaterialAssignedIndexes.clear();

    NESCENE.cleanup();

    emit repaintGLWindows();

    NEBASE.setCurrentSubOperation(QString("Setting Active Zone ")+zoneName);

    zone = P->createZone( zoneName, "" );

    P->m_activeZoneName = zone->zoneName();

    FZone* newZone = P->createSceneZone( this, zone );

    NESCENE.initChildrenOnLoad();

    FEngine *curengine = this;
    if( newZone )
        emit activeZone((QObject* &)curengine, (QObject* &)newZone);

    FScenario* newActiveScn = qobject_cast<FScenario*>( NESCENE.findChildNode(activeScenarioName()) );
    if( newActiveScn )
    {
        P->m_FailedItems = P->getFailedItems( newActiveScn->ConnectorID() );
        P->updateItemsWithFailureInfo( P->m_FailedItems );
        emit allFailedItems( P->m_FailedItems.failedItemID, P->m_FailedItems.failedTime,
                             P->m_FailedItems.scenarioID, P->m_FailedItems.failOutput );
    }

    P->m_SurfaceMaterials = getSurfaceMaterials();
    // Reset and increment the indexes of non-default surface materials
    int i = 1; // index 0 is for the null material
    for( NESurfaceMaterial& sm : P->m_SurfaceMaterials )
    {
        sm.setIndex( i+P->m_DefaultSurfaceMaterialAssignedIndexes.size() );
        i++;
    }

    // Preprend the default surface materials, sorted in ascending index order
    std::sort( std::begin(P->m_DefaultSurfaceMaterialAssignedIndexes), std::end(P->m_DefaultSurfaceMaterialAssignedIndexes) );
    P->m_SurfaceMaterialsWithAssignedDefaults = P->m_SurfaceMaterials;
    i = P->m_DefaultSurfaceMaterialAssignedIndexes.size();

    for( auto itr = P->m_DefaultSurfaceMaterialAssignedIndexes.rbegin();
         itr != P->m_DefaultSurfaceMaterialAssignedIndexes.rend(); itr++ )
    {
        P->m_SurfaceMaterialsWithAssignedDefaults.prepend( P->m_DefaultSurfaceMaterials.at(*itr) );
        P->m_SurfaceMaterialsWithAssignedDefaults.front().setIndex(i);
        i--;
    }

    emit sendUpdateSurfaceMaterials( P->m_SurfaceMaterialsWithAssignedDefaults );

    // Reset the surface materials of zones, to set the right indexes
    P->updateActiveZoneWithMaterialInfo();
    // Reset the surface materials of plant items, to set the right indexes
    P->updateItemsWithMaterialInfo();

    P->m_FireMaterials = getFireMaterials();
    emit sendUpdateFireMaterials( P->m_FireMaterials );

    P->m_FailCriteriaList = getFailureCriteria();
    emit sendUpdateFailureCriterias( P->m_FailCriteriaList );

    //
    // Update current scenario in FRANX
    //

    QString franxfile = P->GetFranxMasterLocDB();
    qDebug() << "FRANX Loc " << franxfile;
    P->m_FRANXFile = franxfile;
    qDebug() << P->m_FRANXFile.path();
    for (FRANXProperties& franxScn : P->m_FRANXScenarios)
    {
        if (franxScn.m_CleanName == activeScenarioName())
        {
            P->m_FRANXScenarios[i].m_Selected = true;
            P->m_FRANXScenarios[i].m_FranxFile = franxfile;
        }
        else
        {
            P->m_FRANXScenarios[i].m_Selected = false;
            P->m_FRANXScenarios[i].m_FranxFile = "";
        }
    }
    emit sendUpdateComputeScenarios( P->m_FRANXScenarios );

    // Cable options must be accessed before cable properties
    //P->m_CableOptions = getCableOptions();
    //P->m_CablePropertyList = getCableProperty();
    //emit sendUpdateCableProperties(P->m_CablePropertyList, P->m_CableOptions);

    emit NEBASE.setCurrentSubOperation("Updating inspectors and GL window");

    emit NEBASE.updateSceneInspector();
    emit NEBASE.updateGLWindows();

    emit NEBASE.hideSceneLoadingWidget();
}

///
/// \brief FEngine::setActiveScenario
/// \param scenarioName
///

void FEngine::setActiveScenario(QString scenarioName)
{
    emit NEBASE.selectObject(NULL);
    emit NEBASE.closeAllPropertyEditors();

    emit NEBASE.showSceneLoadingWidget();
    emit NEBASE.setCurrentOperation(QString("Switching scenario to ") + scenarioName );

    emit NEBASE.setCurrentSubOperation("Saving item's model data");
    P->saveModelItems();

    emit NEBASE.setCurrentSubOperation("Deleting active zone");
    FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );
    if( cactiveZone )
    {
        //
        // Clearing internal storage
        //
        P->m_ZoneItems.clear();
        P->m_SubSceneZoneItems.clear();
        P->m_ZoneCompartments.clear();
        P->m_ZoneVents.clear();
        //
        // Clearing Scene Storage
        //
        NESCENE.deleteObject( cactiveZone, true );
    }

    NESCENE.cleanup();

    emit NEBASE.setCurrentSubOperation("Deleting active scenario");
    FScenario* cactiveScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) );
    if( cactiveScn )
    {
        //
        // Clearing internal storage
        //
        P->m_ScenarioItems.clear();
        P->m_ScenarioVents.clear();
        P->m_SubSceneScenarioItems.clear();
        //
        // Clearing Scene Storage
        //
        NESCENE.deleteObject( cactiveScn, true );
    }

    NESCENE.cleanup();

    emit repaintGLWindows();

    emit NEBASE.setCurrentSubOperation(QString("Creating Scenario ")+scenarioName);
    std::shared_ptr< FScenario > scn = P->createScenario( scenarioName, "" );
    P->m_activeScenarioName = scn->scenarioName();

    FScenario* newScn = P->createSceneScenario( this, scn );

    NESCENE.initChildrenOnLoad();

    FEngine *curengine = this;

    if( newScn )
        emit activeScenario((QObject* &)curengine, (QObject* &)newScn);

    P->updateItemsWithCompartmentInfo();
    P->updateVentsWithZoneInfo();

    if (newScn)
    {
        P->m_FailedItems = P->getFailedItems(newScn->ConnectorID());
        P->updateItemsWithFailureInfo(P->m_FailedItems);
        emit allFailedItems( P->m_FailedItems.failedItemID, P->m_FailedItems.failedTime, P->m_FailedItems.scenarioID, P->m_FailedItems.failOutput );
    }

    //
    // Update current scenario in FRANX

    for( int i = 0; i < P->m_FRANXScenarios.size(); ++i )
    {
        if( activeScenarioName() == P->m_FRANXScenarios[i].m_CleanName )
        {
            P->m_FRANXScenarios[i].m_Selected = true;
        }

    }

    emit sendUpdateComputeScenarios( P->m_FRANXScenarios );

    emit NEBASE.setCurrentSubOperation("Updating inspectors and GL window");
    emit NEBASE.updateSceneInspector();
    emit NEBASE.updateGLWindows();
    emit NEBASE.hideSceneLoadingWidget();

    return;
}

////////////////////////////
/// FEnginePrivate Methods
////////////////////////////

///
/// \brief FEnginePrivate::createItemsFromJSON
/// \param parent
/// \param grandParent
/// \param compsref
/// \param itemType
///

void FEnginePrivate::createZoneItemsFromJSON( std::shared_ptr< FZone > zone,
                                              NESpatialGroup* parent,
                                              QJsonValueRef& compsref,
                                              FZoneItem::FItemType itemType )
{
    QJsonArray::const_iterator jitr = compsref.toArray().begin();

    for( int i = 0; i < compsref.toArray().size(); ++i )
    {
        QJsonValueRef r = compsref.toArray()[i];
        QJsonObject   jsonObject = r.toObject();

        if (jsonObject["type"].toString() == "foZoneBoundary")
        {
            QString originalName = jsonObject["name"].toString();
            QString name = jsonObject["cleanName"].toString();
            QString desc = jsonObject["desc"].toString();
            QString type = jsonObject["type"].toString();
            uint connectorID = jsonObject["connectorID"].toInt();
            QString issues = jsonObject["issues"].toString();

            if( name == "" )
                name = "Empty_" + QString::number( connectorID );

            QJsonArray model3DInfo = jsonObject["model3DInfo"].toArray();

            auto comp = std::shared_ptr<FZoneCompartment>(new FZoneCompartment);
            comp->setOriginalName(originalName);
            comp->setCompartmentName(name);
            comp->SetDesc(desc);
            comp->SetType(type);
            comp->SetID(connectorID);

            comp->setModel3DInfo(QString(QJsonDocument(model3DInfo).toJson()));

            if (FZone* zone = qobject_cast<FZone*>(parent))
                m_ZoneCompartments[zone->ID()].insert(comp->ID(), comp);
        }
        else
        {
            QString originalName = jsonObject["name"].toString();
            QString zoneItemName    = jsonObject["cleanName"].toString();

            //
            // Qt Cant handle colons in the name - so rename the item - however
            // this needs to be communicated to the database.
            //
            //zoneItemName.remove( QChar(':') );
            QString zoneItemDesc    = jsonObject["desc"].toString();
            QString zoneItemType    = jsonObject["type"].toString();

            QString zonepItemType   = jsonObject["pItemType"].toString();
            uint    zoneItemConnID  = jsonObject["connectorID"].toInt();
            uint    surfaceMatID    = jsonObject["materialID"].toInt();
            uint    fireMatId       = jsonObject["fireMaterialID"].toInt();
            uint    failCriteriaID  = jsonObject["failureCriteriaID"].toInt();
            uint    cableSpecsId      = jsonObject["cableSpecsID"].toInt();

            if( zoneItemName == "" )
                zoneItemName = "Empty_" + QString::number( zoneItemConnID );
            //
            // Following getSurfaceMaterials and getFailureCriteria
            // is commented now because we need to replace it
            // to a way whereby we can get the surface materials loaded on memory without a DB call
            //
            static NESurfaceMaterial curmat;
            bool found = false;
            for( int i = 0; i < m_SurfaceMaterials.size(); ++i )
            {
                if( m_SurfaceMaterials[i].uniqueID() == surfaceMatID )
                {
                    curmat = m_SurfaceMaterials[i];
                    found = true;
                    break;
                }
            }
            for( int i = 0; i < m_DefaultSurfaceMaterials.size(); ++i )
            {
                if( !found && m_DefaultSurfaceMaterials[i].uniqueID() == surfaceMatID && surfaceMatID != 0 )
                {
                    curmat = m_DefaultSurfaceMaterials[i];
                    if (!m_DefaultSurfaceMaterialAssignedIndexes.contains(i))
                        m_DefaultSurfaceMaterialAssignedIndexes.push_back(i);
                    found = true;
                    break;
                }
            }
            if( !found )
            {
                curmat.setName( "Null" );
                curmat.setUniqueID(0);
                curmat.setIndex(0);
            }

            static NEFireMaterial curFireMat;
            found = false;
            for( int i = 0; i < m_FireMaterials.size(); ++i )
            {
                if( m_FireMaterials[i].uniqueID() == fireMatId )
                {
                    curFireMat = m_FireMaterials[i];
                    found = true;
                    break;
                }
            }
            if( !found )
            {
                curFireMat.setName("Null");
                curFireMat.setUniqueID(0);
                curFireMat.setIndex(0);
            }

            static NEFailureCriteria curfail;
            found = false;
            for( int i = 0; i < m_FailCriteriaList.size(); ++i )
            {
                if( m_FailCriteriaList[i].ID() == failCriteriaID )
                {
                    curfail = m_FailCriteriaList[i];
                    found = true;
                    break;
                }
            }
            if( !found )
            {
                curfail.setName("Null");
                curfail.setID(0);
                curfail.setIndex(0);
            }

            static NECableProperty curCableSpecs;
            found = false;
            for( int i = 0; i < m_CablePropertyList.size(); ++i )
            {
                if( m_CablePropertyList[i].uniqueID() == cableSpecsId )
                {
                    curCableSpecs = m_CablePropertyList[i];
                    found = true;
                    break;
                }
            }
            if( !found )
            {
                curCableSpecs.setName("Null");
                curCableSpecs.setUniqueID(0);
                curCableSpecs.setIndex(0);
            }

            uint    zoneItemID      = zoneItemConnID;
            QString zoneItemissues  = jsonObject["issues"].toString();
            QJsonArray model3DInfo  = jsonObject["model3DInfo"].toArray();
            QJsonObject jsonFields  = jsonObject["jsonFields"].toObject();
            QJsonObject cfast       = jsonFields["cfast"].toObject();
            uint compartmentID      = cfast["compartmentID"].toInt();

            std::shared_ptr< FZoneItem > newItem;
            if( itemType == FZoneItem::Sources )
                newItem = std::shared_ptr< FFire >( new FFire );
            else if (itemType == FZoneItem::Cable)
                newItem = std::shared_ptr<FCables>(new FCables);
            else
                newItem = std::shared_ptr< FZoneItem >( new FZoneItem );

            newItem->SetItemType( itemType );
            newItem->SetDesc( zoneItemDesc );
            newItem->SetID( zoneItemID );
            newItem->SetConnectorID( zoneItemConnID );
            newItem->SetIssues( zoneItemissues );
            newItem->SetType( zoneItemType );
            newItem->setOriginalName(originalName);
            newItem->setZoneItemName( zoneItemName );
            newItem->setPlantItemType( zonepItemType );
            newItem->SetMaterial( curmat );
            newItem->SetFailure( curfail );
            newItem->SetCompartmentID( compartmentID );

            if (FFire *newFireItem = qobject_cast<FFire*>(newItem.get()))
                newFireItem->SetFireMaterial(curFireMat);

            if (FCables* newCable = qobject_cast<FCables*>(newItem.get()))
                newCable->SetCableMaterial(curCableSpecs);

            QJsonDocument doc;
            doc.setArray( model3DInfo );
            QString dataToString( doc.toJson() );
            newItem->setModel3DInfo( dataToString );

            //
            // If Parent is a zone then add the current zone item to its list
            // Otherwise if a parent is a zone item then it has subzoneitems so add it to the appropriate list
            //
            if( qobject_cast< FZone *>( parent ) )
            {
                if( !m_ZoneItems[ zone->ID() ].contains( zoneItemID ) )
                    m_ZoneItems[ zone->ID() ].insert( zoneItemID, newItem );
            }
            else if ( FZoneItem* zoneItem = qobject_cast< FZoneItem *>( parent ) )
            {
                if( !m_SubSceneZoneItems[ zoneItem->ID() ].contains( zoneItemID ) )
                    m_SubSceneZoneItems[ zoneItem->ID() ].insert( zoneItemID, newItem );
            }

            //
            //
            QJsonValueRef cablesref = jsonObject["cables"];
            if( cablesref.isArray() )
                createZoneItemsFromJSON( zone, newItem.get(), cablesref, FZoneItem::Cable );
        }
    }
}

void FEnginePrivate::createScenarioItemsFromJSON( std::shared_ptr< FScenario > scenario,
                                                  NESpatialGroup* parent,
                                                  QJsonValueRef& compsref,
                                                  FZoneItem::FItemType itemType )
{
    QJsonArray::const_iterator jitr = compsref.toArray().begin();

    for( int i = 0; i < compsref.toArray().size(); ++i )
    {
        QJsonValueRef r = compsref.toArray()[i];
        QJsonObject   jsonObject = r.toObject();

        QString originalName = jsonObject["name"].toString();
        QString zoneItemName    = jsonObject["cleanName"].toString();

        //
        // Qt Cant handle colons in the name - so rename the item - however
        // this needs to be communicated to the database.
        //
        //zoneItemName.remove( QChar(':') );
        QString zoneItemDesc    = jsonObject["desc"].toString();
        QString zoneItemType    = jsonObject["type"].toString();

        QString zonepItemType   = jsonObject["pItemType"].toString();
        uint    zoneItemConnID  = jsonObject["connectorID"].toInt();
        uint    surfaceMatID    = jsonObject["materialID"].toInt();
        uint    fireMatId       = jsonObject["fireMaterialID"].toInt();
        uint    failCriteriaID  = jsonObject["failureCriteriaID"].toInt();
        uint    cableSpecsId      = jsonObject["cableSpecsID"].toInt();

        if( zoneItemName == "" )
            zoneItemName = "Empty_" + QString::number( zoneItemConnID );

        //
        // Following getSurfaceMaterials and getFailureCriteria
        // is commented now because we need to replace it
        // to a way whereby we can get the surface materials loaded on memory without a DB call
        //
        static NESurfaceMaterial curmat;
        bool found = false;
        for( int i = 0; i < m_SurfaceMaterials.size(); ++i )
        {
            if( m_SurfaceMaterials[i].uniqueID() == surfaceMatID )
            {
                curmat = m_SurfaceMaterials[i];
                found = true;
                break;
            }
        }
        for( int i = 0; i < m_DefaultSurfaceMaterials.size(); ++i )
        {
            if( !found && m_DefaultSurfaceMaterials[i].uniqueID() == surfaceMatID && surfaceMatID != 0 )
            {
                curmat = m_DefaultSurfaceMaterials[i];
                if (!m_DefaultSurfaceMaterialAssignedIndexes.contains(i))
                    m_DefaultSurfaceMaterialAssignedIndexes.push_back(i);
                found = true;
                break;
            }
        }
        if( !found )
        {
            curmat.setName( "Null" );
            curmat.setUniqueID(0);
            curmat.setIndex(0);
        }

        static NEFireMaterial curFireMat;
        found = false;
        for( int i = 0; i < m_FireMaterials.size(); ++i )
        {
            if( m_FireMaterials[i].uniqueID() == fireMatId )
            {
                curFireMat = m_FireMaterials[i];
                found = true;
                break;
            }
        }
        if( !found )
        {
            curFireMat.setName("Null");
            curFireMat.setUniqueID(0);
            curFireMat.setIndex(0);
        }

        static NEFailureCriteria curfail;
        found = false;
        for( int i = 0; i < m_FailCriteriaList.size(); ++i )
        {
            if( m_FailCriteriaList[i].ID() == failCriteriaID )
            {
                curfail = m_FailCriteriaList[i];
                found = true;
                break;
            }
        }
        if( !found )
        {
            curfail.setName("Null");
            curfail.setID(0);
            curfail.setIndex(0);
        }

        static NECableProperty curCableSpecs;
        found = false;
        for( int i = 0; i < m_CablePropertyList.size(); ++i )
        {
            if( m_CablePropertyList[i].uniqueID() == cableSpecsId )
            {
                curCableSpecs = m_CablePropertyList[i];
                found = true;
                break;
            }
        }
        if( !found )
        {
            curCableSpecs.setName("Null");
            curCableSpecs.setUniqueID(0);
            curCableSpecs.setIndex(0);
        }

        uint    zoneItemID      = zoneItemConnID;
        QString zoneItemissues  = jsonObject["issues"].toString();
        QJsonArray model3DInfo  = jsonObject["model3DInfo"].toArray();
        QJsonObject jsonFields  = jsonObject["jsonFields"].toObject();
        QJsonObject cfast       = jsonFields["cfast"].toObject();
        uint compartmentID      = cfast["compartmentID"].toInt();

        std::shared_ptr< FZoneItem > newItem;
        if( itemType == FZoneItem::Sources )
            newItem = std::shared_ptr< FFire >( new FFire );
        else if (itemType == FZoneItem::Cable)
            newItem = std::shared_ptr<FCables>(new FCables);
        else
            newItem = std::shared_ptr< FZoneItem >( new FZoneItem );

        newItem->SetItemType( itemType );
        newItem->SetDesc( zoneItemDesc );
        newItem->SetID( zoneItemID );
        newItem->SetConnectorID( zoneItemConnID );
        newItem->SetIssues( zoneItemissues );
        newItem->SetType( zoneItemType );
        newItem->setOriginalName(originalName);
        newItem->setZoneItemName( zoneItemName );
        newItem->setPlantItemType( zonepItemType );
        newItem->SetMaterial( curmat );
        newItem->SetFailure( curfail );
        newItem->SetCompartmentID( compartmentID );

        if (FFire *newFireItem = qobject_cast<FFire*>(newItem.get()))
            newFireItem->SetFireMaterial(curFireMat);

        if (FCables* newCable = qobject_cast<FCables*>(newItem.get()))
            newCable->SetCableMaterial(curCableSpecs);

        QJsonDocument doc;
        doc.setArray( model3DInfo );
        QString dataToString( doc.toJson() );
        newItem->setModel3DInfo( dataToString );

        //
        // If Parent is a scenario then add the current zone item to its list
        // Otherwise if a parent is a zone item then it has subzoneitems so add it to the appropriate list
        //
        if( qobject_cast< FScenario *>( parent ) )
        {
            if( !m_ScenarioItems[ scenario->ID() ].contains( zoneItemID ) )
                m_ScenarioItems[ scenario->ID() ].insert( zoneItemID, newItem );
        }
        else if ( FZoneItem* zoneItem = qobject_cast< FZoneItem *>( parent ) )
        {
            if( !m_SubSceneScenarioItems[ zoneItem->ID() ].contains( zoneItemID ) )
                m_SubSceneScenarioItems[ zoneItem->ID() ].insert( zoneItemID, newItem );
        }

        //
        //
        QJsonValueRef cablesref = jsonObject["cables"];
        if( cablesref.isArray() )
            createScenarioItemsFromJSON( scenario, newItem.get(), cablesref, FZoneItem::Cable );
    }
}

///
/// \brief FEnginePrivate::createSceneZone
/// Create a scene entry in the system with the specific zone info
/// While creating the scene zone item one needs to create the scenario items too corresponding to that zone
/// \param parent
/// \param zone
///
FZone* FEnginePrivate::createSceneZone(NESpatialGroup *parent, std::shared_ptr<FZone>& zone)
{
    if( zone == nullptr )
        return nullptr;

    NESCENE.setPropertiesInitialized(false);

    FZone* newZone = qobject_cast<FZone*>(NESCENE.findChildNode(zone->zoneName()));
    if (newZone == nullptr)
    {
        bool created;
        created = NESCENE.createObjectOnLoad("Fire Zone", zone->zoneName());
        if (!created)
        {
            qDebug() << "Failed creating zone " << zone->zoneName();
            return nullptr;
        }

        newZone = qobject_cast<FZone*>(NESCENE.findChildNode(zone->zoneName()));
    }

    newZone->setParent( parent );
    newZone->SetID( zone->ID() );
    newZone->SetConnectorID( zone->ConnectorID() );
    newZone->SetType( zone->Type() );
    newZone->SetDesc( zone->Desc() );
    newZone->SetIssues( zone->Issues() );
    newZone->setOriginalName(zone->originalName());
    newZone->setObjectName( zone->zoneName() );
    newZone->setZoneName( zone->zoneName() );
    newZone->setModel3DInfo( zone->model3DInfo() );
    newZone->SetCeilingThickness(zone->CeilingThickness());
    newZone->SetFloorThickness(zone->FloorThickness());
    newZone->SetWallThickness(zone->WallThickness());
    newZone->SetCeilingMaterial(zone->CeilingMaterial());
    newZone->SetFloorMaterial(zone->FloorMaterial());
    newZone->SetWallMaterial(zone->WallMaterial());
    newZone->SetWallLeakRatio(zone->WallLeakRatio());
    newZone->SetFloorLeakRatio(zone->FloorLeakRatio());

    //
    // Now add the dynamic properties which are present already as part of the DB
    //
    QList<QByteArray> b = zone->dynamicPropertyNames();

    for( int i = 0; i < b.size(); ++i )
    {
        QVariant v = zone->property( b[i] );
        newZone->addProperty( b[i], v, b[i] );
    }

    // Set the properties based on model3DInfo JSON String

    S->setPropertiesFromJSON( newZone, zone->model3DInfo() );

    NENodeIcon *nilist = newZone->factory().nodeIcon();
    NEIcon ni;
    ni.iconPath = newZone->Type();
    ni.iconSize = QSize( 16, 16 );
    nilist->push_back( ni );

    //
    // Create the zone items which are part of the zone
    //
    auto itr = m_ZoneItems.find( zone->ID() );
    if( itr != m_ZoneItems.end() )
    {
        auto& items = itr.value();
        auto itemsitr = items.begin();
        for( ; itemsitr != items.end(); ++itemsitr )
        {
            std::shared_ptr< FZoneItem >& item = itemsitr.value();
            if( item )
                createSceneZoneItem( newZone, item, newZone );
        }
    }

    //
    // Create the zone compartments which are part of the zone
    //
    if (m_ZoneCompartments.contains(zone->ID()))
    {
        for (auto& comp : m_ZoneCompartments.value(zone->ID()))
        {
            if (comp == nullptr)
                continue;

            newZone->addToItemNameTree(comp->compartmentName(), newZone->zoneName());

            FZoneCompartment* newComp = qobject_cast<FZoneCompartment*>(NESCENE.findChildNode(comp->compartmentName()));
            if (newComp == nullptr)
            {
                bool created;
                created = NESCENE.createObjectOnLoad("Fire Zone Boundary", comp->compartmentName());
                if (!created)
                {
                    qDebug() << "Failed creating zone compartment " << comp->compartmentName();
                    return nullptr;
                }

                newComp = qobject_cast<FZoneCompartment*>(NESCENE.findChildNode(comp->compartmentName()));
            }

            newComp->setParent(newZone);
            newComp->setOriginalName(comp->originalName());
            newComp->setCompartmentName(comp->compartmentName());
            newComp->setObjectName(comp->compartmentName());
            newComp->SetDesc(comp->Desc());
            newComp->SetID(comp->ID());
            newComp->SetType(comp->Type());
            newComp->setItemType(comp->itemType());
            newComp->setModel3DInfo(comp->model3DInfo());

            S->setPropertiesFromJSON(newComp, comp->model3DInfo());
        }
    }
    //
    // Create the zone vents which are part of the zone
    //
    if (m_ZoneVents.contains(zone->ID()))
    {
        for (auto& vent : m_ZoneVents.value(zone->ID()))
        {
            if (vent == nullptr)
                continue;

            newZone->addToItemNameTree(vent->name(), newZone->zoneName());

            FVent* newVent;
            if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
            {
                newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                if (newVent == nullptr)
                {
                    bool created;
                    created = NESCENE.createObjectOnLoad("Fire Mech Vent", mechVent->name());
                    if (!created)
                    {
                        qDebug() << "Failed creating mechanical vent " << mechVent->name();
                        return nullptr;
                    }
                }

                newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
            }
            else
            {
                newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                if (newVent == nullptr)
                {
                    bool created;
                    created = NESCENE.createObjectOnLoad("Fire Vent", vent->name());
                    if (!created)
                    {
                        qDebug() << "Failed creating vent " << vent->name();
                        return nullptr;
                    }
                }

                newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
            }

            newVent->setParent(newZone);
            newVent->setOriginalName(vent->originalName());
            newVent->setName(vent->name());
            newVent->setObjectName(vent->name());
            newVent->SetDesc(vent->Desc());
            newVent->SetIssues(vent->Issues());
            newVent->SetID(vent->ID());
            newVent->SetType(vent->Type());
            newVent->setItemType(vent->itemType());
            newVent->SetFirstCompartment(vent->FirstCompartment());
            newVent->SetSecondCompartment(vent->SecondCompartment());
            newVent->setFirstCompartmentId(vent->firstCompartmentId());
            newVent->setSecondCompartmentId(vent->secondCompartmentId());
            newVent->SetCriterion(vent->Criterion());

            if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newVent))
                {
                    newMechVent->SetFlowRate(mechVent->FlowRate());
                    newMechVent->SetBeginDropOff(mechVent->BeginDropOff());
                    newMechVent->SetZeroAt(mechVent->ZeroAt());
                    newMechVent->SetFilterEfficiency(mechVent->FilterEfficiency());
                    newMechVent->SetFilterStart(mechVent->FilterStart());
                }

            newVent->setModel3DInfo(vent->model3DInfo());
            S->setPropertiesFromJSON(newVent, vent->model3DInfo());
        }
    }

    //
    // Now create the scenarios which are associated with this zone only if its present in the zone
    // associations.
    //
    zone->scenarios() = GetScenarioListForZone( zone->ID() );

    if( zone->scenarios().size() > 0 )
    {
        QString scnName = zone->scenarios().at(0);

        FScenario* newScn = qobject_cast<FScenario*>(NESCENE.findChildNode(scnName));
        if (newScn == nullptr)
        {
            bool created;
            created = NESCENE.createObjectOnLoad("Fire Scenario", scnName);
            if (!created)
            {
                qDebug() << "Failed creating scenario " << scnName;
                return nullptr;
            }

            newScn = qobject_cast<FScenario*>(NESCENE.findChildNode(scnName));
        }

        std::shared_ptr< FScenario > scn = createScenario( scnName, "" );
        newScn->setParent( S );
        newScn->SetScenarioFile( scn->ScenarioFile() );
        newScn->SetID( scn->ID() );
        newScn->SetConnectorID( scn->ConnectorID() );
        newScn->SetType( scn->Type() );
        newScn->SetDesc( scn->Desc() );
        newScn->SetIssues( scn->Issues() );
        newScn->setOriginalName(scn->originalName());
        newScn->setObjectName( scn->scenarioName() );
        newScn->setScenarioName( scn->scenarioName() );
        newScn->setModel3DInfo( scn->model3DInfo() );

        //
        // Now add the dynamic properties which are present already as part of the DB
        //
        QList<QByteArray> b = scn->dynamicPropertyNames();
        for( int i = 0; i < b.size(); ++i )
        {
            QVariant v = scn->property( b[i] );
            newScn->addProperty( b[i], v, b[i] );
        }

        // Set the properties based on model3DInfo JSON String

        S->setPropertiesFromJSON( newScn, scn->model3DInfo() );

        //
        // Set the Icons
        //
        NENodeIcon *nilist = newScn->factory().nodeIcon();
        NEIcon ni;
        ni.iconPath = newScn->Type();
        ni.iconSize = QSize( 16, 16 );
        nilist->push_back( ni );

        if( !newScn->zones().contains( zone->zoneName() ) )
            newScn->zones().push_back( zone->zoneName() );

        //
        // Now create the scenario items which are part of this scenario -
        // now they might already exist in the scene
        // Create the sub scenario items only if their local entry exists in m_ScenarioItems
        //
        auto sititr = m_ScenarioItems.find( scn->ID() );
        if( sititr != m_ScenarioItems.end() )
        {
            auto& items = sititr.value();
            auto itemsitr = items.begin();
            for( ; itemsitr != items.end(); ++itemsitr )
            {
                std::shared_ptr< FZoneItem >& item = itemsitr.value();
                createSceneScenarioItem( newScn, item );
            }
        }

        if (m_ScenarioVents.contains(scn->ID()))
        {
            for (auto& vent : m_ScenarioVents.value(scn->ID()))
            {
                newScn->addToItemNameTree(vent->name(), newScn->scenarioName());

                FVent* newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                if (newVent != nullptr)
                    continue;

                if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                {
                    newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                    if (newVent == nullptr)
                    {
                        bool created;
                        created = NESCENE.createObjectOnLoad("Fire Mech Vent", mechVent->name());
                        if (!created)
                        {
                            qDebug() << "Failed creating mechanical vent " << mechVent->name();
                            return nullptr;
                        }
                    }

                    newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                }
                else
                {
                    newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                    if (newVent == nullptr)
                    {
                        bool created;
                        created = NESCENE.createObjectOnLoad("Fire Vent", vent->name());
                        if (!created)
                        {
                            qDebug() << "Failed creating vent " << vent->name();
                            return nullptr;
                        }
                    }

                    newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                }

                newVent->setParent(newScn);
                newVent->setOriginalName(vent->originalName());
                newVent->setName(vent->name());
                newVent->setObjectName(vent->name());
                newVent->SetDesc(vent->Desc());
                newVent->SetIssues(vent->Issues());
                newVent->SetID(vent->ID());
                newVent->SetType(vent->Type());
                newVent->setItemType(vent->itemType());
                newVent->SetFirstCompartment(vent->FirstCompartment());
                newVent->SetSecondCompartment(vent->SecondCompartment());
                newVent->setFirstCompartmentId(vent->firstCompartmentId());
                newVent->setSecondCompartmentId(vent->secondCompartmentId());
                newVent->SetCriterion(vent->Criterion());

                if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                    if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newVent))
                    {
                        newMechVent->SetFlowRate(mechVent->FlowRate());
                        newMechVent->SetBeginDropOff(mechVent->BeginDropOff());
                        newMechVent->SetZeroAt(mechVent->ZeroAt());
                        newMechVent->SetFilterEfficiency(mechVent->FilterEfficiency());
                        newMechVent->SetFilterStart(mechVent->FilterStart());
                    }

                newVent->setModel3DInfo(vent->model3DInfo());
                S->setPropertiesFromJSON(newVent, vent->model3DInfo());
            }
        }

        //
        // Set the active Scenario to be this one which is currently the first one by default associated
        // with this zone
        FEngine *curengine = S;

        m_activeScenarioName = newScn->scenarioName();
        emit S->activeScenario((QObject* &)curengine, (QObject* &)newScn);

        //
        // Get the list of other scenarios associated with this zone and pass it on to
        // scenario inspection as a signal
        //
        QList<QString> otherzonescnnames = GetScenarioListForZone( zone->ID() );
        //
        // emit a signal so that the  editor can load the associated scenarios to this one
        //
        emit S->relatedScenarios( otherzonescnnames );

        //
        // Get the list of other zones associated with the scenario related to this zone
        //
        QList<QString> otherscnzonenames = GetZoneListForScenario( scn->ID() );

        QString franxtree = GetFRANXTreeDB( newZone->ID(), newScn->ID() );

        QJsonDocument jsonResponse = QJsonDocument::fromJson( franxtree.toUtf8() );
        if(jsonResponse.isNull()){
            std::cerr << "FRANX Tree is invalid." << std::endl;
        }
        if(jsonResponse.isObject()){
            QJsonObject jsonObject = jsonResponse.object();
            createFRANXZoneItems(newZone, jsonObject);
        }
        else if(jsonResponse.isArray())
        {
            QJsonArray jsonArray = jsonResponse.array();

            int jsonSize = jsonArray.size();

            for( int i = 0; i < jsonSize; ++i )
            {
                QJsonValueRef r = jsonArray[i];

                QJsonObject   jsonObject = r.toObject();

                createFRANXZoneItems(newZone, jsonObject);
            }
        }
        emit S->relatedZones( otherscnzonenames );
    }

    NESCENE.setPropertiesInitialized(true);

    updateItemsWithCompartmentInfo();
    updateVentsWithZoneInfo();

    return newZone;
}

///
/// \brief FEnginePrivate::createSceneScnItem
/// \param parent
/// \param item
///
void FEnginePrivate::createSceneScnItem(FScenario *scenario,
                                        NESpatialGroup *parent,
                                        std::shared_ptr< FZoneItem >& item)
{
    if( qobject_cast< FScenario* >( parent ) )
        scenario->addToItemNameTree(item->zoneItemName(), scenario->scenarioName());
    else if( FZoneItem* parentZoneItem = qobject_cast< FZoneItem* >( parent ) )
        scenario->addToItemNameTree(item->zoneItemName(), parentZoneItem->zoneItemName());

    FZoneItem* newItem = qobject_cast<FZoneItem*>(NESCENE.findChildNode(item->zoneItemName()));

    if (newItem == nullptr)
    {
        bool created;
        if (qobject_cast<FFire*>(item.get()))
            created = NESCENE.createObjectOnLoad("Fire Definition", item->zoneItemName());
        else if (qobject_cast<FCables*>(item.get()))
            created = NESCENE.createObjectOnLoad("Cables", item->zoneItemName());
        else
            created = NESCENE.createObjectOnLoad("Fire Plant Item", item->zoneItemName());
        if (!created)
        {
            qDebug() << "Failed creating item " << item->zoneItemName();
            return;
        }

        newItem = qobject_cast<FZoneItem*>(NESCENE.findChildNode(item->zoneItemName()));

        newItem->setParent( parent );
        newItem->SetDesc( item->Desc() );
        newItem->SetID( item->ID() );
        newItem->SetConnectorID( item->ConnectorID() );
        newItem->SetIssues( item->Issues() );
        newItem->setOriginalName(item->originalName());
        newItem->setZoneItemName( item->zoneItemName() );
        newItem->setObjectName( item->zoneItemName() );
        newItem->SetType( item->Type() );
        newItem->SetItemType( item->ItemType() );
        newItem->SetCompartmentID( item->CompartmentID() );
//        if( newItem->Type() == "foCable" || newItem->Type() == "foRaceway" ||
//                newItem->Type() == "foComponent" )
//        {
        newItem->SetMaterial( item->Material() );
        newItem->SetFailure( item->Failure() );
//        }
//        else
//        {
//            newItem->hideProperty( "Material", true );
//            newItem->hideProperty( "Failure", true );
//        }

        if( FFire *fireItem = qobject_cast< FFire *>( item.get() ))
        {
            if (FFire *newFireItem = qobject_cast<FFire*>(newItem))
                newFireItem->SetFireMaterial(fireItem->FireMaterial());
        }

        if (FCables* cable = qobject_cast<FCables*>(item.get()))
        {
            if (FCables* newCable = qobject_cast<FCables*>(newItem))
                newCable->SetCableMaterial(cable->CableMaterial());
        }
    }

    //newItem->SetPlantItemType( item->PlantItemType() );
    newItem->setPlantItemType( m_plantItemLib[ item->ItemType() ] );

    newItem->setModel3DInfo( item->model3DInfo() );

    NENodeIcon *nilist = newItem->factory().nodeIcon();
    NEIcon ni;
    ni.iconPath = newItem->Type();
    ni.iconSize = QSize( 16, 16 );
    nilist->clear();
    nilist->push_back( ni );

    // Set the properties based on model3DInfo JSON String

    S->setPropertiesFromJSON( newItem, item->model3DInfo() );

    auto& items = m_SubSceneScenarioItems[ item->ID() ];
    auto ssitr = items.begin();
    for( ; ssitr != items.end(); ++ssitr )
    {
        std::shared_ptr< FZoneItem > ssitem = ssitr.value();
        createSceneScnItem( scenario, newItem, ssitem );
    }
}

///
/// \brief FEnginePrivate::createSceneZoneItem
/// \param parent
/// \param item
///

void FEnginePrivate::createSceneZoneItem(NESpatialGroup *parent, std::shared_ptr< FZoneItem >& item, FZone* zone)
{
    if( FZone* parentZone = qobject_cast< FZone* >( parent ) )
        zone->addToItemNameTree(item->zoneItemName(), parentZone->zoneName());
    else if( FZoneItem* parentZoneItem = qobject_cast< FZoneItem* >( parent ) )
        zone->addToItemNameTree(item->zoneItemName(), parentZoneItem->zoneItemName());

    FZoneItem* newItem = qobject_cast<FZoneItem*>(NESCENE.findChildNode(item->zoneItemName()));

    if (newItem == nullptr)
    {
        bool created;
        if (qobject_cast<FFire*>(item.get()))
            created = NESCENE.createObjectOnLoad("Fire Definition", item->zoneItemName());
        else if (qobject_cast<FCables*>(item.get()))
            created = NESCENE.createObjectOnLoad("Cables", item->zoneItemName());
        else
            created = NESCENE.createObjectOnLoad("Fire Plant Item", item->zoneItemName());
        if (!created)
        {
            qDebug() << "Failed creating item " << item->zoneItemName();
            return;
        }

        newItem = qobject_cast<FZoneItem*>(NESCENE.findChildNode(item->zoneItemName()));

        newItem->setParent( parent );
        newItem->SetDesc( item->Desc() );
        newItem->SetID( item->ID() );
        newItem->SetConnectorID( item->ConnectorID() );
        newItem->SetIssues( item->Issues() );
        newItem->setOriginalName(item->originalName());
        newItem->setZoneItemName( item->zoneItemName() );
        newItem->setObjectName( item->zoneItemName() );
        newItem->SetType( item->Type() );
        newItem->SetItemType( item->ItemType() );
        newItem->setPlantItemType( m_plantItemLib[ item->ItemType() ] );
        newItem->SetCompartmentID( item->CompartmentID() );
//        if( newItem->Type() == "foCable" || newItem->Type() == "foRaceway" ||
//                newItem->Type() == "foComponent" )
//        {
        newItem->SetMaterial( item->Material() );
        newItem->SetFailure( item->Failure() );
//        }
//        else
//        {
//            newItem->hideProperty( "Material", true );
//            newItem->hideProperty( "Failure", true );
//        }

        if( FFire *fireItem = qobject_cast< FFire *>( item.get() ))
        {
            if (FFire *newFireItem = qobject_cast<FFire*>(newItem))
                newFireItem->SetFireMaterial(fireItem->FireMaterial());
        }

        if (FCables* cable = qobject_cast<FCables*>(item.get()))
        {
            if (FCables* newCable = qobject_cast<FCables*>(newItem))
                newCable->SetCableMaterial(cable->CableMaterial());
        }

    }
    //newItem->SetPlantItemType( item->PlantItemType() );

    newItem->setModel3DInfo( item->model3DInfo() );

    NENodeIcon *nilist = newItem->factory().nodeIcon();
    NEIcon ni;
    ni.iconPath = newItem->Type();
    ni.iconSize = QSize( 16, 16 );
    nilist->clear();
    nilist->push_back( ni );

    // Set the properties based on model3DInfo JSON String

    S->setPropertiesFromJSON( newItem, item->model3DInfo() );

    auto& items = m_SubSceneZoneItems[ item->ID() ];
    auto ssitr = items.begin();
    for( ; ssitr != items.end(); ++ssitr )
    {
        std::shared_ptr< FZoneItem > ssitem = ssitr.value();
        createSceneZoneItem( newItem, ssitem, zone );
    }
}

FZoneItem *FEnginePrivate::createNewFRANXItems(NESpatialGroup *parent, QJsonObject &jObj)
{
    QString newName(jObj["cleanName"].toString());
    newName.replace("&", "__");
    newName.replace(":", "--");

    FZoneItem *newItem = qobject_cast< FZoneItem *>( NESCENE.createObject( "Fire Plant Item", newName ) );
    if( newItem == NULL )
    {
        qDebug() << "Cant Create Item " << newName;
        return NULL;
    }
    newItem->setParent( parent );

    newItem->SetDesc( jObj["desc"].toString() );
    newItem->SetConnectorID( jObj["connectorID"].toInt() );
    newItem->SetID( newItem->ConnectorID() );
    newItem->SetIssues( jObj["issues"].toString() );
    newItem->setOriginalName(jObj["name"].toString());
    newItem->setZoneItemName(newName );
    newItem->setObjectName( newName );
    newItem->SetType( jObj["type"].toString() );

    //newItem->SetItemType( item->ItemType() );
    //newItem->SetPlantItemType( m_plantItemLib[ item->ItemType() ] );
    //newItem->setModel3DInfo( item->model3DInfo() );

    NENodeIcon *nilist = newItem->factory().nodeIcon();
    NEIcon ni;
    ni.iconPath = newItem->Type();
    ni.iconSize = QSize( 16, 16 );
    nilist->clear();
    nilist->push_back( ni );

    // Set the properties based on model3DInfo JSON String

    //setPropertiesFromJSON( newItem, item->model3DInfo() );


    QObject *testObj = NESCENE.findChildNode(newName);
    return newItem;
}


///
/// \brief FEnginePrivate::createFRANXZoneItems
/// \param newZone
/// \param jsonObject
///

void FEnginePrivate::createFRANXZoneItems( NESpatialGroup *parent, QJsonObject& jsonObject )
{
    if(FZone *parentZone = qobject_cast<FZone *>(parent))
    {
        QJsonArray jArray = jsonObject["raceways"].toArray();
        int size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZone->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem )
                {
                    fItem = createNewFRANXItems(parentZone, jObj);
                }
                parentZone->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);

            }
        }

        jArray = jsonObject["cables"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZone->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem )
                {
                    fItem = createNewFRANXItems(parentZone, jObj);
                }
                parentZone->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);

            }
        }

        jArray = jsonObject["components"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZone->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem ){
                    fItem = createNewFRANXItems(parentZone, jObj);
                }
                parentZone->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);

            }
        }

        jArray = jsonObject["basicEvents"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZone->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem ){
                    fItem = createNewFRANXItems(parentZone, jObj);
                }
                parentZone->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);

            }
        }
    }
    else if(FZoneItem *parentZoneItem = qobject_cast<FZoneItem *>(parent)){
        QJsonArray jArray = jsonObject["raceways"].toArray();
        int size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZoneItem->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem )
                {
                    fItem = createNewFRANXItems(parentZoneItem, jObj);
                }
                parentZoneItem->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);
            }
        }

        jArray = jsonObject["cables"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZoneItem->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem ){
                    fItem = createNewFRANXItems(parentZoneItem, jObj);
                }
                parentZoneItem->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);
            }
        }

        jArray = jsonObject["components"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++)
        {
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZoneItem->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem ){
                    fItem = createNewFRANXItems(parentZoneItem, jObj);
                }
                parentZoneItem->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);

            }
        }

        jArray = jsonObject["basicEvents"].toArray();
        size = jArray.size();
        for (int i = 0; i < size; i++){
            QJsonObject jObj = jArray[i].toObject();
            QString franxName(jObj["cleanName"].toString());
            franxName.replace(":", "--");
            if(!parentZoneItem->otherItems().contains(franxName))
            {
                FZoneItem *fItem = qobject_cast<FZoneItem *>(NESCENE.findChildNode(franxName));
                if( !fItem ){
                    fItem = createNewFRANXItems(parentZoneItem, jObj);
                }
                parentZoneItem->otherItems().push_back(franxName);
                createFRANXZoneItems(fItem, jObj);
            }
        }
    }
}

void FEnginePrivate::createFRANXScenarioItems(std::shared_ptr<NESpatialGroup> zone, QJsonObject &jsonObject)
{

}

///
/// \brief FEnginePrivate::createZone
/// \param scn
/// \param zoneName
/// \return
///
std::shared_ptr< FZone > FEnginePrivate::createZone(QString zoneName,
                                                    QString jsonData)
{
    QString allZoneJsonData;
    if( jsonData == "" )
        allZoneJsonData = GetZonesDB();
    else
        allZoneJsonData = jsonData;

    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( allZoneJsonData.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return nullptr;
    }

    QJsonArray jsonArray = jsonResponse.array();

    int jsonSize = jsonArray.size();
    std::shared_ptr< FZone > newZone;

    if( zoneName == "" )
    {
        QJsonValueRef r = jsonArray[0];
        QJsonObject   jsonObject = r.toObject();

        int     zoneID   = jsonObject["connectorID"].toInt();

        QString zString = GetPlantZoneTreeDB( zoneID );

        newZone = createZoneFromJSON( zString );
    }
    else
    {
        for( int i = 0; i < jsonSize; ++i )
        {
            QJsonValueRef r = jsonArray[i];

            QJsonObject   jsonObject = r.toObject();

            QString zname    = jsonObject["cleanName"].toString();

            int     zoneID   = jsonObject["connectorID"].toInt();

            if( zname == zoneName )
            {
                QString zString = GetPlantZoneTreeDB( zoneID );
                newZone = createZoneFromJSON( zString );
                break;
            }
        }
    }

    return newZone;
}

///
/// \brief FEnginePrivate::createZone
/// \param jsonData
///
std::shared_ptr< FZone > FEnginePrivate::createZoneFromJSON(QString jsonData)
{
    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( jsonData.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return nullptr;
    }

    QJsonObject   jsonObject = jsonResponse.object();

    QString originalName = jsonObject["name"].toString();
    QString name        = jsonObject["cleanName"].toString();

    QString desc        = jsonObject["desc"].toString();
    QString ztype       = jsonObject["type"].toString();
    uint connectorID    = jsonObject["connectorID"].toInt();
    uint id             = connectorID;
    QString issues      = jsonObject["issues"].toString();
    QJsonArray model3DInfo = jsonObject["model3DInfo"].toArray();
    QJsonObject simItemsJsonObject = jsonObject["simulationItems"].toObject();

    QJsonObject jsonFields = jsonObject["jsonFields"].toObject();

    uint wallMaterialID = jsonFields["wallMaterialID"].toInt();
    uint ceilingMaterialID = jsonFields["ceilingMaterialID"].toInt();
    uint floorMaterialID = jsonFields["floorMaterialID"].toInt();
    double floorLeakRatio = jsonFields["floorLeakRatio"].toDouble();
    double wallLeakRatio = jsonFields["wallLeakRatio"].toDouble();


    static NESurfaceMaterial wallMat, ceilingMat, floorMat;
    bool wallMatFound = false, ceilingMatFound = false, floorMatFound = false;
    for (int i = 0; i < m_SurfaceMaterials.size(); i++)
    {
        if (!wallMatFound && m_SurfaceMaterials[i].uniqueID() == wallMaterialID)
        {
            wallMat = m_SurfaceMaterials[i];
            wallMatFound = true;
        }
        if (!ceilingMatFound && m_SurfaceMaterials[i].uniqueID() == ceilingMaterialID)
        {
            ceilingMat = m_SurfaceMaterials[i];
            ceilingMatFound = true;
        }
        if (!floorMatFound && m_SurfaceMaterials[i].uniqueID() == floorMaterialID)
        {
            floorMat = m_SurfaceMaterials[i];
            floorMatFound = true;
        }
        if (wallMatFound && ceilingMatFound && floorMatFound)
            break;
    }
    for (int i = 0; i < m_DefaultSurfaceMaterials.size(); i++)
    {
        if (!wallMatFound && m_DefaultSurfaceMaterials[i].uniqueID() == wallMaterialID && wallMaterialID != 0 )
        {
            wallMat = m_DefaultSurfaceMaterials[i];
            if (!m_DefaultSurfaceMaterialAssignedIndexes.contains(i))
                m_DefaultSurfaceMaterialAssignedIndexes.push_back(i);
            wallMatFound = true;
        }
        if (!ceilingMatFound && m_DefaultSurfaceMaterials[i].uniqueID() == ceilingMaterialID && ceilingMaterialID != 0 )
        {
            ceilingMat = m_DefaultSurfaceMaterials[i];
            if (!m_DefaultSurfaceMaterialAssignedIndexes.contains(i))
                m_DefaultSurfaceMaterialAssignedIndexes.push_back(i);
            ceilingMatFound = true;
        }
        if (!floorMatFound && m_DefaultSurfaceMaterials[i].uniqueID() == floorMaterialID && floorMaterialID != 0 )
        {
            floorMat = m_DefaultSurfaceMaterials[i];
            if (!m_DefaultSurfaceMaterialAssignedIndexes.contains(i))
                m_DefaultSurfaceMaterialAssignedIndexes.push_back(i);
            floorMatFound = true;
        }
        if (wallMatFound && ceilingMatFound && floorMatFound)
            break;
    }
    if (!wallMatFound)
    {
        wallMat.setName("Null");
        wallMat.setUniqueID(0);
        wallMat.setIndex(0);
    }
    if (!ceilingMatFound)
    {
        ceilingMat.setName("Null");
        ceilingMat.setUniqueID(0);
        ceilingMat.setIndex(0);
    }
    if (!floorMatFound)
    {
        floorMat.setName("Null");
        floorMat.setUniqueID(0);
        floorMat.setIndex(0);
    }

    std::shared_ptr< FZone >newZone = std::shared_ptr< FZone >( new FZone );
    //newZone->setParent( S );
    //newZone->SetZoneFile( modelFilePath );
    newZone->SetID( id );
    newZone->SetConnectorID( connectorID );
    newZone->SetType( ztype );
    newZone->SetDesc( desc );
    newZone->SetIssues( issues );
    newZone->setOriginalName(originalName);
    newZone->setZoneName( name );
    newZone->SetWallMaterial(wallMat);
    newZone->SetCeilingMaterial(ceilingMat);
    newZone->SetFloorMaterial(floorMat);
    newZone->SetFloorLeakRatio(floorLeakRatio);
    newZone->SetWallLeakRatio(wallLeakRatio);

    QJsonDocument doc;
    doc.setArray( model3DInfo );
    QString dataToString( doc.toJson() );
    newZone->setModel3DInfo( dataToString );

    m_VentCriteriaList = S->getVentCriteria(id);
    emit S->sendUpdateVentCriterias( m_VentCriteriaList );

    createZoneItems( newZone, jsonObject );
    createZoneProperties( newZone, jsonObject );

    createSimItemsFromJson(newZone, simItemsJsonObject);

    return newZone;
}

QString FEnginePrivate::createJSONFromSceneZone(FZone *zone)
{
    QJsonObject   jsonObject ;

    int connectorID = zone->ConnectorID();
    //int id = connectorID;

    jsonObject["name"] = zone->originalName();
    jsonObject["desc"] = zone->Desc();
    jsonObject["type"] = zone->Type();
    jsonObject["connectorID"] = connectorID;
    //jsonObject["ID"]    = id;
    jsonObject["issues"] = zone->Issues();
    jsonObject["wallLeakRatio"] = zone->WallLeakRatio();
    jsonObject["floorLeakRatio"] = zone->FloorLeakRatio();

    QJsonObject jsonFields;
    // Do not save the assigment to null material and if material has id = 0
    const NESurfaceMaterial& wm = zone->WallMaterial();
    if (wm.name() != "Null" && wm.index() != 0 && wm.uniqueID() > 0)
        jsonFields["wallMaterialID"] = wm.uniqueID();
    else
        jsonFields["wallMaterialID"] = 0;
    const NESurfaceMaterial& cm = zone->CeilingMaterial();
    if (cm.name() != "Null" && cm.index() != 0 && cm.uniqueID() > 0)
        jsonFields["ceilingMaterialID"] = cm.uniqueID();
    else
        jsonFields["ceilingMaterialID"] = 0;
    const NESurfaceMaterial& fm = zone->FloorMaterial();
    if (fm.name() != "Null" && fm.index() != 0 && fm.uniqueID() > 0)
        jsonFields["floorMaterialID"] = fm.uniqueID();
    else
        jsonFields["floorMaterialID"] = 0;
    jsonObject["jsonFields"] = jsonFields;

    jsonObject["model3DInfo"] = S->getJSONFromProperties( zone );

    QJsonDocument jsonDoc( jsonObject );
    QString jsonStr( jsonDoc.toJson() );


    return jsonStr;
}

QString FEnginePrivate::createJSONFromSceneItemName(QString zoneItemName)
{
    QString retJson;

    FZoneItem* zitem = qobject_cast< FZoneItem *>( NESCENE.findChildNode( zoneItemName ) );
    if( zitem )
    {
        retJson = createJSONFromSceneItem( zitem );
    }

    return retJson;
}

///
/// \brief FEnginePrivate::createJSONFromSceneItem
/// \param zoneItem
/// \return
///

QString FEnginePrivate::createJSONFromSceneItem(FZoneItem *zoneItem)
{
    QJsonObject   jsonObject ;
    int connectorID = zoneItem->ConnectorID();

    jsonObject["name"] = zoneItem->originalName();
    jsonObject["desc"] = zoneItem->Desc();
    jsonObject["type"] = zoneItem->Type();
    jsonObject["connectorID"] = connectorID;
    jsonObject["issues"] = zoneItem->Issues();
    jsonObject["pItemType"] = zoneItem->PlantItemType();
    if( zoneItem->SpatialMeshType() != NESpatialMesh::None )
        jsonObject["model3DInfo"] = S->getJSONFromProperties( zoneItem );
    else
        jsonObject["model3DInfo"] = QJsonArray();

    QJsonObject jsonFields;
    QJsonObject cfast;
    jsonFields["cfast"] = cfast;
    jsonObject["jsonFields"] = jsonFields;

    if( FFire* fireItem = qobject_cast< FFire *>( zoneItem ) )
    {
        if( FZone* parentZone = qobject_cast< FZone *>( zoneItem->parent() ) )
        {
            int zid = parentZone->ConnectorID();
            jsonObject["zone"] = zid;
        }
        else
            jsonObject["zone"] = 0;

        NEFireMaterial fm = fireItem->FireMaterial();
        // Do not save the assigment to null material and if material has id = 0
        if (fm.name() != "Null" && fm.index() != 0 && fm.uniqueID() > 0)
            jsonObject["fireMaterialID"] = fm.uniqueID();
        else
            jsonObject["fireMaterialID"] = 0;
    }

    if (FCables* cable = qobject_cast<FCables*>(zoneItem))
    {
        NECableProperty cm = cable->CableMaterial();
        // Do not save the assigment to null material and if material has id = 0
        if (cm.name() != "Null" && cm.index() != 0 && cm.uniqueID() > 0)
            jsonObject["cableSpecsID"] = cm.uniqueID();
        else
            jsonObject["cableSpecsID"] = 0;
    }

    NESurfaceMaterial sm = zoneItem->Material();
    // Do not save the assigment to null material and if material has id = 0
    if (sm.name() != "Null" && sm.index() != 0 && sm.uniqueID() > 0)
        jsonObject["materialID"] = sm.uniqueID();
    else
        jsonObject["materialID"] = 0;

    NEFailureCriteria fail = zoneItem->Failure();
    // Do not save the assigment to null failure criteria and if id = 0
    if (fail.name() != "Null" && fail.index() != 0 && fail.ID() > 0)
        jsonObject["failureCriteriaID"] = fail.ID();
    else
        jsonObject["failureCriteriaID"] = 0;

    cfast["compartmentID"] = static_cast<int>(zoneItem->CompartmentID());

    QJsonDocument jsonDoc( jsonObject );
    QString jsonStr( jsonDoc.toJson() );

    return jsonStr;
}

///
/// \brief FEnginePrivate::createJSONFromCompartment
/// \param compartmentName
/// \return
QString FEnginePrivate::createJSONFromCompartment(FZoneCompartment* compartment)
{
    QJsonObject jsonObject;

    jsonObject["name"] = compartment->originalName();
    jsonObject["desc"] = compartment->Desc();
    jsonObject["type"] = compartment->Type();
    jsonObject["connectorID"] = static_cast<int>(compartment->ID());

    jsonObject["model3DInfo"] = S->getJSONFromProperties(compartment);

    QJsonDocument jsonDoc(jsonObject);
    QString jsonStr(jsonDoc.toJson());

    return jsonStr;
}

QString FEnginePrivate::createJSONFromVent(FVent* vent)
{
    QJsonObject jsonObject;

    jsonObject["name"] = vent->originalName();
    jsonObject["desc"] = vent->Desc();
    jsonObject["issues"] = vent->Issues();
    jsonObject["connectorID"] = static_cast<int>(vent->ID());
    if (qobject_cast<FMechVent*>(vent))
    {
        jsonObject["itemType"] = "siMechVent";
        jsonObject["type"] = "foSimulationItem";
    } else
    {
        jsonObject["itemType"] = "siVent";
        jsonObject["type"] = "foSimulationItem";
    }

    std::pair<int, int> compIds(0, 0);
    for (int i = 0; i != m_ZoneIdNamePairs.size(); i++)
    {
        if (m_ZoneIdNamePairs.at(i).second == vent->FirstCompartment().curName())
            compIds.first = m_ZoneIdNamePairs.at(i).first;
        if (m_ZoneIdNamePairs.at(i).second == vent->SecondCompartment().curName())
            compIds.second = m_ZoneIdNamePairs.at(i).first;
    }

    QJsonObject jsonFields;
    if (vent->FirstCompartment().curName() != "Outside" && compIds.first != 0)
        jsonFields["firstTopFromCompartment"] = compIds.first;
    else
        jsonFields["firstTopFromCompartment"] = 0;
    if (vent->SecondCompartment().curName() != "Outside" && compIds.second != 0)
        jsonFields["secondBottomToCompartment"] = compIds.second;
    else
        jsonFields["secondBottomToCompartment"] = 0;

    if (vent->Criterion().name() != "Null" && vent->Criterion().index() != 0 && vent->Criterion().ID() > 0)
        jsonFields["criterionID"] = vent->Criterion().ID();
    else
        jsonFields["criterionID"] = 0;

    if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent))
    {
        jsonFields["flowRate"] = mechVent->FlowRate();
        jsonFields["beginDropOff"] = mechVent->BeginDropOff();
        jsonFields["zeroFlow"] = mechVent->ZeroAt();
        jsonFields["beginFilter"] = mechVent->FilterStart();
        jsonFields["filterEfficiency"] = mechVent->FilterEfficiency();
    }

    jsonFields["cfast"] = QJsonObject();
    jsonFields["fds"] = QJsonObject();
    jsonObject["jsonFields"] = jsonFields;

    jsonObject["model3DInfo"] = S->getJSONFromProperties(vent);

    QString jsonStr(QJsonDocument(jsonObject).toJson());

    return jsonStr;
}

///
/// \brief FEnginePrivate::createJSONFromScenario
/// \param scn
/// \return
///

QString FEnginePrivate::createJSONFromScenario(FScenario* scn)
{
    QJsonObject   jsonObject ;

    int connectorID = scn->ConnectorID();
    int id = connectorID;

    jsonObject["name"] = scn->originalName();
    jsonObject["desc"] = scn->Desc();
    jsonObject["type"] = scn->Type();
    jsonObject["connectorID"] = connectorID;
    //jsonObject["ID"]    = id;
    jsonObject["issues"] = scn->Issues();

    QJsonDocument jsonDoc( jsonObject );

    QString jsonStr( jsonDoc.toJson() );

    return jsonStr;
}


///
/// \brief FEnginePrivate::createJSONFromScenario
/// \param scn
/// \return
///

QString FEnginePrivate::createJSONFromSceneScenario(FScenario* scn)
{
    QJsonObject   jsonObject ;

    int connectorID = scn->ConnectorID();
    //int id = connectorID;

    jsonObject["name"] = scn->originalName();
    jsonObject["desc"] = scn->Desc();
    jsonObject["type"] = scn->Type();
    jsonObject["connectorID"] = connectorID;
    //jsonObject["ID"]    = id;
    jsonObject["issues"] = scn->Issues();

    QJsonDocument jsonDoc( jsonObject );


    jsonObject["model3DInfo"] = S->getJSONFromProperties( scn );
    QString jsonStr( jsonDoc.toJson() );

    // setPropertiesFromJson( QString model3DInfoStr );

    return jsonStr;
}

///
/// \brief FEnginePrivate::createSpatialProperties
/// \param spatial
/// \param jsonObject
///

void FEnginePrivate::createSpatialProperties( std::shared_ptr< NESpatial > spatial, QJsonObject &jsonObject )
{
    QJsonValueRef v =  jsonObject["FRANX Properties"];
    if( !v.isObject() )
        return;

    QJsonObject jsoncObject    = v.toObject();

    QStringList propkeylist = jsoncObject.keys();
    for( int i = 0; i < propkeylist.size(); ++i )
    {
        QJsonValueRef propval = jsoncObject[propkeylist[i]];
        QString propstr = propkeylist[i];
        propstr.replace( 0, 1, propstr[0].toUpper() );
        spatial->addProperty( propkeylist[i], propval.toVariant(), propstr );
#if 0
        QJsonValue::Type t = propval.type();
        switch( t )
        {
        case QJsonValue::Bool:
            spatial->addProperty( propkeylist[i], propval.toBool() );
        case QJsonValue::String:
            spatial->addProperty( propkeylist[i], propval.toString() );
        case QJsonValue::Double:
            spatial->addProperty( propkeylist[i], propval.toDouble() );
            break;
        }
#endif
    }
}

///
/// \brief FEnginePrivate::createZoneProperties
/// \param zone
/// \param jsonObject
///

void FEnginePrivate::createZoneProperties(std::shared_ptr<FZone> zone, QJsonObject &jsonObject)
{
    createSpatialProperties( zone, jsonObject );
}

///
/// \brief FEnginePrivate::loadFRANXFile
/// \param filePath
/// \return
///

uint FEnginePrivate::loadFRANXFile(const QString& filePath )
{
    std::string fstr = filePath.toStdString();
    char* dbfilestr = const_cast< char *>( fstr.c_str() );

    QFileInfo dbFileInfo( filePath );
    //m_DBFile = filePath;
    QDir dbFileDir = dbFileInfo.absoluteDir();

    //
    // Make the new DB Folder
    //
    QString newDBName = dbFileDir.absolutePath() + "/" + dbFileInfo.baseName();
    QDir dbFileProjectDir( newDBName );
    dbFileProjectDir.mkdir( newDBName );

    //
    // Convert the database
    //
    std::string newDBstr = newDBName.toStdString();
    char* newdbcstr = const_cast< char *>( newDBstr.c_str() );


    QProcess P(S);
    QString dbConvertProgramPath;
    dbConvertProgramPath =  QStandardPaths::findExecutable( "ImportFRANX" );

    QStringList arguments;

    std::string str = "\"";
    std::string str2 = "/";
    QString sdbfilestr = QString( dbfilestr );
    //QString snewdbcstr = QString(str.c_str()) + newdbcstr + QString(str.c_str()) + QString(str2.c_str());
    arguments << QString( sdbfilestr );
    QString snewdbcstr ;
    snewdbcstr += S->DBFile().path();
    arguments << QString( snewdbcstr ) ;

    //std::cout << "Db file " << S->DBFile().path().toStdString() << std::endl;

    //dbConvertProgramPath += " ";
    //dbConvertProgramPath += dbfilestr;
    //dbConvertProgramPath += " " ;
    //dbConvertProgramPath += newdbcstr;

    P.start( dbConvertProgramPath, arguments );
    //P.start( dbConvertProgramPath );
    P.waitForFinished();

    //qDebug() <<  dbConvertProgramPath << "  " << arguments ;

    loadDBFile( S->DBFile().path() );
    //S->SetDBFile( FilePath( newDBName + "/ModelDB.sqlite" ) );

    return NE::kReturnSuccess;
}


///
/// \brief FEnginePrivate::loadPDMSFile
/// \param filePath
/// \return
///

uint FEnginePrivate::loadPDMSFile(const QString& filePath )
{
    std::string fstr = filePath.toStdString();
    char* dbfilestr = const_cast< char *>( fstr.c_str() );

    QFileInfo dbFileInfo( filePath );
    m_DBFile = filePath;
    QDir dbFileDir = dbFileInfo.absoluteDir();

    //
    // Make the new DB Folder
    //
    QString newDBName = dbFileDir.absolutePath() + "/" + dbFileInfo.baseName();
    QDir dbFileProjectDir( newDBName );
    dbFileProjectDir.mkdir( newDBName );

    //
    // Convert the database
    //
    std::string newDBstr = newDBName.toStdString();
    char* newdbcstr = const_cast< char *>( newDBstr.c_str() );

    QProcess P(S);
    QString dbConvertProgramPath;
    dbConvertProgramPath =  QStandardPaths::findExecutable( "ImportPDMS" );

    QStringList arguments;

    std::string str = "\"";
    std::string str2 = "/";

    QString sdbfilestr = QString( dbfilestr );
    QString snewdbcstr = QString(newdbcstr);
    arguments << QString(  sdbfilestr );
    arguments << QString(  snewdbcstr ) ;

    //dbConvertProgramPath += " ";
    //dbConvertProgramPath += dbfilestr;
    //dbConvertProgramPath += " " ;
    //dbConvertProgramPath += newdbcstr;

    P.start( dbConvertProgramPath, arguments );
    //P.start( dbConvertProgramPath );
    P.waitForFinished();

    qDebug() << "command " << dbConvertProgramPath << "  " << arguments ;

    loadDBFile( S->DBFile().path() );

    //S->SetDBFile( FilePath( newDBName + "/ModelDB.sqlite" ) );

    return NE::kReturnSuccess;
}


void FEngine::restoreCamera()
{
    FZone* zone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName()));
    if( !zone )
        return;
    QString model3DInfo = zone->model3DInfo();

    QJsonDocument jsonResponse = QJsonDocument::fromJson( model3DInfo.toUtf8() );

    if( jsonResponse.isNull() )
    {
        //std::cout << "NULL " << model3DInfoStr.toStdString() <<  std::endl;
        return;
    }

    QJsonArray jsonArray = jsonResponse.array();

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject    = r.toObject();
        QString propertyName        = jsonObject["name"].toString();
        QString propertyTypeStr     = jsonObject["typeName"].toString();

        // Camera information are retrieved if the node is a zone
        if (propertyTypeStr == "cameraInfo")
        {
            QJsonDocument jsonDoc(jsonObject["value"].toArray());
            FEngine::setPropertiesFromJSON(NERENDERMAN.cameras()[propertyName], QString(jsonDoc.toJson()));

        }
    }
}


QJsonArray FEngine::getJSONFromProperties(NENode *node)
{
    if( node == nullptr )
        return QJsonArray();

    QJsonArray propertyArray;

    const QMetaObject *metaObj = node->metaObject();
//    qDebug() << "class name: " << metaObj->className();
//    qDebug() << "method count: " << metaObj->methodCount();
//    qDebug() << "property count: " << metaObj->propertyCount();
//    qDebug() << "ClassInfo count: " << metaObj->classInfoCount();
//    qDebug() << "properties: ";

    for (int i = 0; i < metaObj->propertyCount(); ++i)
    {
        QMetaProperty p = metaObj->property(i);

        QVariant v = p.read( node );
//        qDebug() << p.name()  << " " << p.type() << " " << p.typeName();
//        qDebug() << v;

        //
        // Accoring to
        // https://stackoverflow.com/questions/31290606/qmetatypefloat-not-in-qvarianttype
        // v.type() returns QMetaType::Type rather than QVariant::Type so typecasting has to be
        // done.
        //

        QJsonObject propValObj;
        propValObj[ "name" ]  = p.name();
        propValObj[ "type" ]  = p.type();
        propValObj[ "typeName" ]  = p.typeName();

        if( v.canConvert<FilePath>() )
        {
            FilePath sv = v.value<FilePath>();
            propValObj[ "value" ] = QJsonValue::fromVariant(sv);
        }
        else if (v.canConvert<vec3f>())
        {
            vec3f vec = v.value<vec3f>();
            QJsonObject vecObj;
            vecObj["x"] = vec.x();
            vecObj["y"] = vec.y();
            vecObj["z"] = vec.z();
            propValObj["value"] = vecObj;
        }
        else if (v.canConvert<vec2f>())
        {
            vec2f vec = v.value<vec2f>();
            QJsonObject vecObj;
            vecObj["x"] = vec.x();
            vecObj["y"] = vec.y();
            propValObj["value"] = vecObj;
        }
        else if( v.canConvert<eulerf>() )
        {
            eulerf sv = v.value<eulerf>();
            QJsonObject vv;
            vv ["phi"] = sv.phi();
            vv ["theta"] = sv.theta();
            vv ["psi"] = sv.psi();
            propValObj[ "value" ] = vv;
        }
        else if( static_cast<QMetaType::Type>(v.type())  == QMetaType::QColor )
        {
            QColor sv = v.value<QColor>();
            propValObj[ "value" ] = QJsonValue::fromVariant(sv);
        }
        else if( static_cast<QMetaType::Type>(v.type())  == QMetaType::QString )
        {
            QString sv = v.value<QString>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Double )
        {
            double sv = v.value<double>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Float )
        {
            float sv = v.value<float>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Int )
        {
            double sv = v.value<double>();
            propValObj[ "value" ] = sv;
        }
        else if( static_cast<QMetaType::Type>(v.type()) == QMetaType::Bool )
        {
            bool sv = v.value<bool>();
            propValObj[ "value" ] = sv;
        }
        else if(v.userType() == (int)getFDYNVarIds().fireMaterialId)
        {
            NEFireMaterial sv = v.value<NEFireMaterial>();
            propValObj[ "value" ] = sv.uniqueID();
        }

        else if(v.userType() == (int)getFDYNVarIds().dynamicComboId)
        {
            NEDynamicCombo val = v.value<NEDynamicCombo>();
            propValObj[ "value" ] = val.curIndex();
        }
        else if(v.userType() == (int)getFDYNVarIds().surfaceMaterialId)
        {
            NESurfaceMaterial sv = v.value<NESurfaceMaterial>();
            propValObj[ "value" ] = sv.uniqueID();
        }
        else if(v.userType() == (int)getFDYNVarIds().cableMaterialId)
        {
            NECableProperty sv = v.value<NECableProperty>();
            propValObj[ "value" ] = sv.uniqueID();
        }
        else if(v.userType() == (int)getFDYNVarIds().failureCriteriaId)
        {
            NEFailureCriteria fail = v.value<NEFailureCriteria>();
            propValObj[ "value" ] = fail.ID();
        }

        propertyArray.append( propValObj );
    }

    // Camera information are stored if the node is a zone
    if (qobject_cast<FZone*>(node))
    {
        foreach (NECamera *cam, NERENDERMAN.cameras())
        {
            QJsonObject propValObj;

            propValObj["name"] = cam->objectName();
            if (cam->Ortho())
                propValObj["type"] = 0;
            propValObj["typeName"] = QString("cameraInfo");
            propValObj["value"] = FEngine::getJSONFromProperties(cam);

            propertyArray.append(propValObj);
        }
    }

    //
    // Now add the dynamic properties which are present already as part of the DB
    //
    QList<QByteArray> b = node->dynamicPropertyNames();

    for( int i = 0; i < b.size(); ++i )
    {
        QVariant v = node->property( b[i] );
    }

    return propertyArray;
}

void FEngine::setPropertiesFromJSON(NENode *node, QString model3DInfoStr)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson( model3DInfoStr.toUtf8() );

    if( jsonResponse.isNull() )
    {
        //std::cout << "NULL " << model3DInfoStr.toStdString() <<  std::endl;
        return;
    }

    QJsonArray jsonArray = jsonResponse.array();
    const QMetaObject *metaObj = node->metaObject();



//        qDebug() << "class name: " << metaObj->className();
//        qDebug() << "method count: " << metaObj->methodCount();
//        qDebug() << "property count: " << metaObj->propertyCount();
//        qDebug() << "ClassInfo count: " << metaObj->classInfoCount();
//        qDebug() << "properties: ";

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject    = r.toObject();
        QString propertyName        = jsonObject["name"].toString();
        //std::cout << "Prop " << propertyName.toStdString() << std::endl;
        QString propertyTypeStr     = jsonObject["typeName"].toString();
        //uint    propertyType        = jsonObject["type"].toInt();
        QVariant v = jsonObject["value"].toVariant();

        //QMetaType::Type ttype = static_cast< QMetaType::Type>( propertyType );
        uint pindex = metaObj->indexOfProperty( propertyName.toUtf8() );
        QMetaProperty pv = metaObj->property( pindex );


        QString propclass = classForProperty(node, propertyName.toStdString().c_str());
//        std::cout << "Class Property " <<  propclass.toStdString() <<
//                     std::endl;

        if( pv.isEnumType() )
        {
            int enumValue = v.toInt();
            if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), enumValue );
        }
        else if(pv.userType() ==  (int)getNEVarIds().filePathId )
        {
            FilePath vpath = v.value<FilePath>();
            if( !vpath.path().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), vpath );
            }
        }
        else if(pv.userType() == (int)getFDYNVarIds().dynamicComboId)
        {
            int val = v.value<int>();

            if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), val );
        }
        else if( pv.userType() == (int)getFDYNVarIds().surfaceMaterialId )
        {
            int matid = v.value<int>();

            NESurfaceMaterial curMat;
            for( int i = 0; i < P->m_SurfaceMaterials.size(); ++i )
            {
                if( P->m_SurfaceMaterials[i].uniqueID() == matid )
                {
                    curMat = P->m_SurfaceMaterials[i];
                    break;
                }
            }
            if( !curMat.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), curMat );
            }
        }
        else if( pv.userType() == (int)getFDYNVarIds().cableMaterialId )
        {
            int cableprop = v.value<int>();

            NECableProperty curCableProp;
            for( int i = 0; i < P->m_CablePropertyList.size(); ++i )
            {
                if( P->m_CablePropertyList[i].uniqueID() == cableprop )
                {
                    curCableProp = P->m_CablePropertyList[i];
                    break;
                }
            }
            if( !curCableProp.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), curCableProp );
            }

            NECableProperty mtl = v.value<NECableProperty>();
            if( !mtl.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), mtl );
            }
        }
        else if(pv.userType() == (int)getFDYNVarIds().fireMaterialId)
        {
            int fireMatId = v.value<int>();

            NEFireMaterial curFireMat;
            for( int i = 0; i < P->m_FireMaterials.size(); ++i )
            {
                if( P->m_FireMaterials[i].uniqueID() == fireMatId )
                {
                    curFireMat = P->m_FireMaterials[i];
                    break;
                }
            }
            if( !curFireMat.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), curFireMat );
            }
        }
        else if(pv.userType() == (int)getFDYNVarIds().failureCriteriaId)
        {
            int failID = v.value<int>();

            NEFailureCriteria curFailCrit;
            for( int i = 0; i < P->m_FailCriteriaList.size(); ++i )
            {
                if( P->m_FailCriteriaList[i].ID() == failID )
                {
                    curFailCrit = P->m_FailCriteriaList[i];
                    break;
                }
            }

            NEFailureCriteria fail = v.value<NEFailureCriteria>();
            if( !curFailCrit.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), curFailCrit );
            }

        }
        else if(pv.userType() == (int)getFDYNVarIds().ventCriteriaId)
        {
            int ventID = v.value<int>();

            NEVentCriteria curVentCrit;
            for( int i = 0; i < P->m_VentCriteriaList.size(); ++i )
            {
                if( P->m_VentCriteriaList[i].ID() == ventID )
                {
                    curVentCrit = P->m_VentCriteriaList[i];
                    break;
                }
            }
            if( !curVentCrit.name().isEmpty())
            {
                if (pv.isWritable()) node->setProperty( propertyName.toUtf8(), curVentCrit );
            }
        }
        else if(propertyTypeStr == "QColor")
        {
            QColor cv= v.value<QColor>();
            if (pv.isWritable()) node->setProperty(propertyName.toUtf8(), cv);
        }
        else if(propertyTypeStr == "vec2f")
        {
            QJsonObject vecObj = jsonObject["value"].toObject();
            float x = vecObj["x"].toDouble();
            float y = vecObj["y"].toDouble();
            vec2f vec(x, y);
            if (pv.isWritable()) node->setProperty(propertyName.toUtf8(), vec);
        }
        else if(propertyTypeStr == "vec3f")
        {
            QJsonObject vecObj = jsonObject["value"].toObject();
            float x = vecObj["x"].toDouble();
            float y = vecObj["y"].toDouble();
            float z = vecObj["z"].toDouble();
            vec3f vec(x, y, z);
            if (pv.isWritable()) node->setProperty(propertyName.toUtf8(), vec);
        }
        else if(propertyTypeStr == "eulerf")
        {
            QJsonObject vv = jsonObject["value"].toObject();
            float phi = vv["phi"].toDouble();
            float psi = vv["psi"].toDouble();
            float theta = vv["theta"].toDouble();
            eulerf ev(phi, theta, psi);
            if (pv.isWritable()) node->setProperty(propertyName.toUtf8(), ev);
        }
        // Camera information are retrieved if the node is a zone
        else if (propertyTypeStr == "cameraInfo")
        {
            if (qobject_cast<FZone*>(node))
            {
                QJsonDocument jsonDoc(jsonObject["value"].toArray());
                FEngine::setPropertiesFromJSON(NERENDERMAN.cameras()[propertyName], QString(jsonDoc.toJson()));
            }
        }
        else
        {
            if (pv.isWritable() &&
                    propertyName != "objectName" &&
                    propertyName != "ID" &&
                    propertyName != "Desc" &&
                    propertyName != "Issues" )
                node->setProperty( propertyName.toUtf8(), v );
        }
    }
}

///
/// \brief FEnginePrivate::loadDBFile
/// \param filePath
/// \return
///

uint FEnginePrivate::loadDBFile(const QString& filePath)
{
    emit NEBASE.showSceneLoadingWidget();
    emit NEBASE.setCurrentOperation( "Loading DB file" );
    emit NEBASE.setCurrentSubOperation( "Preparing API functions" );
    NEPushAttrib(NE_UNDO_REDO_STATE);
    EnableUndoRedo(false);
    //
    // Load the database
    //
    std::string newDBstr = filePath.toStdString();
    char* newdbcstr = const_cast< char *>( newDBstr.c_str() );

    bool recover = ProjectHasRecoverDB( newdbcstr );
    //qDebug() << "Project Has Recover " << recover;
    if( recover == true )
    {
        QString msg =  QString("This project was closed without saving.\n"
                          "Would you like to retrieve the latest changes ?\n"
                          "If No, latest changes will be lost."
                          "");
        emit S->sendProjectRecovery( msg, recover );
    }
    //qDebug() << "Opening after recover ";

    OpenDB( newdbcstr, m_Recover );

    m_ZoneIdNamePairs = getZoneIdNamePairs();
    m_ScnIdNamePairs = getScnIdNamePairs(m_FRANXScenarios);

    emit NEBASE.setCurrentSubOperation( "Creating Zone and Scenario list" );

    QStringList zoneNameList = zoneNames();
    emit S->allZones( zoneNameList );
    QStringList scnNameList = scnNames();
    emit S->allScenarios( scnNameList );

    emit S->sendUpdateComputeScenarios(m_FRANXScenarios);

    m_SurfaceMaterials = S->getSurfaceMaterials();
    m_DefaultSurfaceMaterials = S->getDefaultSurfaceMaterials();
    emit S->sendUpdateSurfaceMaterials( m_SurfaceMaterials );

    m_FireMaterials = S->getFireMaterials();
    emit S->sendUpdateFireMaterials( m_FireMaterials );

    m_FailCriteriaList = S->getFailureCriteria();
    emit S->sendUpdateFailureCriterias( m_FailCriteriaList );

    // Cable options must be accessed before cable properties
    m_CableOptions = S->getCableOptions();
    m_CablePropertyList = S->getCableProperty();
    emit S->sendUpdateCableProperties(m_CablePropertyList, m_CableOptions);

    NEPopAttrib(NE_UNDO_REDO_STATE);

    emit NEBASE.hideSceneLoadingWidget();

    QFileInfo dbfileinfo( filePath );
    //QString scenefile = dbfileinfo.completeBaseName() + "." + NEBASE.FileExt();
    QString scenefile = m_UIDir + "/" + dbfileinfo.baseName() + "." + NEBASE.FileExt();

    //qDebug() << "Base NAme " << dbfileinfo.baseName();

    QFile sceneFilefri3d( scenefile );

    //QString scenefolder = dbfileinfo.absolutePath();
    QString scenefolder = m_UIDir;

    if( sceneFilefri3d.exists() )
    {
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
        NEBASE.SetSceneFilePath( scenefile );
        if( m_loadUI )
        {
            NEBASE.open( scenefile );
        }
        qDebug() << "Open Scene file " << NEBASE.SceneFilePath().path();
        qDebug() << "Open Scene Folder " << NEBASE.SaveDir().path();
        qDebug() << "Open Cache Folder " << NEBASE.CacheDir().path();
    }
    else
    {
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
        NEBASE.SetSceneFilePath( scenefile );
        //NEBASE.create( scenefile );
    }

    emit S->dataChanged( filePath );

    return NE::kReturnSuccess;
}

///
/// \brief FEngine::reloadDBFile
/// \return
///

uint FEngine::reloadDBFile()
{
    clear();
    P->loadDBFile( P->m_DBFile.path() );
    return NE::kReturnSuccess;
}

uint FEngine::saveDBFile()
{
    return NE::kReturnSuccess;
}


///
/// \brief FEngine::newData
/// \param filename
///

void FEngine::newData(QString filename)
{
    NEPushAttrib(NE_UNDO_REDO_STATE);
    EnableUndoRedo(false);
    //
    // Load the database
    //
    std::string newDBstr = filename.toStdString();
    char* newdbcstr = const_cast< char *>( newDBstr.c_str() );

    P->m_DBFile = FilePath( newdbcstr );
    P->NewDB( newdbcstr );

    P->m_ZoneIdNamePairs = P->getZoneIdNamePairs();
    P->m_ScnIdNamePairs = P->getScnIdNamePairs(P->m_FRANXScenarios);
    emit sendUpdateComputeScenarios(P->m_FRANXScenarios);

    QFileInfo dbfileinfo( filename );
    QString scenefile = P->m_WorkingDir + "/" + dbfileinfo.baseName() + "." + NEBASE.FileExt();
    QFile sceneFilefri3d( scenefile );
    QString scenefolder = dbfileinfo.absolutePath();

    if( sceneFilefri3d.exists() )
    {
        NEBASE.SetSceneFilePath( FilePath( scenefile ) );
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
    }
    else
    {
        //NEBASE.create( scenefolder + "/" + scenefile );
        NEBASE.SetSceneFilePath( FilePath( scenefolder ) );
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
    }

    emit dataChanged( filename );

    QStringList zoneNameList = P->zoneNames();
    emit allZones( zoneNameList );
    QStringList scnNameList = P->scnNames();
    emit allScenarios( scnNameList );

    NEPopAttrib(NE_UNDO_REDO_STATE);

    return;

}

void FEngine::load(QString filename)
{
    if( filename == "" )
    {
        clear();
        loadDBFile();
    }
    else
    {
        clear();
        P->loadDBFile( filename );
        P->m_DBFile = FilePath( filename );
    }
    //NEBASE.loadGUIState();
    QFileInfo dbfileinfo( filename );

    if( filename == "" )
    {
        dbfileinfo = QFileInfo( P->m_DBFile.path() );
    }
    QString fullscenefile = P->m_UIDir + "/" + dbfileinfo.baseName() + "." + NEBASE.FileExt();
    QDir uidir( P->m_UIDir);
    QString scenefolder = P->m_UIDir;

    if( P->m_loadUI && uidir.exists() )
    {
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
        NEBASE.SetSceneFilePath( fullscenefile );
        //emit sendLoadUIData( fullscenefile );
    }


    //NEBASE.open( fullscenefile );
    emit dataChanged(filename);
}

///
/// \brief FEngine::save
/// \param filename
///

void FEngine::save(QString filename)
{
    QFileInfo dbfileinfo( filename );
    if( filename == "" )
    {
        dbfileinfo = QFileInfo( P->m_DBFile.path() );
    }
    //QString scenefile = dbfileinfo.completeBaseName() + "." + NEBASE.FileExt();
    QString fullscenefile = P->m_UIDir + "/" + dbfileinfo.baseName() + "." + NEBASE.FileExt();

    //qDebug() << "Base NAme " << dbfileinfo.baseName();

    //std::cout << "scene " << fullscenefile.toStdString() << std::endl;
    QFile sceneFilefri3d( fullscenefile );
    //QString scenefolder = dbfileinfo.absolutePath();
    QString scenefolder = P->m_UIDir;

    if( filename != "" )
    {
        P->m_DBFile = FilePath( filename );
        P->SaveProjectAsDB( filename );
    }
    else
    {
        // qDebug() << "Saving Temp " ;
        if( FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(activeZoneName())) )
        {
            QString json = P->createJSONFromSceneZone(activeZone);
            P->SaveModelItemDB(json, activeZone->ID());
        }
        P->saveModelItems();
        P->SaveTempWorkDB();

    }

    QDir uidir( P->m_UIDir);
    if( P->m_saveUI && uidir.exists() )
    {
        NEBASE.SetSaveDir( scenefolder );
        NEBASE.SetCacheDir( scenefolder + "/" + dbfileinfo.baseName() );
        NEBASE.SetSceneFilePath( fullscenefile );
        emit sendSaveUIData( fullscenefile );
    }
}

//
//
//

void FEngine::quit()
{
    bool recover = P->ProjectHasRecoverDB( P->m_DBFile.path() );
    if( recover == true )
    {
        QString msg = tr("This project has changes.\n"
                         "Would you like to save the latest changes ?\n"
                         "");
        emit sendProjectRecovery( msg , recover );
    }

    if( P->m_Recover == true )
    {
        save( P->m_DBFile );
    }

    //save( P->m_DBFile );

    return;
}



void FEngine::setRecoverData(bool recover)
{
    P->m_Recover = recover;
    //qDebug() << "Setting Recover " << recover ;
    return;
}


void FEngine::updateWorkingFolder(QString dirname)
{
    P->SetTempWorkLocDB( dirname );
    P->m_WorkingDir = dirname;

    return;
}

void FEngine::updateUIFolder(QString dirname)
{
    P->m_UIDir = dirname;

    NEBASE.SetSaveDir( FilePath( P->m_UIDir ) );
    NEBASE.SetCacheDir( FilePath( P->m_UIDir + "/" + NEBASE.sceneName() + "/" ) );

    qDebug() << "Save Dir " << NEBASE.SaveDir().path();
    qDebug() << "Cache Dir " << NEBASE.CacheDir().path();
}

void FEngine::updateLoadUI(bool loadUI)
{
    P->m_loadUI = loadUI;
}

void FEngine::updateSaveUI(bool saveUI)
{
    P->m_saveUI = saveUI;
}


///
/// \brief FEngine::clear
/// \return
///

void FEngine::clear()
{
    emit NEBASE.selectObject(NULL);
    emit NEBASE.closeAllPropertyEditors();
    FZone* cactiveZone = qobject_cast< FZone *>( NESCENE.findChildNode( activeZoneName() ) );

    if( cactiveZone )
    {
        //
        // Clearing internal storage
        //
        P->m_ZoneItems.clear();
        P->m_SubSceneZoneItems.clear();
        P->m_ZoneCompartments.clear();
        P->m_ZoneVents.clear();
        //
        // Clear Scene Storage
        //
        NESCENE.deleteObject( (NENode *) cactiveZone, false );
    }
    NESCENE.cleanup();

    FScenario* cactiveScn = qobject_cast< FScenario *>( NESCENE.findChildNode( activeScenarioName() ) );
    if( cactiveScn )
    {
        //
        // Clearing internal storage
        //
        P->m_ScenarioItems.clear();
        P->m_ScenarioVents.clear();
        P->m_SubSceneScenarioItems.clear();
        //
        // Clear Scene Storage
        //
        NESCENE.deleteObject( cactiveScn, false );

    }

    NESCENE.cleanup();

    FZone *nullZone = NULL;
    FScenario *nullScn = NULL;

    FEngine *curengine = this;

    emit activeZone((QObject* &)curengine, (QObject* &)nullZone);
    emit activeScenario((QObject* &)curengine, (QObject* &)nullScn);

    QList<QString> empty;
    empty.clear();
    emit allZones(empty);
    emit allScenarios(empty);
    emit relatedZones( empty);
    emit relatedScenarios( empty);

    P->m_FailedItems.clear();
    emit allFailedItems(P->m_FailedItems.failedItemID, P->m_FailedItems.failedTime, P->m_FailedItems.scenarioID, P->m_FailedItems.failOutput);

    P->m_DefaultSurfaceMaterialAssignedIndexes.clear();

    emit NEBASE.updateSceneInspector();
    emit NEBASE.updateGLWindows();

    return;
}


///
/// \brief FEnginePrivate::createScenarioProperties
/// \param zone
/// \param jsonObject
///

void FEnginePrivate::createScenarioProperties(std::shared_ptr<FScenario> scn, QJsonObject &jsonObject)
{
    createSpatialProperties( scn, jsonObject );
}

///
/// \brief FEnginePrivate::createScenario
/// \param zone
/// \param scenarioName
///

std::shared_ptr< FScenario >
FEnginePrivate::createScenario( QString scenarioName,
                                QString jsonData )
{
    QString scenarioJsonData = jsonData;

    if( jsonData == "" )
        scenarioJsonData = GetScenariosDB();

    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( scenarioJsonData.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return nullptr;
    }

    QJsonArray jsonArray = jsonResponse.array();

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject = r.toObject();
        QString sname       = jsonObject["cleanName"].toString();

        uint sid            = jsonObject["connectorID"].toInt();

        if( scenarioName == "" )
        {
            QString zString = GetPlantScenarioTreeDB( sid );
            std::shared_ptr< FScenario > scnzone = createScenarioFromJSON( zString );
            return scnzone;
        }
        else if( sname == scenarioName )
        {
            QString zString = GetPlantScenarioTreeDB( sid );

            std::shared_ptr< FScenario > scnzone = createScenarioFromJSON( zString );
            return scnzone;
        }
    }

    return nullptr;
}

///
/// \brief FEnginePrivate::createScenarioFromJSON
/// \param zone
/// \param jsonData
///

std::shared_ptr< FScenario >
FEnginePrivate::createScenarioFromJSON( QString &jsonData )
{
    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( jsonData.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return nullptr;
    }

    QJsonObject   jsonObject = jsonResponse.object();

    QString originalName = jsonObject["name"].toString();
    QString name        = jsonObject["cleanName"].toString();
    QString desc        = jsonObject["desc"].toString();
    QString ztype       = jsonObject["type"].toString();
    uint connectorID    = jsonObject["connectorID"].toInt();
    uint id             = connectorID;
    QString issues      = jsonObject["issues"].toString();
    QJsonArray model3dInfo = jsonObject[ "model3Dinfo"].toArray();
    QJsonObject simItemsJsonObject = jsonObject["simulationItems"].toObject();

    // FilePath modelFilePath( modelFile );

    std::shared_ptr< FScenario >newScenario = std::shared_ptr< FScenario >( new FScenario );
    //newScenario->SetScenarioFile( modelFilePath );
    newScenario->SetID( id );
    newScenario->SetConnectorID( connectorID );
    newScenario->SetType( ztype );
    newScenario->SetDesc( desc );
    newScenario->SetIssues( issues );
    newScenario->setOriginalName(originalName);
    newScenario->setScenarioName( name );

    QJsonDocument doc;
    doc.setArray( model3dInfo );
    QString dataToString( doc.toJson() );
    newScenario->setModel3DInfo( dataToString );

    createScenarioItems( newScenario, jsonObject );
    createScenarioProperties( newScenario, jsonObject );

    createSimItemsFromJson(newScenario, simItemsJsonObject);

    return newScenario;
}

///
/// \brief FEnginePrivate::createItems
/// \param parent
/// \param grandParent
/// \param jsonObject
///

void FEnginePrivate::createZoneItems( std::shared_ptr< FZone > zone,
                                      QJsonObject &jsonObject)
{
    QStringList keys = jsonObject.keys();
    for( int i = 0; i < keys.size(); ++i )
    {
        QJsonValueRef jsonValueRef = jsonObject[keys[i]];
        if( jsonValueRef.type() == QJsonValue::Array  && keys[i] != "model3DInfo" )
            createZoneItemsFromJSON( zone, zone.get(), jsonValueRef, m_JsonPlantTopItemLib[keys[i].toUtf8()] );
    }
}

void FEnginePrivate::createScenarioItems( std::shared_ptr< FScenario > scenario,
                                          QJsonObject &jsonObject)
{
    QStringList keys = jsonObject.keys();
    for( int i = 0; i < keys.size(); ++i )
    {
        QJsonValueRef jsonValueRef = jsonObject[keys[i]];
        if( jsonValueRef.type() == QJsonValue::Array && keys[i] != "model3DInfo" )
            createScenarioItemsFromJSON( scenario, scenario.get(), jsonValueRef, m_JsonPlantTopItemLib[keys[i].toUtf8()] );
    }
}

void FEnginePrivate::createSimItemsFromJson(std::shared_ptr<NESpatialGroup> parent, const QJsonObject &jsonObject)
{
    QJsonArray detectJsonArray = jsonObject["Detection/Suppression"].toArray();
    QJsonArray staticVentJsonArray = jsonObject["Static Vents"].toArray();
    QJsonArray mechVentJsonArray = jsonObject["Mechanical Vents"].toArray();

    for (int i = 0; i != staticVentJsonArray.size(); i++)
        createVentFromJson(parent, staticVentJsonArray[i].toObject());

    for (int i = 0; i != mechVentJsonArray.size(); i++)
        createVentFromJson(parent, mechVentJsonArray[i].toObject());
}

void FEnginePrivate::createVentFromJson(std::shared_ptr<NESpatialGroup> parent, const QJsonObject &jsonObject)
{
    QJsonObject jsonFields = jsonObject["jsonFields"].toObject();

    std::shared_ptr<FVent> vent;
    if (jsonObject["itemType"] == "siMechVent")
        vent = std::shared_ptr<FMechVent>(new FMechVent);
    else
        vent = std::shared_ptr<FVent>(new FVent);

    vent->setOriginalName(jsonObject["name"].toString());
    vent->setName(jsonObject["cleanName"].toString());
    vent->SetDesc(jsonObject["desc"].toString());
    vent->SetType(jsonObject["type"].toString());
    vent->SetID(jsonObject["connectorID"].toInt());
    vent->SetIssues(jsonObject["issues"].toString());
    vent->setModel3DInfo(QString(QJsonDocument(jsonObject["model3DInfo"].toArray()).toJson()));

    vent->setFirstCompartmentId(jsonFields["firstTopFromCompartment"].toInt());
    vent->setSecondCompartmentId(jsonFields["secondBottomToCompartment"].toInt());

    uint criterionId = jsonFields["criterionID"].toInt();
    static NEVentCriteria criterion;
    bool criterionFound = false;
    for (int i = 0; i < m_VentCriteriaList.size(); i++)
    {
        if (m_VentCriteriaList[i].ID() == int(criterionId))
        {
            criterion = m_VentCriteriaList[i];
            criterionFound = true;
            break;
        }
    }
    if (!criterionFound)
    {
        criterion.setName("Null");
        criterion.setID(0);
        criterion.setIndex(0);
    }
    vent->SetCriterion(criterion);

    if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
    {
        mechVent->SetFlowRate(jsonFields["flowRate"].toDouble());
        mechVent->SetBeginDropOff(jsonFields["beginDropOff"].toDouble());
        mechVent->SetZeroAt(jsonFields["ZeroFlow"].toDouble());
        mechVent->SetFilterEfficiency(jsonFields["filterEfficiency"].toDouble());
        mechVent->SetFilterStart(jsonFields["beginFilter"].toDouble());
    }

    if (FZone* zone = qobject_cast<FZone*>(parent.get()))
        m_ZoneVents[zone->ID()].insert(vent->ID(), vent);
    else if (FScenario* scenario = qobject_cast<FScenario*>(parent.get()))
        m_ScenarioVents[scenario->ID()].insert(vent->ID(), vent);
}

///
/// \brief FEnginePrivate::saveModelItem
/// \param itemName
/// \param parentID
///
int FEnginePrivate::saveModelItem(QString itemName)
{
    FZone *zone = qobject_cast< FZone *>( NESCENE.findChildNode( itemName ) );
    FScenario *scn  = qobject_cast< FScenario *>( NESCENE.findChildNode( itemName ) );
    FZoneItem *zi  = qobject_cast< FZoneItem *>( NESCENE.findChildNode( itemName ) );

    uint parentID = 0;
    if( zone )
        parentID = zone->ID();
    else if( scn )
        parentID = scn->ID();
    else if( zi )
        parentID = zi->ID();

    QString sceneZoneJson = createJSONFromSceneItemName( itemName );

    return SaveModelItemDB( sceneZoneJson, parentID );
}

///
/// \brief FEnginePrivate::saveModelItems
/// \param modelitemName
///

void FEnginePrivate::saveModelItems( QString modelItemName )
{
    FZone *zone;
    FScenario *scn;
    if( modelItemName == "" )
    {
        zone = qobject_cast< FZone *>( NESCENE.findChildNode( S->activeZoneName() ) );
        scn  = qobject_cast< FScenario *>( NESCENE.findChildNode( S->activeScenarioName() ) );
    }
    else
    {
        zone = qobject_cast< FZone *>( NESCENE.findChildNode( modelItemName ) );
        scn  = qobject_cast< FScenario *>( NESCENE.findChildNode( modelItemName ) );
    }

    QString allZoneItemsJson, allZoneSimItemsJson;

    if( zone )
    {
        allZoneItemsJson = "[ ";
        allZoneSimItemsJson = "[";

        for (auto& zoneItemName : zone->itemNameChildren(zone->objectName()))
        {
            QString sceneZoneJson = createJSONFromSceneItemName( zoneItemName );
            if( sceneZoneJson != "" )
            {
                allZoneItemsJson += sceneZoneJson + ",";
            }

            if( FZoneItem* zi = qobject_cast< FZoneItem *>( NESCENE.findChildNode( zoneItemName ) ) )
            {
                for (auto& zoneSubItemName : zone->itemNameChildren(zi->objectName()))
                {
                    QString sceneziJson = createJSONFromSceneItemName( zoneSubItemName );
                    if( sceneziJson != "" )
                    {
                        allZoneItemsJson += sceneziJson + ",";
                    }
                }
            }
            else if( FZoneCompartment* comp = qobject_cast< FZoneCompartment *>( NESCENE.findChildNode( zoneItemName ) ) )
            {
                QString json = createJSONFromCompartment(comp);
                if( json != "" )
                {
                    allZoneItemsJson += json + ",";
                }
            }
            else if (FVent* vent = qobject_cast<FVent*>(NESCENE.findChildNode(zoneItemName)))
            {
                QString json = createJSONFromVent(vent);
                if (json != "")
                {
                    allZoneSimItemsJson += json + ",";
                }
            }
        }

        foreach (auto& criterion, m_VentCriteriaList)
        {
            QString jsonStr = createJSONFromVentCriteria(criterion);
            if (jsonStr != "")
                allZoneSimItemsJson += jsonStr + ",";
        }

        allZoneItemsJson += " ] ";
        allZoneSimItemsJson += "]";

        SaveModelItemsDB( allZoneItemsJson, zone->ID() );
        SaveSimulationItemsDB(allZoneSimItemsJson, zone->ID());
    }

    if( scn )
    {
        allZoneItemsJson = "[ ";
        allZoneSimItemsJson = "[";

        for (auto& scnItemName : scn->itemNameChildren(scn->objectName()))
        {
            QString sceneScnJson = createJSONFromSceneItemName( scnItemName );
            if( sceneScnJson != "" )
            {
                allZoneItemsJson += sceneScnJson + ",";
            }

            if( FZoneItem* si = qobject_cast< FZoneItem *>( NESCENE.findChildNode( scnItemName ) ) )
            {
                for( auto& scnSubItemName : zone->itemNameChildren(si->objectName()))
                {
                    QString sceneziJson = createJSONFromSceneItemName( scnSubItemName );
                    if( sceneziJson != "" )
                    {
                        allZoneItemsJson += sceneziJson + ",";
                    }
                }
            }
            else if (FVent* vent = qobject_cast<FVent*>(NESCENE.findChildNode(scnItemName)))
            {
                QString json = createJSONFromVent(vent);
                if (json != "")
                {
                    allZoneSimItemsJson += json + ",";
                }
            }
        }

        foreach (auto& criterion, m_VentCriteriaList)
        {
            QString jsonStr = createJSONFromVentCriteria(criterion);
            if (jsonStr != "")
                allZoneSimItemsJson += jsonStr + ",";
        }

        allZoneItemsJson += " ] ";
        allZoneSimItemsJson += "]";

//        SaveModelItemsDB( allZoneItemsJson, scn->ID() );
//        SaveSimulationItemsDB(allZoneSimItemsJson, scn->ID());
    }
}

QString FEnginePrivate::createJSONFromSurfaceMaterial(NESurfaceMaterial sm)
{
    QJsonObject jsonObject;

    jsonObject["name"] = sm.name();
    jsonObject["connectorID" ] = sm.uniqueID();
    jsonObject["desc"] = "";
    jsonObject["type"] = "foMaterial";
    jsonObject["issues"] = "";
    jsonObject["conductivity"] = sm.conductivity();
    jsonObject["specificHeat"] = sm.heat();
    jsonObject["density"] = sm.density();
    jsonObject["emissivity"] = sm.emissivity();
    jsonObject["thickness"] = sm.thickness();

    QJsonDocument jsonDoc(jsonObject);
    QString jsonStr(jsonDoc.toJson());

    return jsonStr;
}

QString FEnginePrivate::createJSONFromFireMaterial(NEFireMaterial fm)
{
    QJsonObject jsonObject;

    jsonObject["name"] = fm.name();
    jsonObject["connectorID"] = fm.uniqueID();
    jsonObject["desc"] = "";
    jsonObject["type"] = "foFire";
    jsonObject["issues"] = "";

    QJsonObject cFastJsonFields;
    cFastJsonFields["carbon"] = fm.C();
    cFastJsonFields["hydrogen"] = fm.H();
    cFastJsonFields["oxygen"] = fm.O();
    cFastJsonFields["nitrogen"] = fm.N();
    cFastJsonFields["chlorine"] = fm.CI();
    cFastJsonFields["heat combustion"] = fm.combuHeat();
    cFastJsonFields["radiative fraction"] = fm.radFraction();

    QJsonArray times, hrrs, heights, areas, coYields, sootYields, hcnYields, hclYields, traceYields;
    for (auto& info : fm.Infos())
    {
//        QJsonObject time, hrr, height, area, coYield, sootYield, hcnYield, hclYield, traceYield;

//        time["value"] = info.m_Time;
//        hrr["value"] = info.m_HRR;
//        height["value"] = info.m_Height;
//        area["value"] = info.m_Area;
//        coYield["value"] = info.m_CO;
//        sootYield["value"] = info.m_Soot;
//        hcnYield["value"] = info.m_HCN;
//        hclYield["value"] = 0.0/*info.m_HCl*/;
//        traceYield["value"] = info.m_TS;

        double time, hrr, height, area, coYield, sootYield, hcnYield, hclYield, traceYield;

        time = info.m_Time;
        hrr = info.m_HRR;
        height = info.m_Height;
        area = info.m_Area;
        coYield = info.m_CO;
        sootYield = info.m_Soot;
        hcnYield = info.m_HCN;
        hclYield = 0.0/*info.m_HCl*/;
        traceYield  = info.m_TS;

        times.append(time);
        hrrs.append(hrr);
        heights.append(height);
        areas.append(area);
        coYields.append(coYield);
        sootYields.append(sootYield);
        hcnYields.append(hcnYield);
        hclYields.append(hclYield);
        traceYields.append(traceYield);
    }
    cFastJsonFields["times"] = times;
    cFastJsonFields["hrrs"] = hrrs;
    cFastJsonFields["heights"] = heights;
    cFastJsonFields["areas"] = areas;
    cFastJsonFields["co yields"] = coYields;
    cFastJsonFields["soot yields"] = sootYields;
    cFastJsonFields["hcn yields"] = hcnYields;
    cFastJsonFields["hcl yields"] = hclYields;
    cFastJsonFields["trace yields"] = traceYields;
    jsonObject["cfastJsonFields"] = cFastJsonFields;

    QJsonDocument jsonDoc(jsonObject);
    QString jsonStr(jsonDoc.toJson());

    return jsonStr;
}

QString FEnginePrivate::createJSONFromFailureCriteria(NEFailureCriteria fail)
{
    QJsonObject jsonObject;

    jsonObject["name"] = fail.name();
    jsonObject["connectorID" ] = fail.ID();
    jsonObject["desc"] = "";
    jsonObject["type"] = "foFailure";
    jsonObject["itemType"] = "siCriterion";
    jsonObject["issues"] = "";

    if (fail.type() == NEFailureCriteria::Temperature)
        jsonObject["temperature"] = fail.threshold();
    else if (fail.type() == NEFailureCriteria::Time)
        jsonObject["time"] == fail.threshold();
    else
        qDebug() << "Unknown type of vent criterion to json";

    QJsonDocument doc( jsonObject );
    QString dataToString( doc.toJson() );

    return dataToString;
}

QString FEnginePrivate::createJSONFromVentCriteria(NEVentCriteria criterion)
{
    QJsonObject jsonObject;

    jsonObject["name"] = criterion.name();
    jsonObject["connectorID" ] = criterion.ID();
    jsonObject["desc"] = "";
    jsonObject["type"] = "foSimulationItem";
    jsonObject["itemType"] = "siCriterion";
    jsonObject["issues"] = "";

    QJsonObject jsonFields, cfast;
    if (criterion.type() == NEVentCriteria::Temperature)
        cfast["type"] = "temperature";
    else if (criterion.type() == NEVentCriteria::Time)
        cfast["type"] = "time";
    else
        qDebug() << "Unknown type of vent criterion to json " << static_cast<int>(criterion.type());

    QJsonArray values, percents;
    for (auto& ventValues : criterion.values())
    {
        values.append(ventValues.m_Threshold);
        percents.append(ventValues.m_Percent);
    }
    cfast["value"] = values;
    cfast["percent"] = percents;

    jsonFields["cfast"] = cfast;
    jsonObject["jsonFields"] = jsonFields;

    return QString(QJsonDocument(jsonObject).toJson());
}

NESurfaceMaterial FEnginePrivate::createSurfaceMaterialFromJSON(QString matJson)
{
    NESurfaceMaterial sm;
    QJsonDocument jdom;

    jdom = QJsonDocument::fromJson( matJson.toUtf8() );
    QJsonObject jobj = jdom.object();

    sm.setName( jobj["name"].toString() );
    sm.setUniqueID( jobj["connectorID"].toInt() );
    sm.setConductivity( jobj["conductivity"].toDouble() );
    sm.setHeat( jobj["specificHeat"].toDouble() );
    sm.setDensity( jobj["density"].toDouble() );
    sm.setEmissivity( jobj["emissivity"].toDouble() );
    sm.setThickness( jobj["thickness"].toDouble() );

    return sm;
}

NEFireMaterial FEnginePrivate::createFireMaterialFromJSON(QString fmJson)
{
    NEFireMaterial fm;
    QJsonDocument jdom;

    jdom = QJsonDocument::fromJson( fmJson.toUtf8() );
    QJsonObject jobj =  jdom.object();

    fm.setName(jobj["name"].toString());
    fm.setUniqueID(jobj["connectorID"].toInt());

    QJsonObject cfastJsonFields = jobj["cfastJsonFields"].toObject();
    fm.setC(cfastJsonFields["carbon"].toDouble());
    fm.setN(cfastJsonFields["nitrogen"].toDouble());
    fm.setO(cfastJsonFields["oxygen"].toDouble());
    fm.setH(cfastJsonFields["hydrogen"].toDouble());
    fm.setCI(cfastJsonFields["chlorine"].toDouble());
    fm.setCombuHeat(cfastJsonFields["heat combustion"].toDouble());
    fm.setRadFraction(cfastJsonFields["radiative fraction"].toDouble());

    QList<FireMaterialInfo> infos;
    int size = cfastJsonFields["times"].toArray().size();
    for (int i = 0; i != size; i++)
    {
        FireMaterialInfo info;
        info.m_Time = cfastJsonFields["times"].toArray()[i].toDouble();
        info.m_HRR = cfastJsonFields["hrrs"].toArray()[i].toDouble();
        info.m_Height = cfastJsonFields["heights"].toArray()[i].toDouble();
        info.m_Area = cfastJsonFields["areas"].toArray()[i].toDouble();
        info.m_CO = cfastJsonFields["co yields"].toArray()[i].toDouble();
        info.m_Soot = cfastJsonFields["soot yields"].toArray()[i].toDouble();
        info.m_HCN = cfastJsonFields["hcn yields"].toArray()[i].toDouble();
//        info.m_HCl = cfastJsonFields["hcl yields"].toArray()[i].toDouble();
        info.m_TS = cfastJsonFields["trace yields"].toArray()[i].toDouble();

        infos.push_back(info);
    }
    fm.setInfos(infos);

    return fm;
}

NEFailureCriteria FEnginePrivate::createFailureCriteriaFromJSON(QString jsonStr)
{
    NEFailureCriteria fail;
    QJsonDocument jdom;

    jdom = QJsonDocument::fromJson( jsonStr.toUtf8() );
    QJsonObject jobj = jdom.object();
    fail.setName( jobj["name"].toString() );
    fail.setID( jobj["connectorID"].toInt() );

    if (jobj["temperature"] != QJsonValue())
    {
        fail.setType(NEFailureCriteria::Temperature);
        fail.setThreshold(jobj["temperature"].toDouble());
    }
    else if (jobj["time"] != QJsonValue())
    {
        fail.setType(NEFailureCriteria::Time);
        fail.setThreshold(jobj["time"].toDouble());
    }
    else
    {
        qDebug() << "Unknown type of failure criteria from json";
        fail.setType(NEFailureCriteria::Temperature);
        fail.setThreshold(jobj["temperature"].toDouble());
    }

    return fail;
}

NEVentCriteria FEnginePrivate::createVentCriteriaFromJSON(QString jsonStr)
{
    NEVentCriteria criterion;

    QJsonObject jsonObj = QJsonDocument::fromJson(jsonStr.toUtf8()).object();
    criterion.setName(jsonObj["name"].toString());
    criterion.setID(jsonObj["connectorID"].toInt());

    QJsonObject jsonFields = jsonObj["jsonFields"].toObject();
    QJsonObject cfast = jsonFields["cfast"].toObject();
    if (cfast["type"].toString() == "temperature")
        criterion.setType(NEVentCriteria::Temperature);
    else if (cfast["type"].toString() == "time")
        criterion.setType(NEVentCriteria::Time);
    else
    {
        qDebug() << "Unknown type of vent criterion from json" << cfast["type"].toString();
        criterion.setType(NEVentCriteria::Temperature);
    }

    QList<NEVentCriteria::VentValues> ventValuesList;
    int size = cfast["value"].toArray().size();
    for (int i = 0; i != size; i++)
    {
        NEVentCriteria::VentValues ventValues;
        ventValues.m_Threshold = cfast["value"].toArray()[i].toDouble();
        ventValues.m_Percent = cfast["percent"].toArray()[i].toDouble();

        ventValuesList.push_back(ventValues);
    }
    criterion.setValues(ventValuesList);

    return criterion;
}

QString FEnginePrivate::createJSONFromCableProperties(NECableProperty prop)
{
    QJsonObject jsonObject;

    jsonObject["name"] = prop.name();
    jsonObject["cleanName"] = prop.cleanName();
    jsonObject["connectorID" ] = prop.uniqueID();

    if (m_CableOptions.size() > 0)
    {
        auto& categories = m_CableOptions[0];
        int categoryIndex = categories.indexOf(prop.category());
        if (categoryIndex >= 0)
            jsonObject["category"] = categoryIndex;
    }

    jsonObject["manufacturer"] = prop.manufacturer();
    jsonObject["insulType"]    = prop.insulationType();
    jsonObject["jacketType"]   = prop.jacketType();
    jsonObject["od"]           = prop.outSideDiameterMM();
    jsonObject["kgm"]          = prop.outSideDiameterKgPerMeter();
    jsonObject["jackThick"]    = prop.jacketThicknesMM();
    jsonObject["guage"]        = prop.gauges();
    jsonObject["conductors"]   = prop.conductors();
    jsonObject["grade"]        = prop.grade();
    jsonObject["partNum"]      = prop.part();
    jsonObject["itemNum"]      = prop.item();
    jsonObject["model"]        = prop.model();
    jsonObject["issues"]       = prop.issues();
    jsonObject["type"]         = "foCableSpec";
    jsonObject["desc"]         = prop.description();

    QJsonDocument jsonDoc(jsonObject);
    QString jsonStr(jsonDoc.toJson());

    return jsonStr;
}

NECableProperty FEnginePrivate::createCablePropertiesFromJSON(QString cableJson)
{
    NECableProperty cable;
    QJsonDocument jdom;

    jdom = QJsonDocument::fromJson( cableJson.toUtf8() );
    QJsonObject jsonObject = jdom.object();

    cable.setName( jsonObject["name"].toString() );
    cable.setCleanName( jsonObject["cleanName"].toString() );
    cable.setUniqueID( jsonObject["connectorID"].toInt() );

    size_t categoryIndex = static_cast<size_t>(jsonObject["category"].toInt());
    if (m_CableOptions.size() > 0)
    {
        auto& categories = m_CableOptions[0];
        cable.setCategory(categories[categoryIndex]);
    }

    cable.setManufacturer( QString( jsonObject["manufacturer"].toString() ) );
    cable.setInsulationType( jsonObject["insulType"].toInt() );
    cable.setJacketType( jsonObject["jacketType"].toInt() );
    cable.setOutsideDiameterMM( jsonObject["od"].toDouble() );
    cable.setOutSideDiameterKgPerMeter( jsonObject["kgm"].toDouble() );
    cable.setJacketThicknessMM( jsonObject["jackThick"].toDouble() );
    cable.setGauges( jsonObject["guage"].toInt() );
    cable.setConductors( jsonObject["conductors"].toInt() );
    cable.setGrade( jsonObject["grade"].toInt() );
    cable.setPart( jsonObject["partNum"].toInt() );
    cable.setItem( jsonObject["itemNum"].toInt() );
    cable.setModel( jsonObject["model"].toString() );
    cable.setIssues( jsonObject["issues"].toString() );
    cable.setDescription( jsonObject["desc"].toString() );

    return cable;
}

///
/// \brief FEnginePrivate::createSceneScenario
/// \param parent
/// \param scn
/// \return
///

FScenario *FEnginePrivate::createSceneScenario(NESpatialGroup *parent, std::shared_ptr<FScenario> scn)
{
    if( scn == nullptr )
        return nullptr;

    NESCENE.setPropertiesInitialized(false);

    FScenario* newScn = qobject_cast<FScenario*>(NESCENE.findChildNode(scn->scenarioName()));
    if (newScn == nullptr)
    {
        bool created;
        created = NESCENE.createObjectOnLoad("Fire Scenario", scn->scenarioName());
        if (!created)
        {
            qDebug() << "Failed creating scenario " << scn->scenarioName();
            return nullptr;
        }

        newScn = qobject_cast<FScenario*>(NESCENE.findChildNode(scn->scenarioName()));
    }

    newScn->setParent( S );
    newScn->SetScenarioFile( scn->ScenarioFile() );
    newScn->SetID( scn->ID() );
    newScn->SetConnectorID( scn->ConnectorID() );
    newScn->SetType( scn->Type() );
    newScn->SetDesc( scn->Desc() );
    newScn->SetIssues( scn->Issues() );
    newScn->setOriginalName(scn->originalName());
    newScn->setObjectName( scn->scenarioName() );
    newScn->setScenarioName( scn->scenarioName() );

    //
    // Now add the dynamic properties which are present already as part of the DB
    //
    QList<QByteArray> b = scn->dynamicPropertyNames();

    for( int i = 0; i < b.size(); ++i )
    {
        QVariant v = scn->property( b[i] );
        newScn->addProperty( b[i], v, b[i] );
    }

    auto siitr = m_ScenarioItems.find( scn->ID() );
    auto& items = siitr.value();
    if( siitr != m_ScenarioItems.end() )
    {
        auto sitemsitr = items.begin();
        for( ; sitemsitr != items.end(); ++sitemsitr )
        {
            std::shared_ptr< FZoneItem >& item = sitemsitr.value();
            createSceneScenarioItem( newScn, item );
        }
    }

    if (m_ScenarioVents.contains(scn->ID()))
    {
        for (auto& vent : m_ScenarioVents.value(scn->ID()))
        {
            if (vent == nullptr)
                continue;

            newScn->addToItemNameTree(vent->name(), newScn->scenarioName());

            FVent* newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
            if (newVent != nullptr)
                continue;

            if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
            {
                newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                if (newVent == nullptr)
                {
                    bool created;
                    created = NESCENE.createObjectOnLoad("Fire Mech Vent", mechVent->name());
                    if (!created)
                    {
                        qDebug() << "Failed creating mechanical vent " << mechVent->name();
                        return nullptr;
                    }
                }

                newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
            }
            else
            {
                newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                if (newVent == nullptr)
                {
                    bool created;
                    created = NESCENE.createObjectOnLoad("Fire Vent", vent->name());
                    if (!created)
                    {
                        qDebug() << "Failed creating vent " << vent->name();
                        return nullptr;
                    }
                }

                newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
            }

            newVent->setParent(newScn);
            newVent->setOriginalName(vent->originalName());
            newVent->setName(vent->name());
            newVent->setObjectName(vent->name());
            newVent->SetDesc(vent->Desc());
            newVent->SetIssues(vent->Issues());
            newVent->SetID(vent->ID());
            newVent->SetType(vent->Type());
            newVent->setItemType(vent->itemType());
            newVent->SetFirstCompartment(vent->FirstCompartment());
            newVent->SetSecondCompartment(vent->SecondCompartment());
            newVent->setFirstCompartmentId(vent->firstCompartmentId());
            newVent->setSecondCompartmentId(vent->secondCompartmentId());
            newVent->SetCriterion(vent->Criterion());

            if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newVent))
                {
                    newMechVent->SetFlowRate(mechVent->FlowRate());
                    newMechVent->SetBeginDropOff(mechVent->BeginDropOff());
                    newMechVent->SetZeroAt(mechVent->ZeroAt());
                    newMechVent->SetFilterEfficiency(mechVent->FilterEfficiency());
                    newMechVent->SetFilterStart(mechVent->FilterStart());
                }

            newVent->setModel3DInfo(vent->model3DInfo());
            S->setPropertiesFromJSON(newVent, vent->model3DInfo());
        }
    }

    //
    // Get all the zones associated with the scenario
    //
    scn->zones() = GetZoneListForScenario( scn->ID() );

    if( scn->zones().size() > 0 )
    {
        QString zoneName = scn->zones().at(0);

        FZone* newZone = qobject_cast<FZone*>(NESCENE.findChildNode(zoneName));
        if (newZone == nullptr)
        {
            bool created;
            created = NESCENE.createObjectOnLoad("Fire Zone", zoneName);
            if (!created)
            {
                qDebug() << "Failed creating zone " << zoneName;
                return nullptr;
            }

            newZone = qobject_cast<FZone*>(NESCENE.findChildNode(zoneName));
        }

        std::shared_ptr< FZone > zone = createZone( zoneName, "" );
        newZone->setParent( S );
        newZone->SetID( zone->ID() );
        newZone->SetConnectorID( zone->ConnectorID() );
        newZone->SetType( zone->Type() );
        newZone->SetDesc( zone->Desc() );
        newZone->SetIssues( zone->Issues() );
        newZone->setOriginalName(zone->originalName());
        newZone->setObjectName( zone->zoneName() );
        newZone->setZoneName( zone->zoneName() );

        newZone->SetCeilingThickness(zone->CeilingThickness());
        newZone->SetFloorThickness(zone->FloorThickness());
        newZone->SetWallThickness(zone->WallThickness());
        newZone->SetCeilingMaterial(zone->CeilingMaterial());
        newZone->SetFloorMaterial(zone->FloorMaterial());
        newZone->SetWallMaterial(zone->WallMaterial());
        newZone->SetFloorLeakRatio(zone->FloorLeakRatio());
        newZone->SetWallLeakRatio(zone->WallLeakRatio());

        //
        // Now add the dynamic properties which are present already as part of the DB
        //
        QList<QByteArray> b = zone->dynamicPropertyNames();

        for( int i = 0; i < b.size(); ++i )
        {
            QVariant v = zone->property( b[i] );
            newZone->addProperty( b[i], v, b[i] );
        }

        if( !newZone->scenarios().contains( zone->zoneName() ) )
            newZone->scenarios().push_back( zone->zoneName() );

        //
        // Set the active Zone to be this one which is currently the first one by default associated
        // with this Scenario
        FEngine *curengine = S;

        m_activeZoneName = newZone->zoneName();
        emit S->activeZone((QObject* &)curengine, (QObject* &)newZone);

        QList<QString> otherscnzonenames = GetZoneListForScenario(scn->ID());
        emit S->relatedZones(otherscnzonenames);

        //
        // Get the list of zones associated with the scenario related to this zone
        //
        QList<QString> otherzonescnnames = GetScenarioListForZone( zone->ID() );

        QString franxtree = GetFRANXTreeDB( newZone->ID(), newScn->ID() );

        QJsonDocument jsonResponse = QJsonDocument::fromJson( franxtree.toUtf8() );
        if(jsonResponse.isNull())
        {
            std::cerr << "FRANX Tree is invalid." << std::endl;
        }
        if(jsonResponse.isObject())
        {
            QJsonObject jsonObject = jsonResponse.object();
            createFRANXZoneItems(newZone, jsonObject);
        }
        else if(jsonResponse.isArray())
        {
            QJsonArray jsonArray = jsonResponse.array();

            int jsonSize = jsonArray.size();

            for( int i = 0; i < jsonSize; ++i )
            {
                QJsonValueRef r = jsonArray[i];

                QJsonObject   jsonObject = r.toObject();

                createFRANXZoneItems(newZone, jsonObject);
            }
        }

        emit S->relatedScenarios( otherzonescnnames );

        auto& zitems = m_ZoneItems[ zone->ID() ];
        auto zittr = zitems.begin();
        for( ; zittr != zitems.end(); ++zittr )
        {
            std::shared_ptr< FZoneItem >& zitem = zittr.value();

            createSceneZoneItem( newZone, zitem, newZone );
        }

        if (m_ZoneCompartments.contains(zone->ID()))
        {
            for (auto& comp : m_ZoneCompartments.value(zone->ID()))
            {
                if( comp )
                {
                    newZone->addToItemNameTree(comp->compartmentName(), newZone->zoneName());

                    FZoneCompartment* newComp = qobject_cast<FZoneCompartment*>(NESCENE.findChildNode(comp->compartmentName()));
                    if (newComp == nullptr)
                    {
                        bool created;
                        created = NESCENE.createObjectOnLoad("Fire Zone Boundary", comp->compartmentName());
                        if (!created)
                        {
                            qDebug() << "Failed creating zone compartment " << comp->compartmentName();
                            return nullptr;
                        }

                        newComp = qobject_cast<FZoneCompartment*>(NESCENE.findChildNode(comp->compartmentName()));
                    }

                    newComp->setParent(newZone);
                    newComp->setObjectName(comp->compartmentName());
                    newComp->setOriginalName(comp->originalName());
                    newComp->setCompartmentName(comp->compartmentName());
                    newComp->SetDesc(comp->Desc());
                    newComp->SetID(comp->ID());
                    newComp->SetType(comp->Type());
                    newComp->setItemType(comp->itemType());

                    newComp->setModel3DInfo(comp->model3DInfo());
                    S->setPropertiesFromJSON(newComp, comp->model3DInfo());
                }
            }
        }

        if (m_ZoneVents.contains(zone->ID()))
        {
            for (auto& vent : m_ZoneVents.value(zone->ID()))
            {
                if (vent == nullptr)
                    continue;

                newZone->addToItemNameTree(vent->name(), newZone->zoneName());

                FVent* newVent;
                if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                {
                    newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                    if (newVent == nullptr)
                    {
                        bool created;
                        created = NESCENE.createObjectOnLoad("Fire Mech Vent", mechVent->name());
                        if (!created)
                        {
                            qDebug() << "Failed creating mechanical vent " << mechVent->name();
                            return nullptr;
                        }
                    }

                    newVent = qobject_cast<FMechVent*>(NESCENE.findChildNode(mechVent->name()));
                }
                else
                {
                    newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                    if (newVent == nullptr)
                    {
                        bool created;
                        created = NESCENE.createObjectOnLoad("Fire Vent", vent->name());
                        if (!created)
                        {
                            qDebug() << "Failed creating vent " << vent->name();
                            return nullptr;
                        }
                    }

                    newVent = qobject_cast<FVent*>(NESCENE.findChildNode(vent->name()));
                }

                newVent->setParent(newZone);
                newVent->setOriginalName(vent->originalName());
                newVent->setName(vent->name());
                newVent->setObjectName(vent->name());
                newVent->SetDesc(vent->Desc());
                newVent->SetIssues(vent->Issues());
                newVent->SetID(vent->ID());
                newVent->SetType(vent->Type());
                newVent->setItemType(vent->itemType());
                newVent->SetFirstCompartment(vent->FirstCompartment());
                newVent->SetSecondCompartment(vent->SecondCompartment());
                newVent->setFirstCompartmentId(vent->firstCompartmentId());
                newVent->setSecondCompartmentId(vent->secondCompartmentId());
                newVent->SetCriterion(vent->Criterion());

                if (FMechVent* mechVent = qobject_cast<FMechVent*>(vent.get()))
                    if (FMechVent* newMechVent = qobject_cast<FMechVent*>(newVent))
                    {
                        newMechVent->SetFlowRate(mechVent->FlowRate());
                        newMechVent->SetBeginDropOff(mechVent->BeginDropOff());
                        newMechVent->SetZeroAt(mechVent->ZeroAt());
                        newMechVent->SetFilterEfficiency(mechVent->FilterEfficiency());
                        newMechVent->SetFilterStart(mechVent->FilterStart());
                    }

                newVent->setModel3DInfo(vent->model3DInfo());
                S->setPropertiesFromJSON(newVent, vent->model3DInfo());
            }
        }
    }

    NESCENE.setPropertiesInitialized(true);

    return newScn;
}

void FEnginePrivate::createSceneScenarioItem(FScenario* scenario, std::shared_ptr< FZoneItem >& item)
{
    createSceneScnItem( scenario, scenario, item );
}

///
/// Database DB Helper methods
///

///
/// \brief FEnginePrivate::DeleteItemFromZone
/// \param itemName
///

void FEnginePrivate::DeleteItemFromZone(QString itemName)
{
    QString jsonVal = createJSONFromSceneItemName( itemName );
    if( jsonVal != "" )
        DeleteItemDB( jsonVal );
}

///
/// \brief FEnginePrivate::RemoveItemFromAllScenarios
/// \param itemName
/// \param scenarioName
///

void FEnginePrivate::RemoveItemFromAllScenarios(QString itemName)
{
    QString jsonVal = createJSONFromSceneItemName( itemName );
    if( jsonVal != "" )
    {
        RemoveItemFromAllScenariosDB( jsonVal );
    }
}

FailedItems FEnginePrivate::getFailedItems(uint scenarioID)
{
#if 1
    QString jsonStr = GetScenarioFailureOutputDB(scenarioID);

    QJsonDocument jsonResponse = QJsonDocument::fromJson( jsonStr.toUtf8() );

    FailedItems fitems;
    if(jsonResponse.isArray())
    {
        QJsonArray jsonArray = jsonResponse.array();
        int jsonSize = jsonArray.size();

        for( int i = 0; i < jsonSize; ++i )
        {
            QJsonValueRef r = jsonArray[i];
            QJsonObject   jsonObject = r.toObject();
            fitems.scenarioID.append( jsonObject["scenarioID"].toInt() );
            fitems.failedItemID.append( jsonObject["failedItemID"].toInt() );
            fitems.failedTime.append ( jsonObject["failedTime"].toDouble() );
            fitems.failOutput.append( jsonObject["type"].toString() );
        }
    }
#else
    FailedItems fitems;
    fitems.failOutput.push_back("faield output");
    fitems.failedTime.push_back(5);
    fitems.failedItemID.push_back(114);
#endif
    return fitems;
}

void FEnginePrivate::updateItemsWithFailureInfo(const FailedItems& failureInfo)
{
    QList<FZoneItem*> items = NESCENE.findChildren<FZoneItem*>();

    foreach (FZoneItem* item, items)
    {
        item->resetFailed();

        for (int i = 0; i < failureInfo.failedItemID.size(); i++)
        {
            if (item->ID() == failureInfo.failedItemID[i])
            {
                item->setFailed(failureInfo.failedTime[i]);
                break;
            }
        }
    }
}

void FEnginePrivate::updateActiveZoneWithMaterialInfo()
{
    if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(S->activeZoneName())))
    {
        if (activeZone->WallMaterial().uniqueID() != 0 || activeZone->CeilingMaterial().uniqueID() != 0 || activeZone->FloorMaterial().uniqueID() != 0)
        {
            for (NESurfaceMaterial& sm : m_SurfaceMaterialsWithAssignedDefaults)
            {
                if (sm.uniqueID() == activeZone->WallMaterial().uniqueID())
                    activeZone->SetWallMaterial(sm);
                if (sm.uniqueID() == activeZone->CeilingMaterial().uniqueID())
                    activeZone->SetCeilingMaterial(sm);
                if (sm.uniqueID() == activeZone->FloorMaterial().uniqueID())
                    activeZone->SetFloorMaterial(sm);
            }
        }
    }
}

void FEnginePrivate::updateItemsWithMaterialInfo()
{
    QList<FZoneItem*> items = NESCENE.findChildren<FZoneItem*>();
    foreach (FZoneItem* item, items)
    {
        if (item->Material().uniqueID() == 0)
            continue;

        for (NESurfaceMaterial& sm : m_SurfaceMaterialsWithAssignedDefaults)
        {
            if (sm.uniqueID() == item->Material().uniqueID())
                item->SetMaterial(sm);
        }
    }
}

///
/// \brief FEnginePrivate::updateItemsWithCompartmentInfo
///

void FEnginePrivate::updateItemsWithCompartmentInfo()
{
    QList<FZoneItem*> items = NESCENE.findChildren<FZoneItem*>();
    foreach (FZoneItem* item, items)
        updateItemWithCompartmentInfo(item);
}

///
/// \brief FEnginePrivate::updateItemWithCompartmentInfo
///

void FEnginePrivate::updateItemWithCompartmentInfo(FZoneItem* item)
{
    // Set by default compartment ID to 0, meaning that it is not under any compartment
    item->SetCompartmentID(0);
    item->resetGhostPosition();

    if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(S->activeZoneName())))
    {
        if (activeZone->aabb().isInside(item->Position()))
            item->SetCompartmentID(activeZone->ID());

        item->computeGhostPosition(activeZone);
    }
}

///
/// \brief FEnginePrivate::updateVentsWithZoneInfo
///

void FEnginePrivate::updateVentsWithZoneInfo()
{
    QList<FVent*> vents = NESCENE.findChildren<FVent*>();
    foreach (FVent* vent, vents)
        updateVentWithZoneInfo(vent);
}

///
/// \brief FEnginePrivate::updateVentWithZoneInfo
///

void FEnginePrivate::updateVentWithZoneInfo(FVent* vent)
{
    vent->resetCompartments();

    for (int i = 0; i != m_ZoneIdNamePairs.size(); i++)
    {
        vent->addCompartment(m_ZoneIdNamePairs.at(i).second);
        if (m_ZoneIdNamePairs.at(i).first == vent->firstCompartmentId())
            vent->setFirstCompartmentFromName(m_ZoneIdNamePairs.at(i).second, S->activeZoneName());
        if (m_ZoneIdNamePairs.at(i).first == vent->secondCompartmentId())
            vent->setSecondCompartmentFromName(m_ZoneIdNamePairs.at(i).second, S->activeZoneName());
    }

    // Enforce the active zone to be either one of the two compartments
    vent->setFirstCompartmentFromName(vent->FirstCompartment().curName(), S->activeZoneName());
    vent->setSecondCompartmentFromName(vent->SecondCompartment().curName(), S->activeZoneName());

    vent->resetGhostPosition();
    if (FZone* activeZone = qobject_cast<FZone*>(NESCENE.findChildNode(S->activeZoneName())))
        vent->computeGhostPosition(activeZone);
}

///
/// \brief FEnginePrivate::updateVentsWithCriterionInfo
///

void FEnginePrivate::updateVentsWithCriterionInfo()
{
//    QList<FVent*> vents = NESCENE.findChildren<FVent*>();
//    foreach (FVent* vent, vents)
//        updateVentWithCriterionInfo(vent);
}

///
/// \brief FEnginePrivate::updateVentWithCriterionInfo
/// \param vent
///

void FEnginePrivate::updateVentWithCriterionInfo(FVent* vent)
{
//    QList<NEVentCriteria*> criteria = NESCENE.findChildren<NEVentCriteria*>();
//    for (int i = 0; i != criteria.size(); i++)
//    {
//        vent->addCompartment(criteria.at(i));
//        if (compIdList.at(i) == vent->firstCompartmentId())
//            vent->setFirstCompartmentFromName(compNameList.at(i));
    //    }
}


///
/// \brief FEnginePrivate::GetScenarioListForZone
/// \param zoneID
/// \return
///

QStringList FEnginePrivate::GetScenarioListForZone( int zoneID )
{
    QString sString = GetScenariosForZoneDB( zoneID );
    QStringList retval;

    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( sString.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return QStringList();
    }

    QJsonArray jsonArray = jsonResponse.array();

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject = r.toObject();
        QString sname       = jsonObject["cleanName"].toString();


        retval.push_back( sname );
    }

    return retval;
}

///
/// \brief FEnginePrivate::GetZoneListForScenario
/// \param zoneID
/// \return
///

QStringList FEnginePrivate::GetZoneListForScenario( int scnID )
{
    QString sString = GetZonesForScenarioDB( scnID );
    QStringList retval;

    //
    // First create a JSON Object from a string.
    //
    QJsonDocument jsonResponse = QJsonDocument::fromJson( sString.toUtf8() );
    if( jsonResponse.isNull() )
    {
        return QStringList();
    }

    QJsonArray jsonArray = jsonResponse.array();

    for( int i = 0; i < jsonArray.size(); ++i )
    {
        QJsonValueRef r = jsonArray[i];

        QJsonObject   jsonObject = r.toObject();
        QString zname       = jsonObject["cleanName"].toString();

        retval.push_back( zname );
    }

    return retval;
}

QList<QPair<int, QString>> FEnginePrivate::getZoneIdNamePairs() const
{
    QString json = const_cast<FEnginePrivate*>(this)->GetZonesDB();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    if (jsonDoc.isNull())
        return QList<QPair<int, QString>>();

    QList<QPair<int, QString>> zoneIdNamePairs;
    for (int i = 0; i != jsonDoc.array().size(); ++i)
    {
        QJsonObject jsonObj = jsonDoc.array()[i].toObject();
        zoneIdNamePairs.append(QPair<int, QString>(jsonObj["connectorID"].toInt(), jsonObj["cleanName"].toString()));
    }

    return zoneIdNamePairs;
}

QStringList FEnginePrivate::zoneNames() const
{
    QStringList zoneNames;
    for (const QPair<int, QString>& zoneIdNamePair : m_ZoneIdNamePairs)
    {
        zoneNames.push_back(zoneIdNamePair.second);
    }

    return zoneNames;
}

QList<QPair<int, QString>> FEnginePrivate::getScnIdNamePairs(QList<FRANXProperties>& franxScns) const
{
    franxScns.clear();

    QString json = const_cast<FEnginePrivate*>(this)->GetScenariosDB();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());

    if (jsonDoc.isNull())
        return QList<QPair<int, QString>>();

    QList<QPair<int, QString>> scnIdNamePairs;
    for (int i = 0; i != jsonDoc.array().size(); ++i)
    {
        QJsonObject jsonObj = jsonDoc.array()[i].toObject();
        scnIdNamePairs.append(QPair<int, QString>(jsonObj["connectorID"].toInt(), jsonObj["cleanName"].toString()));

        FRANXProperties franxProp;
        franxProp.m_CleanName = jsonObj["cleanName"].toString();
        franxProp.m_Name = jsonObj["cleanName"].toString();
        franxProp.m_Desc = jsonObj["desc"].toString();
        franxProp.m_ID = jsonObj["connectorID"].toInt();
        franxScns.push_back(franxProp);
    }

    return scnIdNamePairs;
}


QStringList FEnginePrivate::scnNames() const
{
    QStringList scnNames;
    for (const QPair<int, QString>& scnIdNamePair : m_ScnIdNamePairs)
    {
        scnNames.push_back(scnIdNamePair.second);
    }

    return scnNames;
}



int FEnginePrivate::indexOfScenario(QString scenarioName) const
{
    QStringList sceneNames = scnNames();
    return sceneNames.indexOf( scenarioName );
}


////////////////////////////////////////////
/// Database query and manipulation methods
/////////////////////////////////////////////

////
/// \brief FEnginePrivate::NewDB
/// \param newdbcstr
/// \return
///


bool FEnginePrivate::NewDB( const char *newdbcstr )
{
    //
    // Create a New Project with that Database file
    //
    f_NewProject NewProject = (f_NewProject)m_pLibrary.resolve("NewProject");
    if (!NewProject)
    {
        std::cerr << "could not locate NewProject function" << std::endl;
        return NE::kReturnFail;
    }

    bool retval = NewProject( const_cast< char *>( newdbcstr )  );

    return retval;
}



/// \brief FEnginePrivate::ImportPlantProjectsDB
/// \param projpath
/// \param franxpath
/// \param pdmspath
/// \return
///

bool FEnginePrivate::ImportPlantProjectsDB( QString projpath, QString franxpath, QString pdmspath )
{
    //
    // Create a New Project with that Database files
    //
    f_ImportPlantProjects IImportProject = (f_ImportPlantProjects) m_pLibrary.resolve("ImportPlantProjects");
    if (!IImportProject)
    {
        std::cerr << "could not locate ImportPlantProjects function" << std::endl;
        return NE::kReturnFail;
    }

    std::string s1 = projpath.toStdString();
    std::string s2 = franxpath.toStdString();
    std::string s3 = pdmspath.toStdString();

    bool retval = IImportProject( &s1[0], &s2[0], &s3[0] );

    emit NEBASE.setCurrentSubOperation( "Imported Plant Databases" );

    return retval;
}

///
/// \brief FEnginePrivate::OpenDB
/// \return
///

bool FEnginePrivate::OpenDB(const char *newdbcstr, bool recover )
{
    //
    // Create a New Project with that Database file
    //
    f_OpenProject OpenProject = (f_OpenProject)m_pLibrary.resolve("OpenProject");
    if (!OpenProject)
    {
        std::cerr << "could not locate the OpenProject function" << std::endl;
        return NE::kReturnFail;
    }

    emit NEBASE.setCurrentSubOperation( "Loading Database" );

//    std::cout << "Loading Database... " << newdbcstr << std::endl;

    bool retval = OpenProject( const_cast< char *>( newdbcstr ), recover );

    emit NEBASE.setCurrentSubOperation( "Loaded Database" );

    return retval;
}


bool FEnginePrivate::SaveProjectAsDB(QString filename)
{
    f_SaveProjectAs saveProjectAs = (f_SaveProjectAs)m_pLibrary.resolve("SaveProjectAs");
    if (!saveProjectAs)
    {
        std::cerr << "could not locate the function SaveProjectAs" << std::endl;
        return false;
    }

    std::string s = filename.toStdString();

    saveProjectAs( &s[0])  ;

    return true;
}


bool FEnginePrivate::SetTempWorkLocDB(QString dirName)
{
    f_SetTempWorkLoc setTempWorkLoc = (f_SetTempWorkLoc)m_pLibrary.resolve("SetTempWorkLoc");
    if (!setTempWorkLoc)
    {
        std::cerr << "could not locate the function SetTempWorkLoc" << std::endl;
        return false;;
    }

    std::string s = dirName.toStdString();

    setTempWorkLoc( &s[0]) ;

    return true;

}



QString FEnginePrivate::GetTempWorkLocDB()
{
    //
    // Temp work location set
    //
    f_GetTempWorkLoc getTempWorkLoc = (f_GetTempWorkLoc)m_pLibrary.resolve("GetTempWorkLoc");
    if (!getTempWorkLoc)
    {
        std::cerr << "could not locate the function GetTempWorkLoc" << std::endl;
        return QString("");
    }

    std::string s = "*";

    char* workloc = getTempWorkLoc( ) ;
    QString zString = QString( workloc );


    return zString;
}

bool FEnginePrivate::SaveTempWorkDB()
{
    f_SaveTempWork saveTempWork = (f_SaveTempWork)m_pLibrary.resolve("SaveTempWork");
    if (!saveTempWork)
    {
        std::cerr << "could not locate the function SaveTempWork" << std::endl;
        return false;
    }


    saveTempWork();

    return true;
}

///
/// \brief FEnginePrivate::ProjectHasRecover
/// \return
///

bool FEnginePrivate::ProjectHasRecoverDB(QString path)
{
    f_ProjectHasRecover projectHasRecover = (f_ProjectHasRecover)m_pLibrary.resolve("ProjectHasRecover");
    if (!projectHasRecover)
    {
        std::cerr << "could not locate the function projectHasRecover" << std::endl;
        return false;
    }

    std::string s = path.toStdString();

    bool retval = projectHasRecover( &s[0] );

    return retval;

}


///
/// Get the JSON for Zones
///

QString FEnginePrivate::GetZonesDB()
{
    //
    // Resolve Zones and test it
    //
    f_GetZonesFunc GetZones = (f_GetZonesFunc)m_pLibrary.resolve("GetZones");
    if (!GetZones)
    {
        std::cerr << "could not locate the function GetZones" << std::endl;
        return QString("");
    }

    std::string s = "*";

    char* zones = GetZones( &s[0]) ;
    QString zString = QString( zones );


    return zString;
}

///
/// \brief FEnginePrivate::GetScenariosDB
/// \return
///

QString FEnginePrivate::GetScenariosDB()
{
    //
    // Resolve Scenarios and test it
    //
    f_GetScenariosFunc GetScenarios = (f_GetZonesFunc)m_pLibrary.resolve("GetScenarios");
    if (!GetScenarios)
    {
        std::cerr << "could not locate the function GetScenarios" << std::endl;
        return QString("");
    }

    std::string s = "*";
    char* scenarios = GetScenarios( &s[0] ) ;

    return QString( scenarios );
}



///
/// Get JSON for Scenarios based on Zone
///

QString FEnginePrivate::GetScenariosForZoneDB( int zoneID )
{
    //
    // Find the scenarios with that zone
    //
    f_GetZonesForScenarioFunc GetScenariosForZone = (f_GetZonesForScenarioFunc)m_pLibrary.resolve("GetScenariosForZone");
    if (!GetScenariosForZone)
    {
        std::cerr << "could not locate the function GetScenariosForZone" << std::endl;
        return QString ("");
    }
    char* scenarios = GetScenariosForZone( zoneID ) ;
    QString sString = QString( scenarios );

    return sString;
}

///
/// \brief FEnginePrivate::NewModelItemDB
/// \param jsonStr
/// \param zoneID
///
int FEnginePrivate::NewModelItemDB( const QString jsonStr, int zoneID, int parentID )
{
    f_NewModelItemFunc NModelItem =
            (f_NewModelItemFunc)m_pLibrary.resolve("NewModelItem");
    if (!NModelItem)
    {
        std::cerr << "could not locate the function SaveModelItem" << std::endl;
        return -1 ;
    }

    std::string s = jsonStr.toStdString();
    int tid = NModelItem( &s[0], zoneID, parentID ) ;
    return tid;
}

///
/// \brief FEnginePrivate::SaveModelItemDB
/// \param jsonStr
/// \param zoneID
///
int FEnginePrivate::SaveModelItemDB( const QString jsonStr, int zoneID )
{
    f_SaveModelItemFunc SModelItem =
            (f_SaveModelItemFunc)m_pLibrary.resolve("SaveModelItem");
    if (!SModelItem)
    {
        std::cerr << "could not locate the function SaveModelItem" << std::endl;
        return -1 ;
    }

    std::string s = jsonStr.toStdString();
    int tid = SModelItem( &s[0], zoneID ) ;
    return tid;
}

///
/// \brief FEnginePrivate::SaveModelItemsDB
/// \param jsonStrArray
/// \param zoneID
///
void FEnginePrivate::SaveModelItemsDB(const QString jsonStrArray, int zoneID)
{
    f_SaveModelItemsFunc SaveModelItems =
            (f_SaveModelItemsFunc)m_pLibrary.resolve("SaveModelItems");
    if (!SaveModelItems)
    {
        std::cerr << "could not locate the function SaveModeItems" << std::endl;
        return ;
    }

    std::string s = jsonStrArray.toStdString();
    SaveModelItems( &s[0], zoneID ) ;
}

///
/// \brief FEnginePrivate::DeleteItemDB
/// \param jsonStr
///

void FEnginePrivate::DeleteItemDB( QString jsonStr )
{
    f_DeleteItemFunc DeleteItem =
            (f_DeleteItemFunc) m_pLibrary.resolve("DeleteItem");
    if (!DeleteItem)
    {
        std::cerr << "could not locate the function DeleteItem" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    DeleteItem( &s[0] ) ;

    emit NEBASE.updateSceneInspector();
}

///
/// \brief FEnginePrivate::RemoveItemDB
/// \param jsonStr
/// \param parentID
///

void FEnginePrivate::RemoveItemDB( QString jsonStr, int parentID )
{
    f_RemoveItem RemoveItem =
            (f_RemoveItem) m_pLibrary.resolve("RemoveItem");
    if (!RemoveItem)
    {
        std::cerr << "could not locate the function RemoveItem" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    RemoveItem( &s[0], parentID ) ;

    emit NEBASE.updateSceneInspector();
}

///
/// \brief FEnginePrivate::ItemRefCntDB
/// \param jsonStr
///

int FEnginePrivate::ItemRefCntDB(QString jsonStr)
{
    f_ItemReferenceCnt ItemReferenceCnt =
            (f_ItemReferenceCnt) m_pLibrary.resolve("ItemReferenceCnt");
    if (!ItemReferenceCnt)
    {
        std::cerr << "could not locate the function ItemReferenceCnt" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();
    int ret = ItemReferenceCnt( &s[0] ) ;

    return ret;
}

///
/// \brief FEnginePrivate::AddItemToScenarioDB
/// \param jsonStr
/// \param scenarioID
///

void FEnginePrivate::AddItemToScenarioDB(QString jsonStr, int scenarioID , int parentID)
{
    f_AddItemToScenarioFunc AaddItemToScenario =
            (f_AddItemToScenarioFunc)m_pLibrary.resolve("AddItemToScenario");
    if (!AaddItemToScenario)
    {
        std::cerr << "could not locate the function AddItemToScenario" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    AaddItemToScenario( &s[0], scenarioID, parentID ) ;
}

///
/// \brief FEnginePrivate::RemoveItemFromScenarioDB
/// \param jsonStr
/// \param scenarioID
///

void FEnginePrivate::RemoveItemFromScenarioDB(QString jsonStr, int scenarioID , int parentID)
{
    f_RemoveScenarioItemFunc RremoveScenarioItem =
            (f_RemoveScenarioItemFunc)m_pLibrary.resolve("RemoveItemFromScenario");
    if (!RremoveScenarioItem)
    {
        std::cerr << "could not locate the function RemoveItemFromScenario" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    RremoveScenarioItem( &s[0], scenarioID, parentID );

    emit NEBASE.updateSceneInspector();
}

void FEnginePrivate::RemoveItemFromAllScenariosDB( QString jsonStr )
{
    f_RemoveItemFromAllScenariosFunc RemoveItemAllScenariosItem =
            (f_RemoveItemFromAllScenariosFunc)m_pLibrary.resolve("RemoveItemFromAllScenarios");
    if (!RemoveItemAllScenariosItem)
    {
        std::cerr << "could not locate the function RemoveItemFromAllScenarios" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    RemoveItemAllScenariosItem( &s[0] ) ;

    emit NEBASE.updateSceneInspector();
}

///
/// \brief FEnginePrivate::SaveScenarioAsDB
/// \param jsonStr
/// \param newName
///

void FEnginePrivate::SaveScenarioAsDB(QString jsonStr, char *newName)
{
    f_SaveScenarioAsFunc SaveScenarioAs =
            (f_SaveScenarioAsFunc)m_pLibrary.resolve("SaveScenarioAs");
    if (!SaveScenarioAs)
    {
        std::cerr << "could not locate the function SaveScenarioAs" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    SaveScenarioAs( &s[0], newName ) ;
}

///
/// \brief FEnginePrivate::NewScenarioDB
/// \param jsonStr
/// \param zoneID
///

int FEnginePrivate::NewScenarioDB(QString jsonStr, int zoneID)
{
    f_NewScenarioFunc NewScenario =
            (f_NewScenarioFunc)m_pLibrary.resolve("NewScenario");
    if (!NewScenario)
    {
        std::cerr << "could not locate the function NewScenario" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();
    int scnID = NewScenario( &s[0], zoneID ) ;

    return scnID;
}

///
/// \brief FEnginePrivate::DeleteScenarioDB
/// \param jsonStr
///

void FEnginePrivate::DeleteScenarioDB(QString jsonStr)
{
    f_DeleteScenarioFunc DeleteScenario =
            (f_DeleteScenarioFunc)m_pLibrary.resolve("DeleteScenario");
    if (!DeleteScenario)
    {
        std::cerr << "could not locate the function DeleteScenario" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();
    DeleteScenario( &s[0] ) ;
}

///
/// \brief FEnginePrivate::GetFRANXTreeDB
/// \param zoneID
/// \param scenarioID
///

QString FEnginePrivate::GetFRANXTreeDB(int zoneID, int scenarioID)
{
    f_GetFRANXTreeFunc GetFRANXTree =
            (f_GetFRANXTreeFunc)m_pLibrary.resolve("GetFranxTree");
    if (!GetFRANXTree)
    {
        std::cerr << "could not locate the function GetFranxTree" << std::endl;
        return "";
    }

    char *franxtree = GetFRANXTree( zoneID, scenarioID ) ;
    return QString( franxtree );

}

///
/// \brief FEnginePrivate::ExcludeFRANXItemDB
/// \param jsonStr
/// \param scenarioID
/// \param parentID
///

void FEnginePrivate::ExcludeFRANXItemDB(QString jsonStr, int scenarioID, int parentID)
{
    f_ExcludeFRANXItemFunc ExcludeFranxItem =
            (f_ExcludeFRANXItemFunc)m_pLibrary.resolve("ExcludeFranxItem");
    if (!ExcludeFranxItem)
    {
        std::cerr << "could not locate the function ExcludeFranxItem" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();

    ExcludeFranxItem( &s[0], scenarioID, parentID ) ;

    return;
}

///
/// \brief FEnginePrivate::IncludeFRANXItemDB
/// \param jsonStr
/// \param scenarioID
/// \param parentID
///

void FEnginePrivate::IncludeFRANXItemDB(QString jsonStr, int scenarioID, int parentID)
{
    f_IncludeFRANXItemFunc IncludeFranxItem =
            (f_IncludeFRANXItemFunc)m_pLibrary.resolve("IncludeFranxItem");
    if (!IncludeFranxItem)
    {
        std::cerr << "could not locate the function IncludeFranxItem" << std::endl;
        return ;
    }

    std::string s = jsonStr.toStdString();

    IncludeFranxItem( &s[0], scenarioID, parentID ) ;

    return;
}

///
/// \brief FEnginePrivate::GetBaseFRANXModelLoc
/// \return
///

QString FEnginePrivate::GetBaseFRANXModelLocDB()
{
    f_GetBaseFRANXModelLoc baseFRANXModelLoc =
            (f_GetBaseFRANXModelLoc)m_pLibrary.resolve("GetBaseFRANXModelLoc");
    if (!baseFRANXModelLoc)
    {
        std::cerr << "could not locate the function GetBaseFRANXModelLoc" << std::endl;
        return "";
    }

    char *franxloc = baseFRANXModelLoc() ;
    return QString( franxloc );
}

QString FEnginePrivate::GetFranxMasterLocDB()
{
    f_GetFranxMasterLoc getFRANXMasterLoc =
            (f_GetFranxMasterLoc)m_pLibrary.resolve("GetFranxMasterLoc");
    if ( !getFRANXMasterLoc )
    {
        std::cerr << "could not locate the function GetFranxMasterLoc" << std::endl;
        return "";
    }

    char *franxloc = getFRANXMasterLoc() ;
    return QString( franxloc );
}

bool FEnginePrivate::SetFranxMasterLocDB( QString franxloc)
{
    f_SetFranxMasterLoc setFRANXMasterLoc =
            (f_SetFranxMasterLoc)m_pLibrary.resolve("SetFranxMasterLoc");
    if ( !setFRANXMasterLoc )
    {
        std::cerr << "could not locate the function SetFranxMasterLoc" << std::endl;
        return false;
    }

    bool ret = setFRANXMasterLoc( franxloc.toUtf8().data() ) ;
    return ret;
}


bool FEnginePrivate::GetFranxMasterChangedDB()
{
    f_GetFranxMasterChanged getFranxMasterChanged =
            (f_GetFranxMasterChanged)m_pLibrary.resolve("GetFranxMasterChanged");
    if ( !getFranxMasterChanged )
    {
        std::cerr << "could not locate the function GetFranxMasterLoc" << std::endl;
        return false;
    }

    bool ret = getFranxMasterChanged() ;
    return ret;
}

///
/// \brief FEnginePrivate::RunFranxScenario
/// \param scenarioID
/// \param basefranx
/// \param saveAs
///

void FEnginePrivate::RunFranxScenarioDB(int scenarioID, QString basefranx, QString saveAs)
{
    f_RunFranxScenario runFranxScenario =
            (f_RunFranxScenario)m_pLibrary.resolve("RunFranxScenario");
    if (!runFranxScenario)
    {
        std::cerr << "could not locate the function RunFranxScenario" << std::endl;
        return ;
    }

    runFranxScenario( scenarioID, basefranx.toUtf8().data(), saveAs.toUtf8().data()) ;
    return;
}

///
/// \brief FEnginePrivate::RunFranxScenariosDB
/// \param jsonStr
///

QString FEnginePrivate::RunFranxScenariosDB(QString jsonStr)
{
    f_RunFranxScenarios runFranxScenarios =
            (f_RunFranxScenarios)m_pLibrary.resolve("RunFranxScenarios");
    if (!runFranxScenarios)
    {
        std::cerr << "could not locate the function RunFranxScenarios" << std::endl;
        return QString("");
    }

    std::string s = jsonStr.toStdString();

    char *franxret = runFranxScenarios( jsonStr.toUtf8().data()) ;
    return QString( franxret );
}

///
/// \brief FEnginePrivate::GetDefaultMaterialPropsDB
/// \return
///

QString FEnginePrivate::GetDefaultMaterialPropsDB()
{
    f_GetDefaultMaterialProps defmaterialProps =
            (f_GetDefaultMaterialProps)m_pLibrary.resolve("GetDefaultMaterialProps");
    if (!defmaterialProps)
    {
        std::cerr << "could not locate the function GetDefaultMaterialProps" << std::endl;
        return "";
    }

    char *defmaterialPropsRet = defmaterialProps(true) ;
    return QString( defmaterialPropsRet );
}

///
/// \brief FEnginePrivate::GetMaterialPropsDB
/// \return
///

QString FEnginePrivate::GetMaterialPropsDB()
{
    f_GetMaterialProps materialProps =
            (f_GetMaterialProps)m_pLibrary.resolve("GetMaterialProps");
    if (!materialProps)
    {
        std::cerr << "could not locate the function GetMaterialProps" << std::endl;
        return "";
    }

    char *materialPropsRet = materialProps() ;
    return QString( materialPropsRet );
}

QString FEnginePrivate::SaveMaterialPropsDB(QString jsonStr)
{
    f_SaveMaterialProps saveMaterialPropsf =
            (f_SaveMaterialProps)m_pLibrary.resolve("SaveMaterialProps");
    if (!saveMaterialPropsf)
    {
        std::cerr << "could not locate the function SaveMaterialProps" << std::endl;
        return "";
    }

    std::string s = jsonStr.toStdString();

    char *saveMaterialPropsRet = saveMaterialPropsf(&s[0]) ;
    return QString( saveMaterialPropsRet );
}

int FEnginePrivate::SaveSingleMaterialPropsDB(QString jsonStr)
{
    f_SaveSingleMaterialProps saveSingleMaterialPropsf =
            (f_SaveSingleMaterialProps)m_pLibrary.resolve("SaveSingleMaterialProp");
    if (!saveSingleMaterialPropsf)
    {
        std::cerr << "could not locate the function SaveSingleMaterialProp" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSingleMaterialPropsRet = saveSingleMaterialPropsf(&s[0]) ;
    return saveSingleMaterialPropsRet;
}

QString FEnginePrivate::DeleteMaterialPropsDB(QString jsonStr)
{
    f_DeleteMaterialProps deleteMaterialPropsf =
            (f_DeleteMaterialProps)m_pLibrary.resolve("DeleteMaterialProps");
    if (!deleteMaterialPropsf)
    {
        std::cerr << "could not locate the function DeleteMaterialProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *deleteMaterialPropsRet = deleteMaterialPropsf(&s[0]) ;
    return QString( deleteMaterialPropsRet );;
}

QString FEnginePrivate::GetFireMaterialPropsDB()
{
    f_GetFireMaterialProps firematerialProps =
            (f_GetFireMaterialProps)m_pLibrary.resolve("GetFireMaterialProps");
    if (!firematerialProps)
    {
        std::cerr << "could not locate the function GetFireMaterialProps" << std::endl;
        return "";
    }

    char *firematerialPropsRet = firematerialProps() ;
    return QString( firematerialPropsRet );
}

QString FEnginePrivate::SaveFireMaterialPropsDB(QString jsonStr)
{
    f_SaveFireMaterialProps saveFireMaterialPropsf =
            (f_SaveFireMaterialProps)m_pLibrary.resolve("SaveFireMaterialProps");
    if (!saveFireMaterialPropsf)
    {
        std::cerr << "could not locate the function SaveFireMaterialProps" << std::endl;
        return "";
    }

    std::string s = jsonStr.toStdString();

    char *saveFireMaterialPropsRet = saveFireMaterialPropsf(&s[0]) ;
    return QString( saveFireMaterialPropsRet );
}

int FEnginePrivate::SaveSingleFireMaterialPropsDB(QString jsonStr)
{
    f_SaveSingleFireMaterialProps saveSingleFireMaterialPropsf =
            (f_SaveSingleFireMaterialProps)m_pLibrary.resolve("SaveSingleFireMaterialProp");
    if (!saveSingleFireMaterialPropsf)
    {
        std::cerr << "could not locate the function SaveSingleFireMaterialProp" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSingleFireMaterialPropsRet = saveSingleFireMaterialPropsf(&s[0]) ;
    return saveSingleFireMaterialPropsRet;
}

QString FEnginePrivate::DeleteFireMaterialPropsDB(QString jsonStr)
{
    f_DeleteFireMaterialProps deleteFireMaterialPropsf =
            (f_DeleteFireMaterialProps)m_pLibrary.resolve("DeleteFireMaterialProps");
    if (!deleteFireMaterialPropsf)
    {
        std::cerr << "could not locate the function DeleteFireMaterialProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *deleteFireMaterialPropsRet = deleteFireMaterialPropsf(&s[0]) ;
    return QString( deleteFireMaterialPropsRet );;
}

QString FEnginePrivate::GetSimInfoPropsDB()
{
    f_GetSimInfoProps simInfoProps =
            (f_GetSimInfoProps)m_pLibrary.resolve("GetSimInfoProp");
    if (!simInfoProps)
    {
        std::cerr << "could not locate the function GetSimInfoProp" << std::endl;
        return "";
    }

    char *simInfoPropsRet = simInfoProps() ;
    return QString( simInfoPropsRet );
}


int FEnginePrivate::SaveSimInfoPropsDB( QString simInfoProps )
{
    f_SaveSimInfoProps savesimInfoProps =
            (f_SaveSimInfoProps)m_pLibrary.resolve("SaveSimInfoProp");
    if (!savesimInfoProps)
    {
        std::cerr << "could not locate the function SaveSimInfoProp" << std::endl;
        return 0;
    }

    std::string s = simInfoProps.toStdString();

    bool simInfoPropsRet = savesimInfoProps( &s[0] ) ;
    return int( simInfoPropsRet );
}


QString FEnginePrivate::GetFailureCriteriaPropsDB()
{
    f_GetFailureCriteriaProps failureCriteriaPropsf =
            (f_GetFailureCriteriaProps)m_pLibrary.resolve("GetFailureCriteriaProps");
    if (!failureCriteriaPropsf)
    {
        std::cerr << "could not locate the function GetFailureCriteriaProps" << std::endl;
        return "";
    }

    char *getFailureCriteriaPropsRet = failureCriteriaPropsf() ;
    return QString( getFailureCriteriaPropsRet );
}

QString FEnginePrivate::SaveFailureCriteriaPropsDB(QString jsonStr)
{
    f_SaveFailureCriteriaProps saveFailureCriteriaPropsf =
            (f_SaveFailureCriteriaProps)m_pLibrary.resolve("SaveFailureCriteriaProps");
    if (!saveFailureCriteriaPropsf)
    {
        std::cerr << "could not locate the function SaveFailureCriteriaProps" << std::endl;
        return "";
    }

    std::string s = jsonStr.toStdString();

    char *saveFailureCriteriaPropsRet = saveFailureCriteriaPropsf( &s[0]) ;
    return QString( saveFailureCriteriaPropsRet );
}

int FEnginePrivate::SaveSingleFailureCriteriaPropsDB(QString jsonStr)
{
    f_SaveSingleFailureCriteriaProps saveSingleFailureCriteriaPropsf =
            (f_SaveSingleFailureCriteriaProps)m_pLibrary.resolve("SaveSingleFailureCriteriaProp");
    if (!saveSingleFailureCriteriaPropsf)
    {
        std::cerr << "could not locate the function SaveSingleFailureCriteriaProp" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSingleFailureCriteriaPropsRet = saveSingleFailureCriteriaPropsf(&s[0]) ;
    return saveSingleFailureCriteriaPropsRet;
}

QString FEnginePrivate::DeleteFailureCriteriaPropsDB(QString jsonStr)
{
    f_DeleteFailureCriteriaProps deleteFailureCriteriaPropsf =
            (f_DeleteFailureCriteriaProps)m_pLibrary.resolve("DeleteFailureCriteriaProps");
    if (!deleteFailureCriteriaPropsf)
    {
        std::cerr << "could not locate the function DeleteFailureCriteriaProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *deleteFailureCriteriaPropsRet = deleteFailureCriteriaPropsf(&s[0]) ;
    return QString( deleteFailureCriteriaPropsRet );;
}

QString FEnginePrivate::GetCriteriaPropsDB()
{
    f_GetCriteriaProps criteriaPropsf =
            (f_GetCriteriaProps)m_pLibrary.resolve("GetCriteriaProps");
    if (!criteriaPropsf)
    {
        std::cerr << "could not locate the function GetCriteriaProps" << std::endl;
        return "";
    }

    char *getCriteriaPropsRet = criteriaPropsf() ;
    return QString( getCriteriaPropsRet );
}

QString FEnginePrivate::SaveCriteriaPropsDB(QString jsonStr)
{
    f_SaveCriteriaProps saveCriteriaPropsf =
            (f_SaveCriteriaProps)m_pLibrary.resolve("SaveCriteriaProps");
    if (!saveCriteriaPropsf)
    {
        std::cerr << "could not locate the function SaveCriteriaProps" << std::endl;
        return "";
    }

    std::string s = jsonStr.toStdString();

    char *saveCriteriaPropsRet = saveCriteriaPropsf( &s[0]) ;
    return QString( saveCriteriaPropsRet );
}

int FEnginePrivate::SaveSingleCriteriaPropsDB(QString jsonStr)
{
    f_SaveSingleCriteriaProps saveSingleCriteriaPropsf =
            (f_SaveSingleCriteriaProps)m_pLibrary.resolve("SaveSingleCriteriaProp");
    if (!saveSingleCriteriaPropsf)
    {
        std::cerr << "could not locate the function SaveSingleCriteriaProp" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSingleCriteriaPropsRet = saveSingleCriteriaPropsf(&s[0]) ;
    return saveSingleCriteriaPropsRet;
}

QString FEnginePrivate::DeleteCriteriaPropsDB(QString jsonStr)
{
    f_DeleteCriteriaProps deleteCriteriaPropsf =
            (f_DeleteCriteriaProps)m_pLibrary.resolve("DeleteCriteriaProps");
    if (!deleteCriteriaPropsf)
    {
        std::cerr << "could not locate the function DeleteCriteriaProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *deleteCriteriaPropsRet = deleteCriteriaPropsf(&s[0]) ;
    return QString( deleteCriteriaPropsRet );;
}

QString FEnginePrivate::GetAlarmPropsDB()
{
    f_GetAlarmProps alarmPropsf =
            (f_GetCriteriaProps)m_pLibrary.resolve("GetAlarmProps");
    if (!alarmPropsf)
    {
        std::cerr << "could not locate the function GetAlarmProps" << std::endl;
        return "";
    }

    char *alarmPropsRet = alarmPropsf() ;
    return QString( alarmPropsRet );
}

QString FEnginePrivate::SaveAlarmPropsDB(QString jsonStr)
{
    f_SaveAlarmProps saveAlarmPropsf =
            (f_SaveAlarmProps)m_pLibrary.resolve("SaveAlarmProps");
    if (!saveAlarmPropsf)
    {
        std::cerr << "could not locate the function SaveAlarmProps" << std::endl;
        return "";
    }

    std::string s = jsonStr.toStdString();

    char *saveAlarmPropsRet = saveAlarmPropsf(&s[0]) ;
    return QString( saveAlarmPropsRet );
}

int FEnginePrivate::SaveSingleAlarmPropsDB(QString jsonStr)
{
    f_SaveSingleAlarmProps saveSingleAlarmPropsf =
            (f_SaveSingleAlarmProps)m_pLibrary.resolve("SaveCAlarmProps");
    if (!saveSingleAlarmPropsf)
    {
        std::cerr << "could not locate the function SaveCAlarmProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSingleAlarmPropsRet = saveSingleAlarmPropsf(&s[0]) ;
    return saveSingleAlarmPropsRet;
}

QString FEnginePrivate::DeleteAlarmPropsDB(QString jsonStr)
{
    f_DeleteAlarmProps deleteAlarmPropsf =
            (f_DeleteAlarmProps)m_pLibrary.resolve("DeleteAlarmProps");
    if (!deleteAlarmPropsf)
    {
        std::cerr << "could not locate the function DeleteAlarmProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *deleteAlarmPropsRet = deleteAlarmPropsf(&s[0]) ;
    return QString( deleteAlarmPropsRet );;
}

QString FEnginePrivate::SaveSimulationItemsDB(QString jsonStr, int zoneID)
{
    f_SaveSimulationItems saveSimulationItemsf =
            (f_SaveSimulationItems)m_pLibrary.resolve("SaveSimulationItemProps");
    if (!saveSimulationItemsf)
    {
        std::cerr << "could not locate the function SaveSimulationItemProps" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    char *saveSimItemsRet = saveSimulationItemsf(&s[0], zoneID) ;
    return QString( saveSimItemsRet );
}

int FEnginePrivate::SaveSingleSimulationItemDB(QString jsonStr, int zoneID)
{
    f_SaveSingleSimulationItem saveSingleSimulationItem =
            (f_SaveSingleSimulationItem)m_pLibrary.resolve("SaveSingleSimulationItemProp");
    if (!saveSingleSimulationItem)
    {
        std::cerr << "could not locate the function SaveSingleSimulationItemProp" << std::endl;
        return 0;
    }

    std::string s = jsonStr.toStdString();

    int saveSimItemsRet = saveSingleSimulationItem(&s[0], zoneID) ;
    return saveSimItemsRet ;
}

QString FEnginePrivate::GetSimulationItemsInZoneDB(int zoneID)
{
    f_GetSimulationItemsInZone getss =
            (f_GetSimulationItemsInZone)m_pLibrary.resolve("GetSimulationItemsInZone");
    if (!getss)
    {
        std::cerr << "could not locate the function GetSimulationItemsInZone" << std::endl;
        return 0;
    }


    char *ret = getss(zoneID) ;
    return QString( ret );
}

QString FEnginePrivate::GetSimulationItemsInZoneForTypeDB(int zoneID, QString category)
{
    f_GetSimulationItemsInZoneForType getst =
            (f_GetSimulationItemsInZoneForType)m_pLibrary.resolve("GetSimulationItemsInZoneForType");
    if (!getst)
    {
        std::cerr << "could not locate the function GetSimulationItemsInZoneForType" << std::endl;
        return 0;
    }

    std::string s = category.toStdString();

    char *ret = getst(zoneID, &s[0]) ;
    return QString( ret );
}

void FEnginePrivate::SetCFASTLocDB(QString loc)
{
    f_SetCFASTLoc cfastloc =
            (f_SetCFASTLoc) m_pLibrary.resolve( "SetCFASTLoc" );
    if (!cfastloc)
    {
        std::cerr << "could not locate the function SetCFASTLoc" << std::endl;
        return ;
    }
    std::string s = loc.toStdString();

    cfastloc(&s[0]) ;
    return ;
}

int FEnginePrivate::RunExistingCFASTSimulationDB(QString jsonStr, int zoneID, QString si )
{
    f_RunExistingCFASTSimulation cfast =
            (f_RunExistingCFASTSimulation) m_pLibrary.resolve( "RunExistingCFASTSimulation" );
    if (!cfast)
    {
        std::cerr << "could not locate the function RunExistingCFASTSimulation" << std::endl;
        return 0;
    }
    std::string s = jsonStr.toStdString();
    std::string s1 = si.toStdString();


    int cfastout = cfast(&s[0], zoneID, &s1[0] ) ;
    return cfastout;
}

int FEnginePrivate::RunNewCFASTSimulationDB(QString jsonStr, int zoneID, QString sr, QString si )
{
    f_RunNewCFASTSimulation cfast =
            (f_RunNewCFASTSimulation) m_pLibrary.resolve( "RunNewCFASTSimulation" );
    if (!cfast)
    {
        std::cerr << "could not locate the function RunCFASTSimulation" << std::endl;
        return 0;
    }
    std::string s1 = jsonStr.toStdString();
    std::string s2 = sr.toStdString();
    std::string s3 = si.toStdString();


    int cfastout = cfast(&s1[0], zoneID, &s2[0], &s3[0] ) ;
    return cfastout;
}

QString FEnginePrivate::GetCableSpecsListDB(int category, int gauge, int conductors, int index, int count)
{
    f_GetCableSpecsList gcl =
            (f_GetCableSpecsList) m_pLibrary.resolve( "GetCableSpecsList" );
    if (!gcl)
    {
        std::cerr << "could not locate the function GetCableSpecsList" << std::endl;
        return 0;
    }

    char* ret = gcl( category, gauge, conductors, index, count ) ;
    return QString(ret);
}

int FEnginePrivate::AddCableSpecDB(QString cableInfoJsonStr)
{
    f_AddCableSpec acl =
            (f_AddCableSpec) m_pLibrary.resolve( "AddCableSpec" );
    if (!acl)
    {
        std::cerr << "could not locate the function AddCableSpec" << std::endl;
        return 0;
    }

    std::string s = cableInfoJsonStr.toStdString();

    int ret = acl( &s[0] ) ;
    return ret;
}

QString FEnginePrivate::GetCableSpecOptionsDB()
{
    f_GetCableSpecOptions gco =
            (f_GetCableSpecOptions) m_pLibrary.resolve( "GetCableSpecOptions" );
    if (!gco)
    {
        std::cerr << "could not locate the function GetCableSpecOptions" << std::endl;
        return QString("");
    }

    char* ret = gco() ;
    return ret;
}

QString FEnginePrivate::GetScenarioFailureOutputDB(int scenarioID)
{
    f_GetScenarioFailureOutput scenarioFailureOutput =
            (f_GetScenarioFailureOutput)m_pLibrary.resolve("GetScenarioFailureOutput");
    if (!scenarioFailureOutput)
    {
        std::cerr << "could not locate the function GetScenarioFailureOutput" << std::endl;
        return 0;
    }

    char *scenarioFailureOutputRet = scenarioFailureOutput(scenarioID) ;
    return QString( scenarioFailureOutputRet );;
}

///
/// \brief FEnginePrivate::GetZonesForScenarioDB
/// \param scenarioID
/// \return
///
/// Get the JSON for Zones for Scenario from the Database
///
///

QString FEnginePrivate::GetZonesForScenarioDB( int scenarioID )
{
    //
    // Find the scenarios with that zone
    //
    f_GetScenariosForZoneFunc GetZonesScenario =
            (f_GetScenariosForZoneFunc)m_pLibrary.resolve("GetZonesForScenario");
    if (!GetZonesScenario)
    {
        std::cerr << "could not locate the function GetZonesForScenario" << std::endl;
        return QString("");
    }
    char* zones = GetZonesScenario( scenarioID ) ;
    QString zString = QString( zones );

    return zString;
}

//
// Get JSON for Plant Zone Tree
//

QString FEnginePrivate::GetPlantZoneTreeDB( int zoneID )
{
    f_GetPlantZoneTreeFunc GetPlantZoneTree =
            (f_GetPlantZoneTreeFunc)m_pLibrary.resolve("GetPlantZoneTree");
    if (!GetPlantZoneTree)
    {
        std::cerr << "could not locate the function GetPlantZoneTree" << std::endl;
        return QString( "" );
    }

    char* zonetree = GetPlantZoneTree( zoneID );

    QString zString = QString( zonetree );

    return zString;
}

///
/// \brief FEnginePrivate::GetPlantScenarioTreeDB
/// \param scnID
/// \return
///

QString FEnginePrivate::GetPlantScenarioTreeDB( int scnID )
{
    f_GetPlantScenarioTreeFunc GetPlantScenarioTree =
            (f_GetPlantScenarioTreeFunc) m_pLibrary.resolve("GetPlantScenarioTree");
    if (!GetPlantScenarioTree)
    {
        std::cerr << "could not locate the function GetPlantScenarioTree" << std::endl;
        return "";
    }


    char* scenariotree = GetPlantScenarioTree( scnID ) ;
    QString zString = QString( scenariotree );

    return zString;
}

///
/// \brief FEngineFactory::FEngineFactory
/// \param m
///

FEngineFactory::FEngineFactory(NEManager* m) : NENodeFactory(m)
{

}

///
/// \brief FEngineFactory::~FEngineFactory
///

FEngineFactory::~FEngineFactory()
{

}

///
/// \brief FEngineFactory::nodeName
/// \return
///

QString FEngineFactory::nodeName()
{
    return "Fire Engine";
}

///
/// \brief FEngineFactory::nodeAltNames
/// \return
///

QStringList FEngineFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("Fire Engine");

    return aNames;
}

///
/// \brief FEngineFactory::nodeType
/// \return
///

uint FEngineFactory::nodeType()
{
    return NE::kObjectGroup;
}

///
/// \brief FEngineFactory::nodeInherits
/// \return
///

QString FEngineFactory::nodeInherits()
{
    return "Groups";
}

///
/// \brief FEngineFactory::objectName
/// \return
///

QString FEngineFactory::objectName()
{
    return "FireEngine";
}

///
/// \brief FEngineFactory::nodeVersion
/// \return
///

QString FEngineFactory::nodeVersion()
{
    return "0.1";
}

///
/// \brief FEngineFactory::nodeVendor
/// \return
///

QString FEngineFactory::nodeVendor()
{
    return "FRI3D";
}

///
/// \brief FEngineFactory::nodeHelp
/// \return
///

QString FEngineFactory::nodeHelp()
{
    return "Describing a Fire Database";
}

///
/// \brief FEngineFactory::createInstance
/// \return
///

NEObject* FEngineFactory::createInstance()
{
    return new FEngine(this);
}
