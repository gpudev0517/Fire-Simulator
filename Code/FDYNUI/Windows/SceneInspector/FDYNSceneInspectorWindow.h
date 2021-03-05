#pragma once

class FDYNTreeItem;
class NENode;
class FDYNSceneInspector;

NE_FWD_DECL_1(class, NE_PRIVATE(FDYNSceneInspectorWindow))


class FDYNSceneInspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDYNSceneInspectorWindow(QWidget *parent = 0);
    ~FDYNSceneInspectorWindow();
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

    FDYNSceneInspector *sceneInspector();

protected slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void SetChildsVisibility(FDYNTreeItem * item, QString exp);

private:
    //Ui::FDYNSceneInspectorWindow *ui;

    NE_DECL_PRIVATE(FDYNSceneInspectorWindow);
};
