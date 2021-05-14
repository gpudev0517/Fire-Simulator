#pragma once

#include <QQuickWidget>
#include "Utilities/FDYNTreeWidget.h"

class FDYNZoneInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;
class NEZone;
class NEScenario;

class FDYNZoneInspector : public FDYNTreeWidget
{
    Q_OBJECT
public:
    FDYNZoneInspector(QWidget* parent);
    ~FDYNZoneInspector();

    //! Sets the root node for the scene inspector
    void setRoot(QObject* root);
    QObject *root(){ return m_pRoot;}
    //! Checks if this object is selected
    bool isSelectedObect(QObject* object);

    //! Sets the parent item of the current item as current.
    void setParentCurrentItem();

    void setNewCurrentItem(QObject* object);
    void setCurrentItems(QList<NENode*> object);
    void saveGUIStates(QXmlStreamWriter *writer);
    void readGUIStates(QXmlStreamReader* reader);

    void updateOtherZones(QList<QString> &otherzones);

    void updateAllZones(QList<QString> allZones );

    void positionToSelected();

signals:
    void setActiveObject(QTreeWidgetItem*, int);

public slots:
    //! Updates the whole inspector tree
    void update();

    void updateChildren(const QObject *rootobj);

    //! Creates the context menu
    void createContextMenu();
    //! Delete object
    void deleteSelectedItem();
    //! Remove object
    void removeSelectedItem();
    //!
    void removeSelected3D();

    //! Add item to scenario
    void addItemToScenario();
    //! Remove item from scenario
    void removeItemFromScenario();

    //! Add item to inspector tree
    void addItemToTree(const QObject* objItem);
    //! Display item as part of active scenario
    void displayItemAsActive(const QObject* objItem);

    //! Selects the given item
    void selectItems();

    void renameSelected();

    void slotDropDownItemChanged( QString newZone );
    void slotOtherItemChanged(QString newZone );

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
    //!
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, const NEZone* zone, const NEScenario* scenario, FDYNZoneInspectorItem* objItem);

    FDYNZoneInspectorItem* SceneItem;
    QMap<QString, FDYNZoneInspectorItem*> SceneInpectorItems;

    FDYNZoneInspectorItem* createRootOrganizerItems(NEZone* rootzone , FDYNZoneInspectorItem *rootItem);

    FDYNZoneInspectorItem* createOrganizerItem(QString categoryName, FDYNZoneInspectorItem *objItem);
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

    QAction* loadDatabaseAction;
    QAction* importFRANXAction;

    QAction* duplicateAction;
    QAction* copyPropertiesAction;
    QAction* pastePropertiesAction;
    QAction* pasteDataPropertiesAction;
    QAction* paste3DPropertiesAction;

    QAction* subItemAction;
    QAction* addComponent;
    QAction* deleteItemAction;
    QAction* removeItemAction;
    QAction* addToScnAction;
    QAction* removeFromScnAction;
    QAction* removeFromAllScnAction;

    QAction* addRaceway;
    QAction* addCable;

    QAction* simulateNewScns;
    QAction* convertToSource;
    QAction* convertToItem;
    QAction* convertToPhyOnly;

    QAction *addCompAction;

    QAction* addFireDescAction;

    QAction* addVentAction;
    QAction* addMechVentAction;
    QAction* turnIntoVentAction;
    QAction* turnIntoMechVentAction;

    QAction* addFireDetectionAction;

    QAction* actionNewZone;
    QAction* actionNewScn;

    QMap<QString, bool> m_ExpNodes;
    QModelIndex topLeft;
    QItemSelection m_LastSelectedItem;
};
