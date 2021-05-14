#pragma once

#include <QQuickItem>
#include <QQuickWidget>
#include "Utilities/FDYNTreeModel.h"
#include <QAbstractItemDelegate>

class FDYNDockWidget;

class FDYNTreeWidget : public QQuickWidget {
    Q_OBJECT
    friend class FDYNDockWidget;

public:
    explicit FDYNTreeWidget(QWidget *parent = 0);
    virtual ~FDYNTreeWidget();

    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    uint topLevelItemCount();
    FDYNTreeItem* takeTopLevelItem( uint );
    void addTopLevelItem( FDYNTreeItem *);
    FDYNTreeModel* model(){return m_FDYNTreeModel;}

    /// TreeWidget Integration - end

    FDYNTreeItem *m_RootItem = nullptr;

    FDYNTreeModel* m_FDYNTreeModel;

    uint selectedRow;
    uint selectedColumn;
    FDYNTreeItem* selectedNode;

    FDYNTreeItem* m_currentItem;
    QList<FDYNTreeItem*> m_selectedItems;
    QList<FDYNTreeItem*> m_allItems;

    mutable QList<FDYNTreeItem*> mResultList;


    mutable QStringList m_dropDownList;
    QObject *m_dropDownObject;

    mutable QStringList m_otherComboList;
    QObject *m_otherComboObject;

    QObject* treeview = NULL;
    QObject* txtSearch = NULL;
    QObject* currentTextInput = NULL;

    QString m_searchText;
    QObjectList m_searchMatchedObjects;

    void addDropDownItem(QString item);
    void modifyDropDownItem(QString orgItem, QString newItem);
    void deleteDropDownItem(QString item);
    void clearDropDownItems();

    void addOtherListItem(QString item);
    void modifyOtherListItem(QString orgItem, QString newItem);
    void deleteOtherListItem(QString item);
    void clearOtherListItems();

    virtual void update();

    bool containStringinChildren(QObject *obj, QString searchText)
    {
        bool ret = false;
        QString tmpString(obj->objectName());
        if(tmpString.contains(searchText, Qt::CaseInsensitive)){

            ret = true;
        }
        else{
            int childcount = obj->children().size();
            if(childcount > 0){
                foreach (QObject *child, obj->children()) {
                    if(containStringinChildren(child, searchText)){
                        ret = true;
                        break;
                    }
                }
            }
        }
        return ret;
    }


public slots:
    void slotItemSelected(QVariant);
    void slotItemExpanded(QVariant);
    void slotItemCollapsed(QVariant);
    void slotItemClicked(QVariant);
    void slotItemSelectionChanged(QVariant selected, QVariant deselected);
    void slotSearchTextChanged(QVariant);
    void slotSearchEditingFinished(QVariant);
    void slotCurrentTextInput(QVariant varTextInput);
    void slotItemRenamed(QVariant idx, QString str);

    void expandItem(FDYNTreeItem* item);
    void collapseItem(FDYNTreeItem* item);
    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    FDYNTreeItem* currentItem();
    QList<FDYNTreeItem *>  selectedItems() const;
    QList<FDYNTreeItem *> &items();
    void subItems(FDYNTreeItem *item);
    void setCurrentItem( FDYNTreeItem* item);
    void setCurrentItem(FDYNTreeItem*, int);
    QModelIndex	indexFromItem(FDYNTreeItem *item, int column = 0) const;
    FDYNTreeItem *	itemFromIndex(const QModelIndex & index) const;
    QList<FDYNTreeItem *> findItems(const QString &text, Qt::MatchFlags flags, int column = 0) const;

    QModelIndex currentIndex() const;
    void setCurrentIndex(const QModelIndex & index);

    void edit(const QModelIndex & index);
    FDYNTreeItem * itemAt(const QPoint & p) const;
    void editItem(FDYNTreeItem* item, int column = 0);

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

    FDYNTreeItem* invisibleRootItem() const;

    void clear();


    /// TreeWidget Integration - end
    ///
public:

    Q_INVOKABLE void thisFunctionCalled(QModelIndex index);

private:
    void findSubItems(FDYNTreeItem *node, const QString &text, Qt::MatchFlags flags, int colum = 0) const;
    void insertQMLRows(FDYNTreeItem* item);
    void insertQMLTopRows();
signals:
    void itemSelectionChanged();
    void itemExpanded(FDYNTreeItem *item);
    void itemCollapsed(FDYNTreeItem *item);
    void itemClicked(FDYNTreeItem *item, int column);
    void dropDownItemAdded(QString);
    void otherItemAdded(QString);
    void itemChanged(FDYNTreeItem *item, int column);


public slots:

    virtual void slotDropDownItemAdded(QString );
    virtual void slotDropDownItemChanged(QString);

    virtual void slotOtherItemAdded(QString );
    virtual void slotOtherItemChanged(QString);

    virtual void slotFilterWithSearchText(QString strSearch);

    /*////////////////////////////////QTreeWidget Functions////////////////////////////////////////*/

};


