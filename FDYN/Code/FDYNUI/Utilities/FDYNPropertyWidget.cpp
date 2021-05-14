#pragma once
#include "FDYNPropertyWidget.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QVariant>
#include <QtQuickControls2>

#include "Managers/FDYNGUIManager.h"

FDYNPropertyWidget::FDYNPropertyWidget(QWidget *parent):
    QQuickWidget(parent)
{

    selectedRow = 0;
    selectedColumn = 0;
    m_currentItem = 0;

    m_RootItem = new FDYNPropertyItem(this);

    m_FDYNPropertyModel = new FDYNPropertyModel(m_RootItem);

    rootContext()->setContextProperty("propertyModel", m_FDYNPropertyModel);

    setSource(QUrl("qrc:/PropertyEditor.qml"));
    setResizeMode(QQuickWidget::SizeRootObjectToView);

    propertyview = rootObject()->findChild<QObject*>("propertyview");

    QObject::connect(propertyview, SIGNAL(sigItemExpanded(QVariant)), this, SLOT(slotItemExpanded(QVariant)));
    QObject::connect(propertyview, SIGNAL(sigItemCollapsed(QVariant)), this, SLOT(slotItemCollapsed(QVariant)));
    QObject::connect(propertyview, SIGNAL(sigItemClicked(QVariant)), this, SLOT(slotItemClicked(QVariant)));
    QObject::connect(propertyview, SIGNAL(sigItemSelectionChanged(QVariant, QVariant)), this, SLOT(slotItemSelectionChanged(QVariant, QVariant)));
    //QObject::connect(propertyview, SIGNAL(sigItemSelectionAdded(QVariant)), this, SLOT(slotItemSelectionAdded(QVariant)));
}

FDYNPropertyWidget::~FDYNPropertyWidget()
{
    //delete m_view;
}

void FDYNPropertyWidget::update()
{
    if(!m_RootItem)
        return;
    //model()->removeRows(0,m_RootItem->childCount());
    insertQMLTopRows();
}

void FDYNPropertyWidget::insertQMLRows(FDYNPropertyItem *item)
{
//    if(item->childCount() > 0)
//    {
//        Q_FOREACH(FDYNPropertyItem* child, item->children())
//        {
//            insertQMLRows(child);
//        }
//    }
}

void FDYNPropertyWidget::insertQMLTopRows()
{
    if(!m_RootItem)
        return;

    if(m_RootItem->childCount() > 0)
    {
        Q_FOREACH(FDYNPropertyItem* child, m_RootItem->takeChildren())
        {
            int itemNum = m_RootItem->indexofChild(child);
            QModelIndex index = model()->index(0, 0);

            if (!model()->insertRow(itemNum, index.parent()))
                return;

            //insertQMLRows(child);
        }
    }
}

void FDYNPropertyWidget::slotItemSelected(QVariant idx) {
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    selectedRow = qmi.row();
    selectedColumn = qmi.column();
    selectedNode = m_FDYNPropertyModel->getItem(qmi);
}

void FDYNPropertyWidget::slotItemExpanded(QVariant idx) {
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    emit expanded(qmi);
    FDYNPropertyItem *item = m_FDYNPropertyModel->getItem(qmi);
    emit itemExpanded(item);
    //qDebug() << "slotItemExpanded:" << idx;
}

void FDYNPropertyWidget::slotItemCollapsed(QVariant idx) {
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    emit collapsed(qmi);
    FDYNPropertyItem *item = m_FDYNPropertyModel->getItem(qmi);
    emit itemCollapsed(item);
    //qDebug() << "slotItemCollapsed:" << idx;
}

void FDYNPropertyWidget::slotItemClicked(QVariant idx) {
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    //qDebug() << "slotItemClicked:" << idx;
}

void FDYNPropertyWidget::slotItemSelectionChanged(QVariant selected, QVariant deselected)
{
    QItemSelection qmis = selected.value<QItemSelection>();
    QItemSelection dqmis = deselected.value<QItemSelection>();

    foreach (QModelIndex dqmi, dqmis.indexes()) {
        FDYNPropertyItem* item = m_FDYNPropertyModel->getItem(dqmi);
        m_selectedItems.removeOne(item);

    }
    foreach (QModelIndex qmi, qmis.indexes()) {
        FDYNPropertyItem* item = m_FDYNPropertyModel->getItem(qmi);
        m_selectedItems.append(item);
    }

    emit itemSelectionChanged();
}

void FDYNPropertyWidget::expandItem(FDYNPropertyItem *item)
{
    QModelIndex index = indexFromItem(item);
    QMetaObject::invokeMethod(propertyview, "itemExpand",
            Q_ARG(QVariant, index));
}

void FDYNPropertyWidget::collapseItem(FDYNPropertyItem *item)
{
    QModelIndex index = indexFromItem(item);
    QMetaObject::invokeMethod(propertyview, "itemCollapse",
            Q_ARG(QVariant, index));
}

/// Property Widget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation
uint FDYNPropertyWidget::topLevelItemCount()
{
    if (m_RootItem)
        return m_RootItem->childCount();
    return 0;
}

