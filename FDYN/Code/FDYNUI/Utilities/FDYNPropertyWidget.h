#pragma once

#include <QQuickItem>
#include <QQuickWidget>
#include "Utilities/FDYNPropertyModel.h"
#include <QAbstractItemDelegate>

class FDYNDockWidget;

class FDYNPropertyWidget : public QQuickWidget {
    Q_OBJECT
    friend class FDYNDockWidget;

public:
    explicit FDYNPropertyWidget(QWidget *parent = 0);
    virtual ~FDYNPropertyWidget();

    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    uint topLevelItemCount();
    FDYNPropertyItem* takeTopLevelItem( uint );
    void addTopLevelItem( FDYNPropertyItem *);
    FDYNPropertyModel* model(){return m_FDYNPropertyModel;}

    /// Property Widget Integration - end

    FDYNPropertyItem *m_RootItem = nullptr;

    FDYNPropertyModel* m_FDYNPropertyModel;

    uint selectedRow;
    uint selectedColumn;
    FDYNPropertyItem* selectedNode;

    FDYNPropertyItem* m_currentItem;
    QList<FDYNPropertyItem*> m_selectedItems;
    QList<FDYNPropertyItem*> m_allItems;

    mutable QList<FDYNPropertyItem*> mResultList;

    QObject* propertyview = NULL;

    virtual void update();


public slots:
    void slotItemSelected(QVariant);
    void slotItemExpanded(QVariant);
    void slotItemCollapsed(QVariant);
    void slotItemClicked(QVariant);
    void slotItemSelectionChanged(QVariant selected, QVariant deselected);

    void expandItem(FDYNPropertyItem* item);
    void collapseItem(FDYNPropertyItem* item);
    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    FDYNPropertyItem* currentItem();
    QList<FDYNPropertyItem *>  selectedItems() const;
    QList<FDYNPropertyItem *> &items();
    void subItems(FDYNPropertyItem *item);
    void setCurrentItem( FDYNPropertyItem* item);
    void setCurrentItem(FDYNPropertyItem*, int);
    QModelIndex	indexFromItem(FDYNPropertyItem *item, int column = 0) const;
    FDYNPropertyItem *	itemFromIndex(const QModelIndex & index) const;
    QList<FDYNPropertyItem *> findItems(const QString &text, Qt::MatchFlags flags, int column = 0) const;

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex & index);

    void edit(const QModelIndex & index);
    FDYNPropertyItem * itemAt(const QPoint & p) const;
    void editItem(FDYNPropertyItem* item, int column = 0);

    bool	rootIsDecorated() const;
    void	setRootIsDecorated(bool show);

    int	indentation() const;
    void setIndentation(int i);

    QSize iconSize() const;
    void setIconSize(const QSize & size);

    int	columnCount() const;
    void setColumnCount(int columns);

    bool isAnimated() const;
    void setAnimated(bool enable);

    void setHeaderLabels(const QStringList & labels);

    bool alternatingRowColors() const;
    void setAlternatingRowColors(bool enable);

    int editTriggers() const;
    void setEditTriggers(int triggers);

    void setItemDelegate(QAbstractItemDelegate * delegate);

    FDYNPropertyItem* invisibleRootItem() const;

    void clear();


    /// TreeWidget Integration - end
    ///
public:

    Q_INVOKABLE void thisFunctionCalled(QModelIndex index);

public:
    void findSubItems(FDYNPropertyItem *node, const QString &text, Qt::MatchFlags flags, int colum = 0) const;
    void insertQMLRows(FDYNPropertyItem* item);
    void insertQMLTopRows();
signals:
    void itemSelectionChanged();
    void itemExpanded(FDYNPropertyItem *item);
    void itemCollapsed(FDYNPropertyItem *item);
    void itemClicked(FDYNPropertyItem *item, int column);
    void collapsed(const QModelIndex &index);
    void expanded(const QModelIndex &index);

    //to to implement.
    void currentItemChanged(FDYNPropertyItem*, FDYNPropertyItem*);

    /*////////////////////////////////QTreeWidget Functions////////////////////////////////////////*/

};


