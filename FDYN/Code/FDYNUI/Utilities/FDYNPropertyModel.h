#pragma once

#include <QObject>
#include <QAbstractItemModel>
#include <QJsonObject>

#include "FDYNPropertyItem.h"

class FDYNPropertyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum FDYNPropertyModelRoles {
        FDYNPropertyModelRoleName = Qt::UserRole + 1,
        FDYNPropertyModelRoleType,
    };
    explicit FDYNPropertyModel(QObject *parent = nullptr);
    ~FDYNPropertyModel();

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int columnCount(const QModelIndex &parent) const override;
    int rowCount(const QModelIndex &parent) const override;
    bool hasChildren(const QModelIndex &parent) const override;

    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /// TreeWidget Integration - end
public slots:


public:
    void findSubItems(FDYNPropertyItem *node, const QString &text, Qt::MatchFlags flags, int colum = 0) const;
    FDYNPropertyItem *getItem(const QModelIndex &index) const;
    QModelIndex indexFromItem(FDYNPropertyItem* item, int column = 0);

    FDYNPropertyItem *rootItem(){return m_RootItem;}

private:
    FDYNPropertyItem *m_RootItem = nullptr;
    QHash<int, QByteArray> mRoleNameMap;
};

