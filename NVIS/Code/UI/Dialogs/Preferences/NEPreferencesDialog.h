#pragma once

class NEPluginInfoDialog;
struct NEDynaFactoryInfo;
class NEPrefTreeWidgetItem;
class NEManager;

namespace Ui {
class NEPreferencesDialog;
}

class NEPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NEPreferencesDialog(QWidget *parent = 0);
    ~NEPreferencesDialog();

public slots:
    void saveUserPreferences();
    void loadUserPreferences();
    void changePrefPage(const QModelIndex& mi);
    void restorePageDefaults();
    void showPluginInfo();
    void reloadPlugin(NEPrefTreeWidgetItem* item = 0);
    void unloadPlugin(NEPrefTreeWidgetItem* item = 0);
    void reactToPluginCheck(NEPrefTreeWidgetItem* item, bool b);
    void deleteNodes(NEDynaFactoryInfo* f);
    void recreateNodes(NEDynaFactoryInfo* f);

private slots:
    void editShelfItem();
    void addShelfItem();
    void removeShelfItem();

private:
    void saveObjectProperties(const QObject& o);
    void loadObjectProperties(QObject& o, QString group);
    void populatePluginsPage();
    void insertManager(NEManager* m, QTreeWidgetItem** item2, QString name);
    void loadManagerPlugins(NEManager* m, QTreeWidgetItem* pi);
    void savePluginStates();
    void savePluginStates(QTreeWidgetItem* item);
    void loadPluginStates();
    void loadPluginStates(QTreeWidgetItem* item);
    void saveShelfData();
    void loadShelfData();

    void populateShelfPage();

protected:
     void showEvent(QShowEvent* e);



    Ui::NEPreferencesDialog *ui;
    QSettings* m_pSettings;
    NEPluginInfoDialog* m_pPluginInfoDialog;

    QTreeWidgetItem* m_pGeoMgrGroup = 0;
    QTreeWidgetItem* m_pDFMgrGroup = 0;
    QTreeWidgetItem* m_pPPMgrGroup = 0;
    QTreeWidgetItem* m_pSPHMgrGroup = 0;
    QTreeWidgetItem* m_pGFMgrGroup = 0;
    QTreeWidgetItem* m_pMSSMgrGroup = 0;
    QTreeWidgetItem* m_pCOLMgrGroup = 0;
    QTreeWidgetItem* m_pDEFMgrGroup = 0;
    QTreeWidgetItem* m_pRGDMgrGroup = 0;
    QTreeWidgetItem* m_pSIMMgrGroup = 0;
    QTreeWidgetItem* m_pRENMgrGroup = 0;



};




