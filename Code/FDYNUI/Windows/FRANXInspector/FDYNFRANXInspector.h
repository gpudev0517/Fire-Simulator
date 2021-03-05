#pragma once

#include <QQuickWidget>
#include "Utilities/FDYNTreeWidget.h"


class FDYNFRANXInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;
class NEZone;

class FDYNFRANXInspector : public FDYNTreeWidget
{
    Q_OBJECT
public:
    FDYNFRANXInspector(QWidget* parent);
    ~FDYNFRANXInspector();

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

    void positionToSelected();

signals:
    void setActiveObject(QTreeWidgetItem*, int);

public slots:
    //! Updates the whole inspector tree
    void update();

    void updateChildren(const QObject *child);

    //! Creates the context menu
    void createContextMenu();

    //! Selects the given item
    void selectItems();

    void renameSelected();

    void slotDropDownItemChanged( QString newZone );
    void slotOtherItemChanged(QString newZone );

private slots:

    void itemExpansion(FDYNTreeItem *item);
    void itemCollapse(FDYNTreeItem* item);
    void itemActivation(FDYNTreeItem* item, int column);

private:

    void restoreExpandedState();
    void clearState();
    //void mousePressEvent(QMouseEvent *event);
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, const NEZone* zone, FDYNFRANXInspectorItem* parent);

    FDYNFRANXInspectorItem* SceneItem;
    QMap<QString, FDYNFRANXInspectorItem*> SceneInpectorItems;

    FDYNFRANXInspectorItem* createOrganizerItem(QString name, FDYNFRANXInspectorItem *parentItem);
    //! Pointers to different scene inspector items for grouping
    //!

    //! Pointer to the root object for building the tree
    QObject* m_pRoot;
    QList<QString> m_allScenarioItems;

public:

    QIcon* m_IconActive;
    QIcon* m_IconCache;
    QIcon* m_IconInactive;

    QPixmap* m_PixmapActive;
    QPixmap* m_PixmapCache;
    QPixmap* m_PixmapInactive;
    //QPixelFormat* m_PixmapDirLight;

    QRegExpValidator* validator;
    EditDelegate* editDelagate;

    QMap<QString, bool> m_ExpNodes;
    QModelIndex topLeft;
    QItemSelection m_LastSelectedItem;
};


