#pragma once


class NETreeInspectorItem;
class NENode;
class EditDelegate;
class QRegExpValidator;


class NETreeInspector : public QTreeWidget
{
    Q_OBJECT
public:
    NETreeInspector(QWidget* parent);
    ~NETreeInspector();

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
    void traverseTree(const QObject* obj, NETreeInspectorItem* parent);


    NETreeInspectorItem* createOrganizerItem(const char *name);
    //! Pointers to different scene inspector items for grouping
    NETreeInspectorItem* Cameras;
    NETreeInspectorItem* Lights;
    NETreeInspectorItem* Materials;
    NETreeInspectorItem* Textures;
    NETreeInspectorItem* FluidSolvers;
    NETreeInspectorItem* Fluids;
    NETreeInspectorItem* ParticleSources;
    NETreeInspectorItem* KillerFields;
    NETreeInspectorItem* DualBoundaries;
    NETreeInspectorItem* ForceFields;
    NETreeInspectorItem* DataFields;
    NETreeInspectorItem* Meshes;
    NETreeInspectorItem* Rigids;
    NETreeInspectorItem* Groups;
    NETreeInspectorItem* Deformables;
    NETreeInspectorItem* MSSs;
    NETreeInspectorItem* VolumeOps;
    NETreeInspectorItem* MeasurementFields;
    NETreeInspectorItem* OSDs;
    NETreeInspectorItem* Meshing;
    NETreeInspectorItem* PostProcessors;
    NETreeInspectorItem* RagDolls;
    NETreeInspectorItem* Bridges;
    NETreeInspectorItem* CollisionDetection;

    NETreeInspectorItem* SceneItem;

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


