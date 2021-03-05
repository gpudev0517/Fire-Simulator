#pragma once
#include <QList>
#include <QObject>
#include <QVariant>
//#include "FDYNTreeWidget.h"

class FDYNTreeWidget;
class FDYNTreeItem: public QObject
{
    Q_OBJECT
public:

    enum ItemType {Type, UserType};
    explicit FDYNTreeItem(FDYNTreeItem *parent = nullptr);
    FDYNTreeItem(FDYNTreeWidget *parent);
    FDYNTreeItem(FDYNTreeItem *parent, FDYNTreeItem *preceding, int type = Type);
    FDYNTreeItem(FDYNTreeWidget *parent, FDYNTreeItem *preceding, int type = Type);
    ~FDYNTreeItem();

    int dataCount() const;

    QVariant newCustomType();

    /// TreeWidget Integration - end
private:
    QVector<QVariant> m_itemData;
    QList<FDYNTreeItem *> m_childItems;
    FDYNTreeWidget* m_parentWidget;
    FDYNTreeItem *m_parentItem = nullptr;

    bool m_expanded;

    void clear();

public:
    void setTreeWidget(FDYNTreeWidget* widget){m_parentWidget = widget;}

    FDYNTreeWidget* treeWidget(){return m_parentWidget;}

    void addChild(FDYNTreeItem *child);
    void appendChild(FDYNTreeItem *child);
    void addChildren( const QList<FDYNTreeItem *> & childrens);
    QList<FDYNTreeItem *> takeChildren();
    bool insertChildren(int position, int count, int columns);
    void removeChild(FDYNTreeItem *child);
    bool removeChildren(int position, int count);
    FDYNTreeItem *child(int index) const;
    int childCount() const;
    int indexofChild(FDYNTreeItem* child) const;

    void setData(int index, int role, const QVariant &data);
    QVariant data(int index) const;

    FDYNTreeItem *parent() const;


    QString text(int column) const;
    void setText(int column, QString text );

    void setExpanded(bool expand);
    bool	isExpanded() const;

    Qt::ItemFlags	flags() const;
    void setFlags(Qt::ItemFlags flags);

    void	setIcon(int column, const QString &iconUrl);
    void	setIcon2(int column, const QString &iconUrl);
    void	setIcon(int column, const QIcon &icon);
    void	setToolTip(int column, const QString &toolTip);

    inline int columnCount() const { return m_itemData.count(); }

    /// TreeWidget Integration - start
    /// - need to configure the following signals/slots to the correct
    /// - implementation
    void setSelected( bool selMode );
    //bool isSelected(){return selMode;}

    void setBackground(int column, const QBrush &brush);
    void setForeground(int column, const QBrush &brush);
    void setFont(int column, const QFont &font);

    void setFirstColumnSpanned(bool span);
    void setStatusTip(int column, const QString &statusTip);
    void setWhatsThis(int column, const QString &whatsThis);

    bool isHidden() const;
    void setHidden(bool hide);

    QList<FDYNTreeItem* > childrenAll();

    void printCustomType();

signals:
    //temp works to demo
};



