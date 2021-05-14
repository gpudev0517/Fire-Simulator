#pragma once


//class QTreeWidgetItem;

class NENode;
class FDYNScenarioInspector;
class FDYNTreeItem;

NE_FWD_DECL_1(class, NE_PRIVATE(FDYNScenarioInspectorWindow))


class FDYNScenarioInspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDYNScenarioInspectorWindow(QWidget *parent = 0);
    ~FDYNScenarioInspectorWindow();
public slots:
    void removeSelectedItem();
    //! Sets the root node for the scene inspector
public:
    void setRoot(QObject* root);
    void update();
    void setNewCurrentItem(QObject* obj);
    void setCurrentItems(QList<NENode*> objList);
    bool isSelectedObject(NENode* obj);
    void setParentCurrentItem();
    void writeGUIState(QXmlStreamWriter *writer);
    void readGUIState(QXmlStreamReader *reader);

    FDYNScenarioInspector *scenarioInspector();

protected slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void SetChildsVisibility(FDYNTreeItem * item, QString exp);

private:
    //Ui::FDYNZoneInspectorWindow *ui;
    NE_DECL_PRIVATE(FDYNScenarioInspectorWindow);
};
