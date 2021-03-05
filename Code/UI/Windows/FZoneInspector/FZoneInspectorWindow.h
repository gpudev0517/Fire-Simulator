#pragma once


class QTreeWidgetItem;

class NENode;

namespace Ui {
class FZoneInspectorWindow;
}

class FZoneInspectorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FZoneInspectorWindow(QWidget *parent = 0);
    ~FZoneInspectorWindow();
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

protected slots:
    void on_lineEdit_textChanged(const QString &arg1);
    void SetChildsVisibility(QTreeWidgetItem * item, QString exp);

private:
    Ui::FZoneInspectorWindow *ui;
};
