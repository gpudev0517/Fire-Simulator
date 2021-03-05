#pragma once
#include <QList>
#include <QObject>
#include <QVariant>
//#include "FDYNTreeWidget.h"

class FDYNPropertyWidget;
class FDYNPropertyItem: public QObject
{
    Q_OBJECT
public:

    enum ItemType {Type, UserType};
    explicit FDYNPropertyItem(FDYNPropertyItem *parent = nullptr);
    FDYNPropertyItem(FDYNPropertyWidget *parent);
    FDYNPropertyItem(FDYNPropertyItem *parent, FDYNPropertyItem *preceding, int type = Type);
    FDYNPropertyItem(FDYNPropertyWidget *parent, FDYNPropertyItem *preceding, int type = Type);
    ~FDYNPropertyItem();

    int dataCount() const;

    QVariant newCustomType();

    /// TreeWidget Integration - end
private:
    QVector<QVariant> m_itemData;
    QList<FDYNPropertyItem *> m_childItems;
    FDYNPropertyWidget* m_parentWidget;
    FDYNPropertyItem *m_parentItem = nullptr;

    bool m_expanded;

    void clear();

public:
    void setTreeWidget(FDYNPropertyWidget* widget){m_parentWidget = widget;}

    FDYNPropertyWidget* treeWidget(){return m_parentWidget;}

    void addChild(FDYNPropertyItem *child);
    void insertChild(int pos, FDYNPropertyItem *child);
    void appendChild(FDYNPropertyItem *child);
    void addChildren( const QList<FDYNPropertyItem *> & childrens);
    QList<FDYNPropertyItem *> takeChildren();
    bool insertChildren(int position, int count, int columns);
    void removeChild(FDYNPropertyItem *child);
    bool removeChildren(int position, int count);
    FDYNPropertyItem *child(int index) const;
    int childCount() const;
    int indexofChild(FDYNPropertyItem* child) const;

    void setData(int index, int role, const QVariant &data);
    QVariant data(int index) const;

    FDYNPropertyItem *parent() const;

    QString text(int column) const;
    void setText(int column, QString text );

    QVariant extra() const;
    void setExtra(QVariant extra );

    double extra1() const;
    void setExtra1(double extra );

    double extra2() const;
    void setExtra2(double extra );

    double extra3() const;
    void setExtra3(double extra );

    QVariant value() const;
    void setValue(QVariant extra );

    void setExpanded(bool expand);
    bool	isExpanded() const;

    Qt::ItemFlags	flags() const;
    void setFlags(Qt::ItemFlags flags);

    int	type() const;
    void setType(int _type);

    int materialIndex() const;
    void setMaterialIndex(int val);

    QVariant materialList() const;
    void setMaterialList(QVariant val);

    int index() const;
    void setIndex(int index);

    void	setIcon(int column, const QString &iconUrl);
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

    QList<FDYNPropertyItem* > childrenAll();

    void printCustomType();

signals:
    //temp works to demo
};



