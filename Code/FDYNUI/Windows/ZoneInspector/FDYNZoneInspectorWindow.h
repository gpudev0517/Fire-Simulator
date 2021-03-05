#pragma once

//class QTreeWidgetItem;

class NENode;
class FDYNZoneInspector;
class FDYNTreeItem;

NE_FWD_DECL_1(class, NE_PRIVATE(FDYNZoneInspectorWindow))

class FDYNZoneInspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDYNZoneInspectorWindow(QWidget *parent = 0);
    ~FDYNZoneInspectorWindow();

public slots:
    void removeSelectedItem();

public:
    //! Sets the root node for the scene inspector
    void setRoot(QObject* root);
    QObject *root();
    void update();
    void setNewCurrentItem(QObject* obj);
    void setCurrentItems(QList<NENode*> objList);
    bool isSelectedObject(NENode* obj);
    void setParentCurrentItem();
    void writeGUIState(QXmlStreamWriter *writer);
    void readGUIState(QXmlStreamReader *reader);

    FDYNZoneInspector *zoneInspector();

protected slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void SetChildsVisibility(FDYNTreeItem * item, QString exp);

private:
    //Ui::FDYNZoneInspectorWindow *ui;
    NE_DECL_PRIVATE(FDYNZoneInspectorWindow);
};
