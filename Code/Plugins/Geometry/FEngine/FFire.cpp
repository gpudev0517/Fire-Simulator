#include "FFire.h"

class FFirePrivate : public QObject
{
public:
    FFirePrivate(FFire* s) : S(s)
    {
    }

    /* Functions */

    /* Properties */
    FFire::FireIgnitionType m_FireIgnition;
    NEFireMaterial m_FireMaterial;
    uint m_CompartmentID;

private:
    FFire* S;
};

FFireFactory::FFireFactory(NEManager* m) : NENodeFactory(m)
{
}

FFireFactory::~FFireFactory()
{
}

QString FFireFactory::nodeName()
{
    return "Fire Definition";
}

uint FFireFactory::nodeType()
{
    return NE::kObjectPhysical;
}

QString FFireFactory::nodeInherits()
{
    return "Physical";
}

QString FFireFactory::objectName()
{
    return "FSource";
}

QString FFireFactory::nodeVersion()
{
    return "0.1";
}

QString FFireFactory::nodeVendor()
{
    return "F Dynamics";
}

QString FFireFactory::nodeHelp()
{
    return "Fire Definition";
}

NEObject* FFireFactory::createInstance()
{
    return new FFire(this);
}

NE_PROP_DEF(FFire::FireIgnitionType, FFire, FireIgnition, FFire::Time)
NE_PROP_DEF(NEFireMaterial, FFire, FireMaterial, NEFireMaterial(""))

static void initProperties()
{
    NE_INIT_PROP(FFire, FireIgnition);
    NE_INIT_PROP(FFire, FireMaterial);
}

FFire::FFire(NENodeFactory *factory) : FZoneItem(factory)
{
    P = new FFirePrivate(this);

    addNodeTypeToNodeGraphSceneType("Physical", NENode::SceneType::Simulation);

    //hideProperty("PlantItemType", true );

    CALL_ONCE(initProperties);
    P->m_FireIgnition = NE_DEFAULT(FireIgnition);
    P->m_FireMaterial = NE_DEFAULT(FireMaterial);

    SetColor( QColor( 255, 85, 0.0 )) ;

    //hideProperties( "NEZoneItem" );
}

FFire::~FFire()
{
    delete P;
}

void FFire::copyValues(const NEZoneItem *zoneitem)
{
    FFire* fire = qobject_cast< FFire* >( const_cast< NEZoneItem *>( zoneitem ) );
    if( !fire )
    {
        FZoneItem* fzoneitem = qobject_cast< FZoneItem* >(  const_cast< NEZoneItem *>( zoneitem ) );
        if( fzoneitem )
            FZoneItem::copyValues( fzoneitem );
        return;
    }

    SetID(fire->ID());
    SetType(fire->Type());
    SetDesc(fire->Desc());
    SetIssues(fire->Issues());
    SetFireIgnition(fire->FireIgnition());
    SetFireMaterial(fire->FireMaterial());
    setModel3DInfo( fire->model3DInfo() );
    FZoneItem::copyValues( fire );
}

const NEFireMaterial& FFire::FireMaterial() const
{
    return P->m_FireMaterial;
}

const FFire::FireIgnitionType& FFire::FireIgnition() const
{
    return P->m_FireIgnition;
}

uint FFire::SetFireMaterial(const NEFireMaterial& val)
{
    NE_PROP_SET_EVAL(FireMaterial, P, val);

    return NE::kReturnSuccess;
}

uint FFire::SetFireIgnition(const FFire::FireIgnitionType& val)
{
    NE_PROP_SET_EVAL(FireIgnition, P, val);

    return NE::kReturnSuccess;
}

uint FFire::init(uint initMode)
{
    uint ret = FZoneItem::init(initMode);
    if (ret != NE::kReturnSuccess)
        return ret;

    SetItemType( FFire::Sources );
    return NE::kReturnSuccess;
}

void FFire::resetProperties()
{
    NENode::resetProperties();

    ResetFireIgnition();
    ResetFireMaterial();
}

QString FFire::categoryType()
{
    return "Sources";
}
