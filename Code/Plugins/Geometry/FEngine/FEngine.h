#pragma once

#include "Utility/FilePath.h"
#include "Mesh/NESpatialEngine.h"
#include "Base/NENodeFactory.h"

#include "Mesh/NEZone.h"
#include "Mesh/NEScenario.h"
#include "Mesh/NEZoneItem.h"
#include "Mesh/NEZoneCompartment.h"

#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFireMaterial.h"
#include "Utility/NEFailureCriteria.h"
#include "Utility/NEVentCriteria.h"
#include "Utility/NECableProperty.h"
#include "Utility/NECustomVariants.h"

#include "FFRANXProperties.h"

NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(FEngine))
NE_FWD_DECL_1(struct, FailedItems)

class FDYNCEXP_PLUGINS FEngine : public NESpatialEngine
{
public:
    Q_OBJECT
    NE_CLASS("Fire Data Engine")

    NE_PROP_RW(FilePath, DBFile, "Database", "Fire Database File", false, )

public:
    FEngine( NENodeFactory* factory );
    virtual ~FEngine();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    void initializeGUIActions() override;

signals:
    void sendProjectRecovery( QString msg, bool recover );
    void sendUpdateSurfaceMaterials( QList<NESurfaceMaterial> smats );
    void sendUpdateSurfaceMaterial( NESurfaceMaterial smat );

    void sendUpdateSimTime(double simtime);

    void sendUpdateFireMaterials( QList<NEFireMaterial> firemats );
    void sendUpdateFireMaterial( NEFireMaterial fmat );

    void sendUpdateFailureCriterias( QList<NEFailureCriteria> fails );
    void sendUpdateFailureCriteria( NEFailureCriteria fail );

    void sendUpdateVentCriterias( QList<NEVentCriteria> criteria );
    void sendUpdateVentCriteria( NEVentCriteria criterion );

    void sendUpdateCableProperties( QList<NECableProperty> props, std::vector<QList<QString>> options );
    void sendUpdateCableProperty( NECableProperty prop );

    void sendUpdateCableOptions(std::vector<QList<QString>> options);

    void sendUpdateComputeScenarios( QList<FRANXProperties> fplist );

    //void allFailedItems( QList<int> failedItemID, QList<double> failedTime, QList<int> scenarioID, QList<QString> failOutput );

    void sendUpdateWorkingFolder( QString tempFolder );
    void sendUpdateUIFolder( QString uiFolder );

    void sendLoadUIData(QString filename);
    void sendSaveUIData(QString filename);

public slots:
    uint importFRANX(QString fileName);
    uint importPDMS(QString fileName);

    uint importPlantData(QString projPath, QString franxPath, QString pdmsPath);

    uint loadDBFile();
    uint reloadDBFile();
    uint saveDBFile();

    virtual void clear() override;
    virtual void newData( QString filename = QString ("" ) ) override;
    virtual void load( QString filename ) override;
    virtual void save( QString filename ) override;
    virtual void quit() override;

    virtual void updateWorkingFolder(QString dirname) override;
    virtual void computeOptions() override;

    void setRecoverData(bool recover);

    virtual QString activeZoneName() override;
    virtual QString activeScenarioName() override;

    void addNewZoneAndScenario(NEZone *zone, NEScenario* scenario);
    void addNewScenario(NEScenario *scenario);

    virtual int zoneCreated( NEZone *newZone ) override;
    virtual int zoneDeleted( NEZone *zone ) override;

    virtual int scenarioCreated( NEScenario *newScn ) override;
    virtual int scenarioDeleted( NEScenario *newScn ) override;

    int componentCreated( NEZoneItem *newZoneItem, NENode* parent = 0);
    virtual int itemCreated( NEZoneItem *newZoneItem, NENode*parent = 0 ) override;
    virtual int itemDeleted(NEZoneItem *newZoneItem , bool force) override;

    virtual void duplicateObject( NEZoneItem *item ) override;

    //! \brief copyProperties
    virtual void copyProperties(NEZoneItem *item) override;
    ///
    /// \brief pasteProperties
    virtual void pasteProperties(QList< NEZoneItem *> items, QString filter ) override;

    int trayCreated(NEZoneItem *newZoneItem, NENode *parent);
    int cableCreated(NEZoneItem *newZoneItem, NENode *parent);

