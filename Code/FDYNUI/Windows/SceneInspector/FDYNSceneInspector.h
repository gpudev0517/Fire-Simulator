#pragma once

#include <QQuickWidget>
#include "Utilities/FDYNTreeWidget.h"

class FDYNSceneInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;

class FDYNTreeModel;
class FDYNTreeItem;

class FDYNSceneInspector : public FDYNTreeWidget
{
    Q_OBJECT
public:
    FDYNSceneInspector(QWidget* parent = 0);
    ~FDYNSceneInspector();

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

signals:
    void setActiveObject(QTreeWidgetItem*, int);

public slots:
    //! Updates the whole inspector tree
    void update();
    //! Creates the context menu
    void createContextMenu();
    //! Remove object
    void removeSelectedItem();
    //! Selects the given item
    void selectItems();

    void renameSelected();

    void groupNodes();
    void ungroupNodes();

    void parentNodes();
    void unparentNodes();

    //void clear();
    void expandAll();

private slots:

    void nodeRenamed(FDYNTreeItem * item, int column);
    void itemExpansion(FDYNTreeItem *item);
    void itemCollapse(FDYNTreeItem* item);
    void itemActivation(FDYNTreeItem * item, int column);


private:

    void restoreExpandedState();
    void clearState();
    //void mousePressEvent(QMouseEvent *event);
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, FDYNSceneInspectorItem* parent);

    FDYNSceneInspectorItem* SceneItem;
    QMap<QString, FDYNSceneInspectorItem*> SceneInpectorItems;

    FDYNSceneInspectorItem* createOrganizerItem(QString name, FDYNSceneInspectorItem *parentItem);
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

    QAction* groupAction;
    QAction* ungroupAction;
    QAction* subItemAction;

    QAction* parentAction;
    QAction* unparentAction;

    QMap<QString, bool> m_ExpNodes;
    QModelIndex topLeft;
    QItemSelection m_LastSelectedItem;
};