FDYNPropertyItem *FDYNPropertyWidget::takeTopLevelItem(uint idx)
{
    return m_RootItem->child(idx);
}

void FDYNPropertyWidget::addTopLevelItem(FDYNPropertyItem *item)
{
    item->setTreeWidget(this);
    m_RootItem->addChild(item);
}

FDYNPropertyItem *FDYNPropertyWidget::currentItem()
{
    return m_currentItem;
}

QList<FDYNPropertyItem *> FDYNPropertyWidget::selectedItems() const
{
    return m_selectedItems;
}

void FDYNPropertyWidget::subItems(FDYNPropertyItem *item)
{
    m_allItems.append(item);
    for( int i = 0;i < item->childCount(); i++){
        m_allItems.append(item->child(i));
        if(item->child(i)->childCount() > 0)
            subItems(item->child(i));
    }

}

QList<FDYNPropertyItem *> &FDYNPropertyWidget::items()
{
    m_allItems.clear();
    m_allItems = m_RootItem->childrenAll();
    return m_allItems;
}

QModelIndex FDYNPropertyWidget::indexFromItem(FDYNPropertyItem *item, int column) const
{
    return m_FDYNPropertyModel->indexFromItem(item, column);

}

QModelIndex FDYNPropertyWidget::currentIndex() const
{
    return indexFromItem(m_currentItem);
}

void FDYNPropertyWidget::setCurrentIndex(const QModelIndex & index)
{

}

void FDYNPropertyWidget::edit(const QModelIndex & index)
{

}

FDYNPropertyItem *FDYNPropertyWidget::itemAt(const QPoint & p) const
{
    FDYNPropertyItem *node = NULL;
    return node;
}

void FDYNPropertyWidget::editItem(FDYNPropertyItem* item, int column)
{

}

FDYNPropertyItem *FDYNPropertyWidget::itemFromIndex(const QModelIndex & index) const
{
    static FDYNPropertyItem *node;
    return node;
}

bool FDYNPropertyWidget::rootIsDecorated() const
{
    return false;
}
void FDYNPropertyWidget::setRootIsDecorated(bool show)
{

}

int	FDYNPropertyWidget::indentation() const
{

    return 0;
}
void FDYNPropertyWidget::setIndentation(int i)
{

}

QSize FDYNPropertyWidget::iconSize() const
{
    return QSize();
}
void FDYNPropertyWidget::setIconSize(const QSize & size)
{

}

int	FDYNPropertyWidget::columnCount() const
{
    return m_RootItem->dataCount();
}
void FDYNPropertyWidget::setColumnCount(int columns)
{

}

bool FDYNPropertyWidget::isAnimated() const
{
    return false;
}
void FDYNPropertyWidget::setAnimated(bool enable)
{

}

void FDYNPropertyWidget::setHeaderLabels(const QStringList & labels)
{

}

bool FDYNPropertyWidget::alternatingRowColors() const
{
    return true;
}
void FDYNPropertyWidget::setAlternatingRowColors(bool enable)
{

}

int FDYNPropertyWidget::editTriggers() const
{
    return 0;
}
void FDYNPropertyWidget::setEditTriggers(int triggers)
{

}

FDYNPropertyItem* FDYNPropertyWidget::invisibleRootItem() const
{
    FDYNPropertyItem *node = NULL;
    return node;

}

void FDYNPropertyWidget::clear()
{
    m_FDYNPropertyModel->removeRows(0, m_RootItem->childCount());
}

void FDYNPropertyWidget::setItemDelegate(QAbstractItemDelegate * delegate)
{

}

void FDYNPropertyWidget::setCurrentItem(FDYNPropertyItem *item)
{
    m_currentItem = item;
    if(item){
        QModelIndex idx = indexFromItem(item);

        QVariant returnValue;
        QMetaObject::invokeMethod(propertyview, "setCurrentItemFromCpp", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, idx));
    }
    else{
        QVariant returnValue;
        QMetaObject::invokeMethod(propertyview, "clearCurrentItemFromCpp", Q_RETURN_ARG(QVariant, returnValue));
    }
}

void FDYNPropertyWidget::setCurrentItem(FDYNPropertyItem* item, int index)
{

}

QList<FDYNPropertyItem *> FDYNPropertyWidget::findItems(const QString &text, Qt::MatchFlags flags, int column) const
{
    mResultList.clear();
    findSubItems(m_RootItem, text, flags, column);
    return mResultList;
}

// find items recursively
void FDYNPropertyWidget::findSubItems(FDYNPropertyItem* item, const QString &text, Qt::MatchFlags flags, int column) const
{
    for (int i = 0; i < item->childCount(); i ++) {
        if (item->child(i)->text(column) == text)
        {
            mResultList.append(item->child(i));
        }

        if (item->child(i)->childCount() > 0) {
            findSubItems(item->child(i), text, flags, column);
        }
    }
}

/// TreeWidget Integration - end


void FDYNPropertyWidget::thisFunctionCalled(QModelIndex index)
{
    //qDebug() << index.column() << index.row();
}
