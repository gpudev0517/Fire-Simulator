#pragma once
#include "FDYNTreeWidget.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QVariant>
#include <QtQuickControls2>

#include "Managers/FDYNGUIManager.h"

FDYNTreeWidget::FDYNTreeWidget(QWidget *parent):
    QQuickWidget(parent)
{

    selectedRow = 0;
    selectedColumn = 0;
    m_currentItem = 0;

    m_RootItem = new FDYNTreeItem(this);

    m_FDYNTreeModel = new FDYNTreeModel(m_RootItem);

    rootContext()->setContextProperty("treeModel", m_FDYNTreeModel);

    setSource(QUrl("qrc:/ScenarioManager.qml"));

    treeview = rootObject()->findChild<QObject*>("treeview");
    txtSearch = rootObject()->findChild<QObject*>("txtSearch");

    QObject::connect(treeview, SIGNAL(sigItemExpanded(QVariant)), this, SLOT(slotItemExpanded(QVariant)));
    QObject::connect(treeview, SIGNAL(sigItemCollapsed(QVariant)), this, SLOT(slotItemCollapsed(QVariant)));
    QObject::connect(treeview, SIGNAL(sigItemClicked(QVariant)), this, SLOT(slotItemClicked(QVariant)));
    QObject::connect(treeview, SIGNAL(sigItemSelectionChanged(QVariant, QVariant)), this, SLOT(slotItemSelectionChanged(QVariant, QVariant)));
    //QObject::connect(treeview, SIGNAL(sigItemSelectionAdded(QVariant)), this, SLOT(slotItemSelectionAdded(QVariant)));
    QObject::connect(txtSearch, SIGNAL(sigTextChanged(QVariant)), this, SLOT(slotSearchTextChanged(QVariant)));
    QObject::connect(txtSearch, SIGNAL(sigEditingFinished(QVariant)), this, SLOT(slotSearchEditingFinished(QVariant)));

    m_dropDownObject = rootObject()->findChild<QObject *>("combo_scena");
    QObject::connect(this, SIGNAL(dropDownItemAdded(QString)), this, SLOT(slotDropDownItemAdded(QString)));
    QObject::connect(m_dropDownObject, SIGNAL(comboValChanged(QString)), this, SLOT(slotDropDownItemChanged(QString)));

    m_otherComboObject = rootObject()->findChild<QObject *>("combo_list");
    QObject::connect(this, SIGNAL(otherItemAdded(QString)), this, SLOT(slotOtherItemAdded(QString)));
    QObject::connect(m_otherComboObject, SIGNAL(comboValChanged(QString)), this, SLOT(slotOtherItemChanged(QString)));
    QObject::connect(m_otherComboObject, SIGNAL(sigSearchTextChanged(QString)), this, SLOT(slotFilterWithSearchText(QString)));


    QObject::connect(treeview, SIGNAL(sigCurrentTextInput(QVariant)), this, SLOT(slotCurrentTextInput(QVariant)));
    QObject::connect(treeview, SIGNAL(sigItemRenamed(QVariant, QString)), this, SLOT(slotItemRenamed(QVariant,QString)));

    //addDropDownItem("DropDown1");
    //addDropDownItem("DropDown2");

    //addOtherListItem("Combo1");
    //addOtherListItem("Combo2");
    //addOtherListItem("Combo3");

}

FDYNTreeWidget::~FDYNTreeWidget()
{
    //delete m_view;
}

void FDYNTreeWidget::update()
{
    if(!m_RootItem)
        return;
    //model()->removeRows(0,m_RootItem->childCount());
    insertQMLTopRows();
}

void FDYNTreeWidget::insertQMLRows(FDYNTreeItem *item)
{
//    if(item->childCount() > 0)
//    {
//        Q_FOREACH(FDYNTreeItem* child, item->children())
//        {
//            insertQMLRows(child);
//        }
//    }
}

void FDYNTreeWidget::insertQMLTopRows()
{

    if(!m_RootItem)
        return;

    if(m_RootItem->childCount() > 0)
    {
        Q_FOREACH(FDYNTreeItem* child, m_RootItem->takeChildren())
        {
            int itemNum = m_RootItem->indexofChild(child);
            QModelIndex index = model()->index(0, 0);

            if (!model()->insertRow(itemNum, index.parent()))
                return;

            //insertQMLRows(child);
        }
    }
}

void FDYNTreeWidget::slotDropDownItemAdded(QString strItem)
{

}

void FDYNTreeWidget::slotDropDownItemChanged(QString strItem)
{
    //std::cout << "drop down is changed" << std::endl;
}

void FDYNTreeWidget::slotOtherItemAdded(QString strItem)
{

}

void FDYNTreeWidget::slotOtherItemChanged(QString strItem)
{

}

void FDYNTreeWidget::slotFilterWithSearchText(QString strSearch)
{
    QStringList searchedList;

    foreach (QString item, m_otherComboList)
    {
        if( item.contains(strSearch, Qt::CaseInsensitive))
        {
            searchedList.push_back(item);
        }
    }
    m_otherComboObject->setProperty("model", searchedList);
}


