#pragma once


//class QTreeWidgetItem;

class NENode;
class FDYNFRANXInspector;
class FDYNTreeItem;

NE_FWD_DECL_1(class, NE_PRIVATE(FDYNFRANXInspectorWindow))


class FDYNFRANXInspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDYNFRANXInspectorWindow(QWidget *parent = 0);
    ~FDYNFRANXInspectorWindow();

public:
    void setRoot(QObject* root);
    void update();
    void setNewCurrentItem(QObject* obj);
    void setCurrentItems(QList<NENode*> objList);
    bool isSelectedObject(NENode* obj);
    void setParentCurrentItem();
    void writeGUIState(QXmlStreamWriter *writer);
    void readGUIState(QXmlStreamReader *reader);

    FDYNFRANXInspector *franxInspector();

protected slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void SetChildsVisibility(FDYNTreeItem * item, QString exp);

private:
    //Ui::FDYNZoneInspectorWindow *ui;
    NE_DECL_PRIVATE(FDYNFRANXInspectorWindow)
};
