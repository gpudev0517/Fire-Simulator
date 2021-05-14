#include "FDYNPropertyItem.h"
#include "FDYNPropertyWidget.h"
#include "FDYNPropertyCustomtype.h"
#include "FDYNPropertyModel.h"

FDYNPropertyItem::FDYNPropertyItem(FDYNPropertyItem *parent) : m_parentItem(parent)
{
    clear();
    m_itemData.append(newCustomType());
}

FDYNPropertyItem::FDYNPropertyItem(FDYNPropertyWidget *parent)
{
    clear();
    m_parentWidget = parent;
    m_itemData.append(newCustomType());
}

FDYNPropertyItem::FDYNPropertyItem(FDYNPropertyItem *parent, FDYNPropertyItem *after, int type)
{
    clear();
    m_parentItem = parent;
    m_parentItem->m_parentWidget = parent->m_parentWidget;

    m_itemData.append(newCustomType());

    if (parent) {

        int i = parent->m_childItems.indexOf(after) + 1;
        parent->insertChild(i, this);
    }
}

FDYNPropertyItem::FDYNPropertyItem(FDYNPropertyWidget *parent, FDYNPropertyItem *after, int type)
{
    clear();
    m_parentWidget = parent;

    m_itemData.append(newCustomType());

    if (parent) {

        FDYNPropertyModel *model = qobject_cast<FDYNPropertyModel*>(parent->model());
        if (model) {

            int i = model->rootItem()->m_childItems.indexOf(after) + 1;
            model->rootItem()->insertChild(i, this);
            //model->m_RootItem->m_childItems.insert(i, this);
            //values.reserve(model->headerItem->columnCount());
#if QT_VERSION >= QT_VERSION_CHECK(5,12,0)
            model->insertRow(i);
#endif
        }

    }
}

FDYNPropertyItem::~FDYNPropertyItem()
{
//    qDeleteAll(m_childItems);
//    if(m_parentWidget)
//        delete m_parentWidget;
//    if(m_parentItem)
//        delete m_parentItem;
}

void FDYNPropertyItem::clear()
{
    m_itemData.clear();
    m_parentWidget = nullptr;
    m_parentItem = nullptr;
    m_childItems.clear();
    m_expanded = true;
}
void FDYNPropertyItem::removeChild(FDYNPropertyItem *child)
{
    if(m_childItems.removeOne(child)) {
        delete child;
    }
}

bool FDYNPropertyItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        m_childItems.removeAt(position);
        //delete m_childItems.takeAt(position);

    return true;
}

void FDYNPropertyItem::setData(int index, int role, const QVariant &data)
{
    m_itemData[index] = data;
}

FDYNPropertyItem *FDYNPropertyItem::child(int index) const
{
    return m_childItems.value(index);
}

FDYNPropertyItem *FDYNPropertyItem::parent() const
{
    return m_parentItem;
}

int FDYNPropertyItem::childCount() const
{
    return m_childItems.count();
}

int FDYNPropertyItem::indexofChild(FDYNPropertyItem *child) const
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

QVariant FDYNPropertyItem::data(int index) const
{
    return m_itemData.at(index);
}

int FDYNPropertyItem::dataCount() const
{
    return m_itemData.count();
}

void FDYNPropertyItem::setText(int column, QString text)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setName(text);
}


void FDYNPropertyItem::setExtra(QVariant extra)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setExtra(extra);
}

QVariant FDYNPropertyItem::extra() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->extra();
}

void FDYNPropertyItem::setExtra1(double extra)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setExtra1(extra);
}

double FDYNPropertyItem::extra1() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->extra1();
}
void FDYNPropertyItem::setExtra2(double extra)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setExtra2(extra);
}

double FDYNPropertyItem::extra2() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->extra2();
}

void FDYNPropertyItem::setExtra3(double extra)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setExtra3(extra);
}

double FDYNPropertyItem::extra3() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->extra3();
}

QVariant FDYNPropertyItem::value() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->value();
}
void FDYNPropertyItem::setValue(QVariant value )
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setValue(value);
}

bool FDYNPropertyItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        //QVariant> data(columns);
        FDYNPropertyItem *item = new FDYNPropertyItem;
        m_childItems.insert(position, item);
    }

    return true;
}

void FDYNPropertyItem::addChild(FDYNPropertyItem *childNode)
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

void FDYNPropertyItem::insertChild(int pos, FDYNPropertyItem *child)
{
    m_childItems.insert(pos, child);
    child->m_parentItem = this;
    child->m_parentWidget = this->m_parentWidget;
}

void FDYNPropertyItem::addChildren(const QList<FDYNPropertyItem *> &childrens)
{
    Q_FOREACH(FDYNPropertyItem* child, childrens)
    {
        addChild(child);
    }
}