void FDYNTreeWidget::addDropDownItem(QString item)
{
    m_dropDownList.append(item);
    m_dropDownObject->setProperty("model", m_dropDownList);
    emit dropDownItemAdded(item);

}

void FDYNTreeWidget::modifyDropDownItem(QString orgItem, QString newItem)
{
    int idx = m_dropDownList.indexOf(orgItem);
    m_dropDownList.replace(idx, newItem);
    m_dropDownObject->setProperty("model", m_dropDownList);

}

void FDYNTreeWidget::deleteDropDownItem(QString item)
{
    m_dropDownList.removeOne(item);
    m_dropDownObject->setProperty("model", m_dropDownList);
}
void FDYNTreeWidget::clearDropDownItems()
{
    m_dropDownList.clear();
    m_dropDownObject->setProperty("model", m_dropDownList);
}


void FDYNTreeWidget::addOtherListItem(QString item)
{
    m_otherComboList.append(item);
    m_otherComboObject->setProperty("model", m_otherComboList);
    emit otherItemAdded(item);

}

void FDYNTreeWidget::modifyOtherListItem(QString orgItem, QString newItem)
{
    int idx = m_otherComboList.indexOf(orgItem);
    m_otherComboList.replace(idx, newItem);
    m_otherComboObject->setProperty("model", m_otherComboList);

}

void FDYNTreeWidget::deleteOtherListItem(QString item)
{
    m_otherComboList.removeOne(item);
    m_otherComboObject->setProperty("model", m_otherComboList);
}

void FDYNTreeWidget::clearOtherListItems()
{
    m_otherComboList.clear();
    m_otherComboObject->setProperty("model", m_otherComboList);
}

void FDYNTreeWidget::slotItemSelected(QVariant idx)
{
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    selectedRow = qmi.row();
    selectedColumn = qmi.column();
    selectedNode = m_FDYNTreeModel->getItem(qmi);
}

void FDYNTreeWidget::slotItemExpanded(QVariant idx)
{
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    FDYNTreeItem *item = m_FDYNTreeModel->getItem(qmi);
    emit itemExpanded(item);
    //qDebug() << "slotItemExpanded:" << idx;
}

void FDYNTreeWidget::slotItemCollapsed(QVariant idx)
{
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    FDYNTreeItem *item = m_FDYNTreeModel->getItem(qmi);
    emit itemCollapsed(item);
    //qDebug() << "slotItemCollapsed:" << idx;
}

void FDYNTreeWidget::slotItemClicked(QVariant idx)
{
    //qDebug() << idx;
    QModelIndex qmi = idx.value<QModelIndex>();
    //qDebug() << "slotItemClicked:" << idx;
}

void FDYNTreeWidget::slotItemSelectionChanged(QVariant selected, QVariant deselected)
{
    QItemSelection qmis = selected.value<QItemSelection>();
    QItemSelection dqmis = deselected.value<QItemSelection>();


    foreach (QModelIndex dqmi, dqmis.indexes())
    {
        FDYNTreeItem* item = m_FDYNTreeModel->getItem(dqmi);
        m_selectedItems.removeAll(item);
    }

    foreach (QModelIndex qmi, qmis.indexes())
    {
        FDYNTreeItem* item = m_FDYNTreeModel->getItem(qmi);
        if(m_selectedItems.contains(item))
            m_selectedItems.removeAll(item);
        m_selectedItems.append(item);
    }

    emit itemSelectionChanged();
}


void FDYNTreeWidget::slotSearchTextChanged(QVariant str)
{
    m_searchText = str.value<QString>();
    update();

}

void FDYNTreeWidget::slotSearchEditingFinished(QVariant str)
{
    QString qstr = str.value<QString>();
    if(m_searchMatchedObjects.size() > 0){
        //std::cout << "search enter pressed is " << m_searchMatchedObjects.first()->objectName().toStdString() << ", search text : " << qstr.toStdString() << std::endl;
        FDYNGUI.selectObject(NULL);
        FDYNGUI.selectObject(m_searchMatchedObjects.first());
        m_searchMatchedObjects.clear();

    }

}

void FDYNTreeWidget::slotCurrentTextInput(QVariant varTextInput)
{
    currentTextInput = varTextInput.value<QObject*>();
}

void FDYNTreeWidget::slotItemRenamed(QVariant idx, QString str)
{
    QModelIndex qmi = idx.value<QModelIndex>();
    FDYNTreeItem *item = m_FDYNTreeModel->getItem(qmi);
    if(!item->text(0).isEmpty() && str != item->text(0))
    {
        qDebug() << "slotItemRenamed : " << str << ", item text" << item->text(0) << ", column : " << qmi.column();
        item->setText(0, str);
        emit itemChanged(item, qmi.column());
    }
}

void FDYNTreeWidget::expandItem(FDYNTreeItem *item)
{
    if(item){
        QModelIndex index = indexFromItem(item);
        QMetaObject::invokeMethod(treeview, "itemExpand",
                Q_ARG(QVariant, index));
    }

}

