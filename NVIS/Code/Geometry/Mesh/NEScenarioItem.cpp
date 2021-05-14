#include "NEScenarioItem.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "Resource/Mesh/NEMeshContainer.h"

class NEScenarioItemPrivate
{
public:
    NEScenarioItemPrivate(NEScenarioItem* s) : S(s)
    {
        m_ID = 0;
    }

    uint m_ID;
    QString m_Type;



private:
    NEScenarioItem* S;
};

NE_PROP_DEF(uint, NEScenarioItem, ID, 0)
NE_PROP_DEF(QString, NEScenarioItem, Type, QString(""))

static void initProperties()
{
    //NE_INIT_PROP(FZoneItem, GeoFile);
    NE_INIT_PROP(NEScenarioItem, ID);
    NE_INIT_PROP(NEScenarioItem, Type);
}


NEScenarioItem::NEScenarioItem(NENodeFactory* factory) : NEZoneItem(factory)
{
    P = new NEScenarioItemPrivate(this);

}


NEScenarioItem::~NEScenarioItem()
{
    delete P;
}


QString NEScenarioItem::nodeDescription()
{
    return factory().nodeName();
}


uint NEScenarioItem::init(uint initMode)
{
    uint ret = NESpatialMesh::init( initMode );
    return ret;
}


const uint& NEScenarioItem::ID() const
{
    return P->m_ID;
}

uint NEScenarioItem::SetID(const uint &ID)
{
    P->m_ID = ID;
    return NE::kReturnSuccess;
}


const QString& NEScenarioItem::Type() const
{
    return P->m_Type;
}

uint NEScenarioItem::SetType(const QString &type)
{
    P->m_Type = type;
    return NE::kReturnSuccess;
}




QString NEScenarioItem::categoryType()
{
    return "None";
}

QString NEScenarioItem::otherCategoryType()
{
    return "";

}

void NEScenarioItem::setItemType(int itype)
{

}
int NEScenarioItem::itemType()
{
    return 0;
}