QList<FDYNPropertyItem *> FDYNPropertyItem::takeChildren()
{
    return m_childItems;
}

QList<FDYNPropertyItem*> FDYNPropertyItem::childrenAll()
{
    QList<FDYNPropertyItem*> allChildren;
    if(childCount() > 0)
    {
        Q_FOREACH(FDYNPropertyItem* child, takeChildren())
        {
            allChildren.append(child);
            if(child->childCount() > 0)
                allChildren.append(child->childrenAll());
        }
    }
    return allChildren;
}

void FDYNPropertyItem::setExpanded(bool expand)
{
    m_expanded = expand;
//    if(m_expanded)
//        treeWidget()->expandItem(this);
//    else
//        treeWidget()->collapseItem(this);
}

Qt::ItemFlags FDYNPropertyItem::flags() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->flags();
}

void FDYNPropertyItem::setFlags(Qt::ItemFlags flags)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setFlags(flags);
}

int	FDYNPropertyItem::type() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->type();
}
void FDYNPropertyItem::setType(int _type)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setType(_type);
}

int FDYNPropertyItem::materialIndex() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->materialIndex();
}
void FDYNPropertyItem::setMaterialIndex(int val)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setMaterialIndex(val);
}

QVariant FDYNPropertyItem::materialList() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->materialList();
}
void FDYNPropertyItem::setMaterialList(QVariant val)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setMaterialList(val);
}

int FDYNPropertyItem::index() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->index();
}
void FDYNPropertyItem::setIndex(int index)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    t->setIndex(index);
}

QString FDYNPropertyItem::text(int column) const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData[column].value<QObject*>());
    return t->name();
}

bool FDYNPropertyItem::isExpanded() const
{
    return m_expanded;
}

void FDYNPropertyItem::setIcon(int column, const QString &iconUrl)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setIcon(iconUrl);
}

void FDYNPropertyItem::setIcon(int column, const QIcon &icon)
{
    if(m_itemData.count() < column + 1 )
        return;
//    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(column).value<QObject*>());
//    t->setIcon(icon);
}

void FDYNPropertyItem::setToolTip(int column, const QString &toolTip)
{
    if(m_itemData.count() < column + 1 )
        return;
}

/// TreeWidget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation
void FDYNPropertyItem::setSelected(bool selMode)
{
    if(selMode){
        QModelIndex idx = m_parentWidget->indexFromItem(this);

        QVariant returnValue;
        QMetaObject::invokeMethod(m_parentWidget->propertyview, "setSelectItemFromCpp", Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, idx));
    }

}

void FDYNPropertyItem::setBackground(int column, const QBrush &brush)
{

}

void FDYNPropertyItem::setForeground(int column, const QBrush &brush)
{

}

void FDYNPropertyItem::setFont(int column, const QFont &font)
{
    if(m_itemData.count() < column + 1 )
        return;
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(column).value<QObject*>());
    t->setFont(font);
}

void FDYNPropertyItem::setFirstColumnSpanned(bool span)
{

}

void FDYNPropertyItem::setStatusTip(int column, const QString &statusTip)
{

}

void FDYNPropertyItem::setWhatsThis(int column, const QString &whatsThis)
{

}

bool FDYNPropertyItem::isHidden() const
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());
    return t->hidden();
}

void FDYNPropertyItem::setHidden(bool hide)
{
    FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(m_itemData.at(0).value<QObject*>());

    if(hide ){
        FDYNPropertyModel *model = treeWidget()->model();
        if(parent()->indexofChild(this) != -1){
            model->removeRow(parent()->indexofChild(this), model->indexFromItem(parent()));
        }
    }
    else {
        FDYNPropertyModel *model = treeWidget()->model();
        if(parent()->indexofChild(this) == -1){
            int idx = parent()->childCount();
            parent()->insertChild(idx, this);
            QModelIndex index = model->indexFromItem(parent());
            model->insertRow(idx, index);

        }

    }

    t->setHidden(hide);

}

QVariant FDYNPropertyItem::newCustomType()
{
    FDYNPropertyCustomType *t = new FDYNPropertyCustomType(this);

//    t->setName(QString(""));
//    t->setIcon(QIcon(""));
//    t->setStatus(1);

    QVariant v;
    v.setValue(t);
    return v;
}
/// TreeWidget Integration - end

void FDYNPropertyItem::printCustomType()
{
    for(int i = 0; i < m_itemData.size(); i++)
    {
        QVariant v = m_itemData.at(i);
        FDYNPropertyCustomType *t = qobject_cast<FDYNPropertyCustomType*>(v.value<QObject*>());
        qDebug() << t->name() << t->icon();
    }
}
