#pragma once

#include <QQuickWidget>
#include "Utilities/FDYNTreeWidget.h"

class FDYNScenarioInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;
class NEScenario;

class FDYNScenarioInspector : public FDYNTreeWidget
{
    Q_OBJECT
public:
    FDYNScenarioInspector(QWidget* parent);
    ~FDYNScenarioInspector();

    //! Sets the root node for the scene inspector
    void setRoot(QObject* root);
    //! Checks if this object is selected
    bool isSelectedObect(QObject* object);
    //! Sets the parent item of the current item as current.
    void setParentCurrentItem();

    void setNewCurrentItem(QObject* object);
    void setCurrentItems(QList<NENode*> object);
    void saveGUIStates(QXmlStreamWriter *writer);
    void readGUIStates(QXmlStreamReader* reader);

    void updateOtherScenarios(QList<QString> &otherscns );
    void updateAllScenarios(QList<QString> allScenarios );

signals:
    void setActiveObject(QTreeWidgetItem*, int);

public slots:
    //! Updates the whole inspector tree
    void update();

    void updateChildren(const QObject *rootobj);

    //! Creates the context menu
    void createContextMenu();
    //! Remove object
    void removeSelectedItem();
    //! Selects the given item
    void selectItems();

    //! Add item to inspector tree
    void addItemToTree(const QObject* itemObj, const QObject* parentObj);

    void renameSelected();

    void slotDropDownItemChanged( QString newScn );
    void slotOtherItemChanged(QString newScn );

    void runDrop(matrix44f view, matrix44f proj, double x, double y, double w, double h, void *tItem);

private slots:

    void nodeRenamed(FDYNTreeItem * item, int column);
    void itemExpansion(FDYNTreeItem *item);
    void itemCollapse(FDYNTreeItem* item);
    void itemActivation(FDYNTreeItem* item, int column);


private:

    void restoreExpandedState();
    void clearState();
    //void mousePressEvent(QMouseEvent *event);
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, const NEScenario* scenario, FDYNScenarioInspectorItem* objItem);

    FDYNScenarioInspectorItem* SceneItem;
    QMap<QString, FDYNScenarioInspectorItem*> SceneInpectorItems;

    FDYNScenarioInspectorItem* createRootOrganizerItems(NEScenario *Scn, FDYNScenarioInspectorItem *rootItem);
    FDYNScenarioInspectorItem* createOrganizerItem(QString name, FDYNScenarioInspectorItem *objItem);
    //! Pointers to different scene inspector items for grouping
    //!

    //! Pointer to the root object for building the tree
    QObject* m_pRoot;


public:

    QIcon* m_IconActive;
    QIcon* m_IconCache;
    QIcon* m_IconInactive;

    QPixmap* m_PixmapActive;
    QPixmap* m_PixmapCache;
    QPixmap* m_PixmapInactive;
    QPixmap* m_PixmapCamera;
    //QPixelFormat* m_PixmapDirLight;

    QRegExpValidator* validator;
    EditDelegate* editDelagate;

    QAction* simulateAction;

    QAction* deleteScnAction;
    QAction* saveAsScnAction;
    QAction* removeItemAction;
    QAction* addScnAction;

    QMap<QString, bool> m_ExpNodes;
    QModelIndex topLeft;
    QItemSelection m_LastSelectedItem;
};