void FDYNTreeWidget::collapseItem(FDYNTreeItem *item)
{
    QModelIndex index = indexFromItem(item);
    QMetaObject::invokeMethod(treeview, "itemCollapse",
            Q_ARG(QVariant, index));
}

/// TreeWidget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation
uint FDYNTreeWidget::topLevelItemCount()
{
    if (m_RootItem)
        return m_RootItem->childCount();
    return 0;
}

FDYNTreeItem *FDYNTreeWidget::takeTopLevelItem(uint idx)
{
    return m_RootItem->child(idx);
}

void FDYNTreeWidget::addTopLevelItem(FDYNTreeItem *item)
{
    item->setTreeWidget(this);
    m_RootItem->addChild(item);
}

FDYNTreeItem *FDYNTreeWidget::currentItem()
{
    return m_currentItem;
}

QList<FDYNTreeItem *> FDYNTreeWidget::selectedItems() const
{
    return m_selectedItems;
}

void FDYNTreeWidget::subItems(FDYNTreeItem *item)
{
    m_allItems.append(item);
    for( int i = 0;i < item->childCount(); i++){
        m_allItems.append(item->child(i));
        if(item->child(i)->childCount() > 0)
            subItems(item->child(i));
    }

}

QList<FDYNTreeItem *> &FDYNTreeWidget::items()
{
    m_allItems.clear();
    m_allItems = m_RootItem->childrenAll();
    return m_allItems;
}

QModelIndex FDYNTreeWidget::indexFromItem(FDYNTreeItem *item, int column) const
{
    return m_FDYNTreeModel->indexFromItem(item, column);
}

QModelIndex FDYNTreeWidget::currentIndex() const
{
    return indexFromItem(m_currentItem);
}

void FDYNTreeWidget::setCurrentIndex(const QModelIndex & index)
{

}

void FDYNTreeWidget::edit(const QModelIndex & index)
{

}

FDYNTreeItem *FDYNTreeWidget::itemAt(const QPoint & p) const
{
    FDYNTreeItem *node = NULL;
    return node;
}

void FDYNTreeWidget::editItem(FDYNTreeItem* item, int column)
{

}

FDYNTreeItem *FDYNTreeWidget::itemFromIndex(const QModelIndex & index) const
{
    static FDYNTreeItem *node;
    return node;
}

bool FDYNTreeWidget::rootIsDecorated() const
{
    return false;
}
void FDYNTreeWidget::setRootIsDecorated(bool show)
{

}

int	FDYNTreeWidget::indentation() const
{

    return 0;
}
void FDYNTreeWidget::setIndentation(int i)
{

}

QSize FDYNTreeWidget::iconSize() const
{
    return QSize();
}
void FDYNTreeWidget::setIconSize(const QSize & size)
{

}

int	FDYNTreeWidget::columnCount() const
{
    return m_RootItem->dataCount();
}
void FDYNTreeWidget::setColumnCount(int columns)
{

}

bool FDYNTreeWidget::isAnimated() const
{
    return false;
}
void FDYNTreeWidget::setAnimated(bool enable)
{

}

void FDYNTreeWidget::setHeaderLabels(const QStringList & labels)
{

}

bool FDYNTreeWidget::alternatingRowColors() const
{
    return true;
}
void FDYNTreeWidget::setAlternatingRowColors(bool enable)
{

}

int FDYNTreeWidget::editTriggers() const
{
    return 0;
}
void FDYNTreeWidget::setEditTriggers(int triggers)
{

}

FDYNTreeItem* FDYNTreeWidget::invisibleRootItem() const
{
    FDYNTreeItem *node = NULL;
    return node;

}

void FDYNTreeWidget::clear()
{
    m_FDYNTreeModel->removeRows(0, m_RootItem->childCount());
}

void FDYNTreeWidget::setItemDelegate(QAbstractItemDelegate * delegate)
{

}

void FDYNTreeWidget::setCurrentItem(FDYNTreeItem *item)
{
    m_currentItem = item;
    if(item)
    {
        QModelIndex idx = indexFromItem(item);

        QVariant returnValue;
        QMetaObject::invokeMethod(treeview, "setCurrentItemFromCpp", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, idx));
    }
    else
    {
        QVariant returnValue;
        QMetaObject::invokeMethod(treeview, "clearCurrentItemFromCpp", Q_RETURN_ARG(QVariant, returnValue));
    }
}

void FDYNTreeWidget::setCurrentItem(FDYNTreeItem* item, int index)
{

}

QList<FDYNTreeItem *> FDYNTreeWidget::findItems(const QString &text, Qt::MatchFlags flags, int column) const
{
    mResultList.clear();
    findSubItems(m_RootItem, text, flags, column);
    return mResultList;
}

// find items recursively
void FDYNTreeWidget::findSubItems(FDYNTreeItem* item, const QString &text, Qt::MatchFlags flags, int column) const
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


void FDYNTreeWidget::thisFunctionCalled(QModelIndex index)
{
}
