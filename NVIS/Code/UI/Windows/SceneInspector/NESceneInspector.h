#pragma once


class NESceneInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;


class NESceneInspector : public QTreeWidget
{
    Q_OBJECT
public:
    NESceneInspector(QWidget* parent);
    ~NESceneInspector();

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

    void groupNodes();
    void ungroupNodes();

    void parentNodes();
    void unparentNodes();

private slots:

    void nodeRenamed(QTreeWidgetItem * item, int column);
    void itemExpansion(QTreeWidgetItem *item);
    void itemCollapse(QTreeWidgetItem* item);
    void itemActivation(QTreeWidgetItem * item, int column);


private:

    void restoreExpandedState();
    void clearState();
    void mousePressEvent(QMouseEvent *event);
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, NESceneInspectorItem* parent);

    NESceneInspectorItem* SceneItem;
    QMap<QString, NESceneInspectorItem*> SceneInpectorItems;

    NESceneInspectorItem* createOrganizerItem(QString name, NESceneInspectorItem *parentItem);
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

    QAction* parentAction;
    QAction* unparentAction;

    QMap<QString, bool> m_ExpNodes;
    QModelIndex topLeft;
    QItemSelection m_LastSelectedItem;
};


