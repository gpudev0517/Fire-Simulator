#pragma once

#include "Spatial/NESpatialGroup.h"
#include "globals.h"

NE_FWD_DECL_2(class, NEScenario, NE_PRIVATE(NEScenario))

class NECEXP_GEOMETRY NEScenario : public NESpatialGroup
{
    Q_OBJECT
    NE_CLASS("Properties")

    NE_PROP_RW(uint, ID, "Scenario ID", "Scenario ID", true,)
    NE_PROP_RW(QString, Type, "Type", "Scenario Type", true,)
    NE_PROP_RW(QString, Desc, "Scenario Description", "Scenario Description String", true,)

public:
    NEScenario( NENodeFactory* factory );
    virtual ~NEScenario();

    const QString& originalName() const;
    uint setOriginalName(const QString& val);

    virtual QList< QString >& zones() = 0;

    QList<QString> itemNameChildren(const QString& name) const;
    bool itemNameTreeContains(const QString& childName, const QString& parentName) const;
    void addToItemNameTree(const QString& childName, const QString& parentName);
    void removeFromItemNameTree(const QString& childName, const QString& parentName);
    void removeAllFromItemNameTree(const QString& name);
    void renameInItemNameTree(const QString& oldName, const QString& newName);
    void clearItemNameTree();

    virtual QString nodeDescription();

private:
    NE_DECL_PRIVATE(NEScenario)
};
