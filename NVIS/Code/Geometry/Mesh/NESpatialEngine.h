#pragma once
#include "Spatial/NESpatialGroup.h"
#include "NEZone.h"
#include "NEZoneItem.h"
#include "NEScenario.h"

#include "Utility/NEFireMaterial.h"
#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFailureCriteria.h"
#include "Utility/NEVentCriteria.h"
#include "Utility/NECableProperty.h"

class NECEXP_GEOMETRY NESpatialEngine : public NESpatialGroup
{
public:
    Q_OBJECT
    NE_CLASS("Spatial Engine")

public:
    NESpatialEngine( NENodeFactory* factory );
    virtual ~NESpatialEngine();

public slots:

    virtual void nodeRenamed( NENode* node, QString name );

    virtual void setActiveZone(QString zoneName);
    virtual void setActiveScenario(QString scenarioName);

    QList< QString > zones();
    QList< QString > scenarios();

    virtual int zoneCreated( NEZone *newZone );
    virtual int zoneDeleted( NEZone *zone );

    virtual int scenarioCreated( NEScenario *newScn );
    virtual int scenarioDeleted( NEScenario *scn );

    virtual int itemCreated( NEZoneItem *newZoneItem, NENode* parent = 0);
    virtual int itemDeleted( NEZoneItem *newZoneItem, bool force = true );

    virtual void duplicateObject( NEZoneItem *item );
    virtual void copyProperties( NEZoneItem *item );
    virtual void pasteProperties( QList<NEZoneItem *> items, QString filter );

    virtual int addCompartment(NENode *node, NENode *parent );

    virtual void addItemToScenario( NEZoneItem* item, NENode *parent ) ;
    virtual void removeItemFromScenario(NEZoneItem *zoneItem, NENode* parent ) ;
    virtual void removeItemFromScenarios(NEZoneItem *zoneItem, QList<NENode*> parents ) ;

    virtual QString activeZoneName();
    virtual QString activeScenarioName();

    virtual void clear();
    virtual void newData( QString filename = QString( "" ) );
    virtual void load( QString filename );
    virtual void save( QString filename );
    virtual void quit();
    virtual void saveZone( QString zonename, QString filename );
    virtual void saveScenario( QString scenarioname, QString filename );
    virtual uint compute();
    virtual void computeOptions();

    virtual QString nodeDescription();

    //
    // All material related calls
    //
    virtual QList<NESurfaceMaterial> getSurfaceMaterials();
    virtual void addSurfaceMaterial(NESurfaceMaterial sm);
    virtual void removeSurfaceMaterial(NESurfaceMaterial sm);
    virtual void assignSurfaceMaterial( NENode *zoneitem, NESurfaceMaterial sm, QString matKind ) ;

    virtual QList<NEFireMaterial> getFireMaterials();
    virtual void addFireMaterial(NEFireMaterial fm);
    virtual void removeFireMaterial(NEFireMaterial fm);
    virtual void assignFireMaterial( NENode *zoneitem, NEFireMaterial sm );

    virtual QList<NEFailureCriteria> getFailureCriteria() ;
    virtual void addFailureCriteria(NEFailureCriteria fail) ;
    virtual void removeFailureCriteria(NEFailureCriteria fail) ;
    virtual void assignFailureCriteria(NENode* node, NEFailureCriteria fail) ;

    virtual QList<NEVentCriteria> getVentCriteria(uint zoneId) ;
    virtual void addVentCriteria(NEVentCriteria criterion) ;
    virtual void removeVentCriteria(NEVentCriteria criterion) ;
    virtual void assignVentCriteria(NENode* node, NEVentCriteria criterion, QString criterionKind) ;

    virtual QList<NECableProperty> getCableProperty() ;
    virtual void addCableProperty(NECableProperty cable) ;
    virtual void removeCableProperty(NECableProperty cable) ;
    virtual void assignCableProperty(NENode* node, NECableProperty cable) ;

    virtual void changeColorOfZoneItem(double elapsedTime);

    virtual void simulateScenario(QString scnName, QString desc, QString simNodeName) ;
    virtual void updateWorkingFolder( QString folderName );

signals:
    void sendProjectRecovery( bool recover );

    void newZones();
    void newScenarios();
    void activeZone(QObject*& fengine, QObject* &activeZone);
    void activeScenario(QObject*& fengine, QObject* &activeScn);
    void allZones(QList<QString> &allZones);
    void allScenarios(QList<QString> &allScenarios);
    void relatedZones( QList<QString> &relatedZones );
    void relatedScenarios( QList<QString> &relatedScenarios );
    void dataChanged( QString name );
    void sendShowWarningMessage(QString messege);

    void updateSurfaceMaterials( QList<NESurfaceMaterial> smats );
    void updateFireMaterials( QList<NEFireMaterial> firemats );

    void allFailedItems( QList<int> failedItemID, QList<double> failedTime, QList<int> scenarioID, QList<QString> failOutput );
    void repaintGLWindows();

    void sendUpdateWorkingFolder(QString tempFolder);

private:

};



