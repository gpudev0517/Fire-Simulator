#include "FDYNPropertyModel.h"

#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "FDYNPropertyCustomtype.h"

FDYNPropertyModel::FDYNPropertyModel(QObject *parent) : QAbstractItemModel(parent)
{
    mRoleNameMap[FDYNPropertyModelRoleName] = "name";
    mRoleNameMap[FDYNPropertyModelRoleType] = "type";
    m_RootItem = (FDYNPropertyItem* )parent;
}

FDYNPropertyModel::~FDYNPropertyModel()
{

}

/// TreeWidget Integration - start
/// - need to configure the following signals/slots to the correct
/// - implementation

/// TreeWidget Integration - end


FDYNPropertyItem *FDYNPropertyModel::getItem(const QModelIndex &index) const {
    if (index.isValid())
    {
        auto node = static_cast<FDYNPropertyItem *>(index.internalPointer());
        if (node) {
            return node;
        }
    }
    return m_RootItem;
}

QModelIndex FDYNPropertyModel::indexFromItem(FDYNPropertyItem *item, int column)
{
    FDYNPropertyItem* parentItem = item->parent();
    if(!parentItem)
        return QModelIndex();
    int row = parentItem->indexofChild(item);
    return createIndex(row,column,item);
}

QVariant FDYNPropertyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
    {
        return QModelIndex();
    }
    if (role >= FDYNPropertyModelRoleName)
    {
        auto node = getItem(index);
        return node->data(role - FDYNPropertyModelRoleName);
    } else {
        return QVariant();
    }
}

QModelIndex FDYNPropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
    {
        return QModelIndex();
    }
    FDYNPropertyItem* parentNode = getItem(parent);
    FDYNPropertyItem* childNode = parentNode->child(row);
    if (childNode)
    {
        QModelIndex createdIndex = createIndex(row, column, childNode);
        return createdIndex;
    } else
    {
        return QModelIndex();
    }
}

QModelIndex FDYNPropertyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }
    FDYNPropertyItem* childNode = getItem(child);
    FDYNPropertyItem* parentNode = childNode->parent();
    if (parentNode == m_RootItem) {
        return QModelIndex();
    }
    return createIndex(parentNode->parent()->indexofChild(parentNode), 0, parentNode);
}

int FDYNPropertyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_RootItem->dataCount();
}

int FDYNPropertyModel::rowCount(const QModelIndex &parent) const
{
    return getItem(parent)->childCount();
}

bool FDYNPropertyModel::hasChildren(const QModelIndex &parent) const
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


bool FDYNPropertyModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    if(rows == 0)
        return true;
    //FDYNPropertyItem *m_parentItem = getItem(parent);
    beginInsertRows(parent, position, position + rows - 1);
    //m_parentItem->insertChildren(position, rows, 1);
    endInsertRows();

    return true;
}

bool FDYNPropertyModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    if(rows == 0)
        return true;

    FDYNPropertyItem *m_parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = m_parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

bool FDYNPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole) return false;

    FDYNPropertyItem *node = getItem(index);
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

QHash<int, QByteArray> FDYNPropertyModel::roleNames() const
{
    return mRoleNameMap;
}

Qt::ItemFlags FDYNPropertyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return 0;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
