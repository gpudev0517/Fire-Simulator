#include "NEScenario.h"

class NEScenarioPrivate
{
public:
    NEScenarioPrivate(NEScenario* s) : S(s)
    {
    }

    uint m_ID;
    QString m_Type;
    QString m_Desc;

    QString m_OriginalName;
    QMultiMap<QString, QString> m_ItemNameTree;

private:
    NEScenario* S;
};

NE_PROP_DEF(uint, NEScenario, ID, 0)
NE_PROP_DEF(QString, NEScenario, Type, QString(""))
NE_PROP_DEF(QString, NEScenario, Desc, QString(""))

static void initProperties()
{
    NE_INIT_PROP(NEScenario, ID );
    NE_INIT_PROP(NEScenario, Type);
    NE_INIT_PROP(NEScenario, Desc );
}

QString NEScenario::nodeDescription()
{
    return factory().nodeName();
}

NEScenario::NEScenario(NENodeFactory* factory) : NESpatialGroup(factory)
{
    P = new NEScenarioPrivate(this);

    // Initialize the properties
    CALL_ONCE(initProperties);
    P->m_ID = NE_DEFAULT(ID);
    P->m_Type = NE_DEFAULT(Type);
    P->m_Desc = NE_DEFAULT(Desc);
}

NEScenario::~NEScenario()
{
    delete P;
}

const uint& NEScenario::ID() const
{
    return P->m_ID;
}

uint NEScenario::SetID(const uint& val)
{
    NE_PROP_SET_EVAL(ID, P, val);

    return NE::kReturnSuccess;
}

const QString& NEScenario::Type() const
{
    return P->m_Type;
}

uint NEScenario::SetType(const QString& val)
{
    NE_PROP_SET_EVAL(Type, P, val);

    return NE::kReturnSuccess;
}

const QString& NEScenario::originalName() const
{
    return P->m_OriginalName;
}

uint NEScenario::setOriginalName(const QString& val)
{
    P->m_OriginalName = val;

    return NE::kReturnSuccess;
}

const QString& NEScenario::Desc() const
{
    return P->m_Desc;
}

uint NEScenario::SetDesc(const QString& val)
{
    NE_PROP_SET_EVAL(Desc, P, val);

    return NE::kReturnSuccess;
}

QList<QString> NEScenario::itemNameChildren(const QString& name) const
{
    // Sort the children, otherwise they would be in reverse order of insertion
    QList<QString> children = P->m_ItemNameTree.values(name);
    std::sort(children.begin(), children.end());

    return children;
}

bool NEScenario::itemNameTreeContains(const QString& childName, const QString& parentName) const
{
    return P->m_ItemNameTree.contains(parentName, childName);
}

void NEScenario::addToItemNameTree(const QString& childName, const QString& parentName)
{
    if (!P->m_ItemNameTree.contains(parentName, childName))
        P->m_ItemNameTree.insert(parentName, childName);
}

void NEScenario::removeFromItemNameTree(const QString& childName, const QString& parentName)
{
    // Clear the sub-tree under child name
    for (auto& grandChildName : P->m_ItemNameTree.values(childName))
        removeFromItemNameTree(grandChildName, childName);
    P->m_ItemNameTree.remove(childName);

    // Remove the branche
    P->m_ItemNameTree.remove(parentName, childName);
}

void NEScenario::removeAllFromItemNameTree(const QString& name)
{
    // Clear the sub-tree under name
    for (auto& childName : P->m_ItemNameTree.values(name))
        removeFromItemNameTree(childName, name);
    P->m_ItemNameTree.remove(name);

    // Clear the branches to which name is the child
    for (auto& parentName : P->m_ItemNameTree.keys(name))
        P->m_ItemNameTree.remove(parentName, name);
}

void NEScenario::renameInItemNameTree(const QString& oldName, const QString& newName)
{
    // Replace name in values
    for (QMap<QString, QString>::iterator iter = P->m_ItemNameTree.begin(); iter != P->m_ItemNameTree.end(); iter++)
    {
        if (iter.value() == oldName)
            iter.value() = newName;
    }

    // Replace name in keys
    QMultiMap<QString, QString>::iterator iter = P->m_ItemNameTree.find(oldName);
    while (iter != P->m_ItemNameTree.end())
    {
        QString& value = iter.value();
        P->m_ItemNameTree.remove(oldName, value);
        P->m_ItemNameTree.insert(newName, value);

        iter = P->m_ItemNameTree.find(oldName);
    }
}

void NEScenario::clearItemNameTree()
{
    P->m_ItemNameTree.clear();
}
