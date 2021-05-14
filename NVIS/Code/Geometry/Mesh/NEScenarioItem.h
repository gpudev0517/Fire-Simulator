#pragma once
#include "Utility/FilePath.h"
#include "Mesh/NEZoneItem.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(NEScenarioItem))


class NECEXP_GEOMETRY NEScenarioItem : public NEZoneItem
{
    Q_OBJECT
    NE_CLASS("Properties")

    NE_PROP_RW(uint, ID, "ID", "Scenario Item ID", true,)
    NE_PROP_RW(QString, Type, "Type", "Type String", true,)

public:
    NEScenarioItem( NENodeFactory* factory );
    virtual ~NEScenarioItem();

    uint init(uint initMode);


    virtual QString nodeDescription();


    virtual QString categoryType();
    virtual QString otherCategoryType();

    virtual void setItemType(int itype);
    virtual int itemType();

private:
    NE_DECL_PRIVATE(NEScenarioItem)

};