    virtual void addItemToScenario( NEZoneItem* item, NENode *parent ) override;
    virtual void removeItemFromScenario(NEZoneItem *zoneItem, NENode* parent ) override;
    virtual void removeItemFromScenarios(NEZoneItem *zoneItem, QList<NENode*> scnlist ) override;

    virtual void setActiveZone( QString zoneName ) override;
    virtual void setActiveScenario( QString scenarioName ) override;

    virtual void nodeRenamed(NENode *node, QString name) override;

    virtual void saveZone( QString zonename, QString filename ) override;
    virtual void saveScenario( QString scenarioname, QString name ) override;
    virtual uint compute() override;
    virtual uint finishedCompute() ;

    virtual int addCompartment(NENode *node, NENode *parent ) override;

    virtual void updateUIFolder(QString dirname);

    //!
    //! \brief addFire
    //! add a fire description
    //!
    void addFire(NENode* zoneItem) ;

    void convertToSource(NENode* oldNode, NENode* newNode);
    void convertToItem(NENode* prevNode, NENode* newNode);


    ///
    /// \brief addVent
    /// \param zoneItem
    ///
    void addVent(NENode* zoneItem) ;
    void turnIntoVent(NENode* oldNode, NENode* newNode);
    void turnIntoMechVent(NENode* prevNode, NENode* newNode);

    //!
    //! \brief addFireDetection
    //! add alarm or detection unit to the system
    //!
    void addFireDetection(NENode* scnItem) ;

    void loadDefaultSurfaceMaterials();

    void restoreCamera();

    //
    // All material related calls
    //
    QList<NESurfaceMaterial> getDefaultSurfaceMaterials();

    QList<NESurfaceMaterial> getSurfaceMaterials() override;
    void addSurfaceMaterial(NESurfaceMaterial sm) override;
    void removeSurfaceMaterial(NESurfaceMaterial sm) override;
    void assignSurfaceMaterial( NENode *zoneitem, NESurfaceMaterial sm, QString matKind ) override;
    void changeSurfaceMaterial(NESurfaceMaterial sm);

    QList<NEFireMaterial> getFireMaterials() override;
    void addFireMaterial(NEFireMaterial fm) override;
    void removeFireMaterial(NEFireMaterial fm) override;
    void assignFireMaterial(NENode *zoneitem, NEFireMaterial sm ) override;

    QList<NEFailureCriteria> getFailureCriteria() override;
    void addFailureCriteria(NEFailureCriteria fail) override;
    void removeFailureCriteria(NEFailureCriteria fail) override;
    void assignFailureCriteria(NENode *node, NEFailureCriteria fail) override;

    QList<NEVentCriteria> getVentCriteria(uint zoneId) override;
    void addVentCriteria(NEVentCriteria criterion) override;
    void removeVentCriteria(NEVentCriteria criterion) override;
    void assignVentCriteria(NENode *node, NEVentCriteria criterion, QString criterionKind) override;

    QList<NECableProperty> getCableProperty() override;
    void addCableProperty(NECableProperty cable) override;
    void removeCableProperty(NECableProperty cable) override;
    void assignCableProperty(NENode* node, NECableProperty cable) override;

    std::vector<QList<QString>> getCableOptions();

    void setDynamicCombo(NENode* node, NEDynamicCombo dynamicCombo, QString propName);

    void changeColorOfZoneItem(double elapsedTime) override;

    FailedItems &currentFailedItems();

    //
    //
    // Compute Related
    void updateComputeScenarios( FRANXProperties fplist );

    //
    // Simulation related
    //
    void simulateScenario(QString scnName, QString desc, QString simEngineNodeName) override;
    void finishedSimulateScenario();

    ///
    /// \brief DBName
    /// \return
    /// - Get Name of database used
    QString DBName();

    void saveAs( NENode *node, QString name );

    void updateLoadUI(bool loadUI);
    void updateSaveUI(bool saveUI);

public:
    int createPlantItem(NEZoneItem *newZoneItem, NENode *parent);

    //
    // Get and set properties of the nodes in the scene based out of JSON String stored in the
    // Database.
    //
    QJsonArray getJSONFromProperties( NENode* node );

    void       setPropertiesFromJSON( NENode *node, QString model3DInfoStr );

    QString    filterProperties(NENode *node, QString model3DInfoStr, QString filterStr);

private:
    NE_DECL_PRIVATE(FEngine)

};

// The factory class
class FEngineFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FEngineFactory(NEManager* m = 0);
    ~FEngineFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames() override;
};
