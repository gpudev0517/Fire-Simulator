#pragma once

class NESystemInspectorItem;

class NESystemInspector : public QTreeWidget
{
    Q_OBJECT
public:
    explicit NESystemInspector(QWidget *parent = 0);
    ~NESystemInspector();

    //! Sets the root node for the scene inspector
    //void setRoot(QObject* root);

    void addRoot(QObject* root);
    void removeRoot(QObject* root);
    //! Checks if this object is selected
    bool isSelectedObect(QObject* object);
    //! Sets the parent item of the current item as current.
    void setParentCurrentItem();

    void setNewCurrentItem(QObject* object);
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


private:
    void mousePressEvent(QMouseEvent *event);
    //! Builds the subtree for the given QObject
    void traverseTree(const QObject* obj, NESystemInspectorItem* parent);

    QFont* m_pBoldFont;
    QBrush* m_pBrushLight;
    QBrush* m_pBrushBlack;

    //! Pointer to the root object for building the tree
    //QObject* m_pRoot;
    QList<QObject*> m_Roots;
};

