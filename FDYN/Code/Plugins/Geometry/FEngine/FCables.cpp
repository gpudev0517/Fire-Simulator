#include "FCables.h"

class FCablesPrivate : public QObject
{
public:
    FCablesPrivate(FCables* s) : S(s)
    {
    }

    /* Functions */

    /* Properties */
    NECableProperty m_CableMaterial;

private:
    FCables* S;
};

FCablesFactory::FCablesFactory(NEManager* m) : NENodeFactory(m)
{
}

FCablesFactory::~FCablesFactory()
{
}

QString FCablesFactory::nodeName()
{
    return "Cables";
}

uint FCablesFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FCablesFactory::nodeInherits()
{
    return "Physical";
}

QString FCablesFactory::objectName()
{
    return "FCables";
}

QString FCablesFactory::nodeVersion()
{
    return "0.1";
}

QString FCablesFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FCablesFactory::nodeHelp()
{
    return "Cables";
}

NEObject* FCablesFactory::createInstance()
{
    return new FCables(this);
}

NE_PROP_DEF(NECableProperty, FCables, CableMaterial, NECableProperty(""))

static void initProperties()
{
    NE_INIT_PROP(FCables, CableMaterial);
}

FCables::FCables(NENodeFactory *factory) : FZoneItem(factory)
{
    P = new FCablesPrivate(this);

    CALL_ONCE(initProperties);
    P->m_CableMaterial = NE_DEFAULT(CableMaterial);

    hideProperty("PlantItemType", true);
    hideProperty("Failure", true);
    hideProperty("Material", true);

    SetItemType( FZoneItem::Cable );
    SetSpatialMeshType(NESpatialMeshType::None);
}

FCables::~FCables()
{
    delete P;
}

const NECableProperty& FCables::CableMaterial() const
{
    return P->m_CableMaterial;
}


uint FCables::SetCableMaterial(const NECableProperty& val)
{
    NE_PROP_SET_EVAL(CableMaterial, P, val);

    return NE::kReturnSuccess;
}

uint FCables::init(uint initMode)
{
    uint ret = FZoneItem::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    return NE::kReturnSuccess;
}

void FCables::resetProperties()
{
    FZoneItem::resetProperties();

    ResetCableMaterial();

    SetItemType( FZoneItem::Cable );
    SetSpatialMeshType(NESpatialMeshType::None);
}

QString FCables::categoryType()
{
    return "Cables";
}
