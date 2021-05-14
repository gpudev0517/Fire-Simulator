#include "FDYNTreeModel.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "FDYNTreeCustomtype.h"

FDYNTreeModel::FDYNTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
    mRoleNameMap[FDYNTreeModelRoleName] = "name";
    mRoleNameMap[FDYNTreeModelRoleType] = "type";
    m_RootItem = (FDYNTreeItem* )parent;
}

FDYNTreeModel::~FDYNTreeModel()
{

}

/// TreeWidget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation

/// TreeWidget Integration - end


FDYNTreeItem *FDYNTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto node = static_cast<FDYNTreeItem *>(index.internalPointer());
        if (node) {
            return node;
        }
    }
    return m_RootItem;
}

QModelIndex FDYNTreeModel::indexFromItem(FDYNTreeItem *item, int column)
{
    FDYNTreeItem* parentItem = item->parent();
    if(!parentItem)
        return QModelIndex();
    int row = parentItem->indexofChild(item);
    return createIndex(row,column,item);
}

QVariant FDYNTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
    {
        return QModelIndex();
    }
    if (role >= FDYNTreeModelRoleName)
    {
        auto node = getItem(index);
        return node->data(role - FDYNTreeModelRoleName);
    } else {
        return QVariant();
    }
}

QModelIndex FDYNTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
    {
        return QModelIndex();
    }
    FDYNTreeItem* parentNode = getItem(parent);
    FDYNTreeItem* childNode = parentNode->child(row);
    if (childNode)
    {
        QModelIndex createdIndex = createIndex(row, column, childNode);
        return createdIndex;
    } else
    {
        return QModelIndex();
    }
}

QModelIndex FDYNTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }
    FDYNTreeItem* childNode = getItem(child);
    FDYNTreeItem* parentNode = childNode->parent();
    if (parentNode == m_RootItem) {
        return QModelIndex();
    }
    return createIndex(parentNode->parent()->indexofChild(parentNode), 0, parentNode);
}

int FDYNTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_RootItem->dataCount();
}

int FDYNTreeModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent)->childCount();
}

bool FDYNTreeModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) return false;
    auto node = getItem(parent);
    if (node)
    {
        return node->childCount() > 0;
    } else
    {
        return false;
    }
}


bool FDYNTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(parent, position, position + rows - 1);
    endInsertRows();

    return true;
}

bool FDYNTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if(rows == 0)
        return true;

    FDYNTreeItem *m_parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = m_parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

bool FDYNTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;

    FDYNTreeItem *node = getItem(index);
    auto column = index.column();
    if (column < node->dataCount())
    {
        node->setData(column, Qt::EditRole, value);
        emit dataChanged(index, index);
        return true;
    }
    else
    {
        return false;
    }
}

QHash<int, QByteArray> FDYNTreeModel::roleNames() const
{
    return mRoleNameMap;
}

Qt::ItemFlags FDYNTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
