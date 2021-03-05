#pragma once
#include "Utility/FilePath.h"
#include "Mesh/NEScenario.h"
#include "Base/NENodeFactory.h"
#include "Mesh/NEZone.h"

NE_FWD_DECL_4(class, FZone, FZoneItem, FilePath, NE_PRIVATE(FScenario))

class FDYNCEXP_PLUGINS FScenario : public NEScenario
{
    Q_OBJECT
    NE_CLASS("Fire Scenario")

    NE_PROP_RW(FilePath, ScenarioFile, "Scenario File", "File Describing the Scenario", true,)
    NE_PROP_RW(uint, ConnectorID, "Scenario Connector ID", "Scenario Connection ID", true,)
    NE_PROP_RW(QString, Issues, "Scenario Issues", "Zone Issues not convered prior", true,)

public:
    FScenario( NENodeFactory* factory = 0);
    virtual ~FScenario();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void resetProperties() override;

    void initializeGUIActions() override;
    QString scenarioName();
    uint setScenarioName(const QString scenarioName );
    void loadScenarioFile(const QString &filePath);
    void clearScenarioFile();
    void reloadScenarioFile();

    QList<QString > &zones() override;

    QString model3DInfo();
    void setModel3DInfo( QString model3dinfo );

    virtual QString nodeDescription() override;

private:
    NE_DECL_PRIVATE(FScenario)

};

// The factory class
class FScenarioFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    FScenarioFactory(NEManager* m = 0);
    ~FScenarioFactory();

    NE_DERIVED(NENodeFactory, Public)
    QStringList nodeAltNames();
};
