#include "FDYNTreeItem.h"
#include "FDYNTreeWidget.h"
#include "FDYNTreeCustomtype.h"
#include "FDYNTreeModel.h"

FDYNTreeItem::FDYNTreeItem(FDYNTreeItem *parent) : m_parentItem(parent)
{
    clear();
    m_itemData.append(newCustomType());
    m_itemData.append(newCustomType());
}

FDYNTreeItem::FDYNTreeItem(FDYNTreeWidget *parent)
{
    clear();
    m_parentWidget = parent;
    m_itemData.append(newCustomType());
    m_itemData.append(newCustomType());
}

FDYNTreeItem::FDYNTreeItem(FDYNTreeItem *parent, FDYNTreeItem *preceding, int type)
{
    clear();
    m_parentItem = parent;
    m_parentItem->m_parentWidget = parent->m_parentWidget;
}

FDYNTreeItem::FDYNTreeItem(FDYNTreeWidget *parent, FDYNTreeItem *preceding, int type)
{
    clear();
    m_parentWidget = parent;
}

FDYNTreeItem::~FDYNTreeItem()
{
    qDeleteAll(m_childItems);
    delete m_parentWidget;
    delete m_parentItem;
}

void FDYNTreeItem::clear()
{
    m_itemData.clear();
    m_parentWidget = nullptr;
    m_parentItem = nullptr;
    m_childItems.clear();
    m_expanded = true;
}
void FDYNTreeItem::removeChild(FDYNTreeItem *child)
{
    if(m_childItems.removeOne(child)) {
        delete child;
    }
}

bool FDYNTreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        m_childItems.removeAt(position);
        //delete m_childItems.takeAt(position);

    return true;
}

void FDYNTreeItem::setData(int index, int role, const QVariant &data)
{
    m_itemData[index] = data;
}

FDYNTreeItem *FDYNTreeItem::child(int index) const
{
    return m_childItems.value(index);
}

FDYNTreeItem *FDYNTreeItem::parent() const
{
    return m_parentItem;
}

int FDYNTreeItem::childCount() const
{
    return m_childItems.count();
}

int FDYNTreeItem::indexofChild(FDYNTreeItem *child) const
{

    for(int i = 0; i < this->childCount(); i++)
    {
       if(this->child(i) == child)
       {
           return i;
       }
    }

    return -1;
}

QVariant FDYNTreeItem::data(int index) const
{
    return m_itemData.at(index);
}

int FDYNTreeItem::dataCount() const
{
    return m_itemData.count();
}

void FDYNTreeItem::setText(int column, QString text)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setName(text);
}

bool FDYNTreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        //QVariant> data(columns);
        FDYNTreeItem *item = new FDYNTreeItem;
        m_childItems.insert(position, item);
    }

    return true;
}

void FDYNTreeItem::addChild(FDYNTreeItem *childNode)
{
    m_childItems.append(childNode);
    childNode->m_parentItem = this;
    childNode->m_parentWidget = this->m_parentWidget;
//    if(!m_parentWidget)
//    {
//        m_childItems.append(childNode);
//        return;
//    }
//    tempAddItem = childNode;

//    childNode->m_parentWidget = this->m_parentWidget;
//    FDYNTreeModel* model = m_parentWidget->model();

//    int i = childCount();
//    if (!model->insertRow(childCount(), m_modelIndex))
//        return ;
//    i = childCount();
//    QModelIndex child = model->index(childCount()-1, 0, m_modelIndex);
//    model->setData(child, childNode->data(0), Qt::EditRole);
//    childNode->m_modelIndex = child;

//    QModelIndex child2 = model->index(childCount()-1, 1, m_modelIndex);
//    model->setData(child2, childNode->data(1), Qt::EditRole);

}

void FDYNTreeItem::addChildren(const QList<FDYNTreeItem *> &childrens)
{
    Q_FOREACH(FDYNTreeItem* child, childrens)
    {
        addChild(child);
    }
}

QList<FDYNTreeItem *> FDYNTreeItem::takeChildren()
{
    return m_childItems;
}

QList<FDYNTreeItem*> FDYNTreeItem::childrenAll()
{
    QList<FDYNTreeItem*> allChildren;
    if(childCount() > 0)
    {
        Q_FOREACH(FDYNTreeItem* child, takeChildren())
        {
            allChildren.append(child);
            if(child->childCount() > 0)
                allChildren.append(child->childrenAll());
        }
    }
    return allChildren;
}

void FDYNTreeItem::setExpanded(bool expand)
{
    m_expanded = expand;
//    if(m_expanded)
//        treeWidget()->expandItem(this);
//    else
//        treeWidget()->collapseItem(this);
}

Qt::ItemFlags FDYNTreeItem::flags() const
{
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->flags();
}

void FDYNTreeItem::setFlags(Qt::ItemFlags flags)
{
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setFlags(flags);
}

QString FDYNTreeItem::text(int column) const
{
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData[column].value<QObject*>());
    return t->name();
}

bool FDYNTreeItem::isExpanded() const
{
    return m_expanded;
}

void FDYNTreeItem::setIcon(int column, const QString &iconUrl)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setIcon(iconUrl);
}

void FDYNTreeItem::setIcon2(int column, const QString &iconUrl)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setIcon2(iconUrl);
}

void FDYNTreeItem::setIcon(int column, const QIcon &icon)
{
    if(m_itemData.count() < column + 1 )
        return;
//    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
//    t->setIcon(icon);
}

void FDYNTreeItem::setToolTip(int column, const QString &toolTip)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setTooltip(toolTip);
}

/// TreeWidget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation
void FDYNTreeItem::setSelected(bool selMode)
{
    if(selMode){
        QModelIndex idx = m_parentWidget->indexFromItem(this);

        QVariant returnValue;
        QMetaObject::invokeMethod(m_parentWidget->treeview, "setSelectItemFromCpp", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, idx));
    }

}

void FDYNTreeItem::setBackground(int column, const QBrush &brush)
{

}

void FDYNTreeItem::setForeground(int column, const QBrush &brush)
{

}

void FDYNTreeItem::setFont(int column, const QFont &font)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setFont(font);
}

void FDYNTreeItem::setFirstColumnSpanned(bool span)
{

}

void FDYNTreeItem::setStatusTip(int column, const QString &statusTip)
{

}

void FDYNTreeItem::setWhatsThis(int column, const QString &whatsThis)
{

}

bool FDYNTreeItem::isHidden() const
{
    return false;
}

void FDYNTreeItem::setHidden(bool hide)
{

}

QVariant FDYNTreeItem::newCustomType()
{
    FDYNTreeCustomType *t = new FDYNTreeCustomType(this);

//    t->setName(QString(""));
//    t->setIcon(QIcon(""));
//    t->setStatus(1);

    QVariant v;
    v.setValue(t);
    return v;
}
/// TreeWidget Integration - end

void FDYNTreeItem::printCustomType()
{
    for(int i = 0; i < m_itemData.size(); i++)
    {
        QVariant v = m_itemData.at(i);
        FDYNTreeCustomType *t = qobject_cast<FDYNTreeCustomType*>(v.value<QObject*>());
        qDebug() << t->name() << t->icon();
    }
}
