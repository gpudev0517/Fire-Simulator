#include "FZone.h"
#include "FZoneItem.h"
#include "FEngine.h"
#include "FScenario.h"

#include "FCables.h"

#include "Rendering/Manager/NERenderManager.h"

#include "FFactory.h"

#include "FEngineRenderer.h"
#include "FZoneRenderer.h"
#include "FZoneCompartment.h"
#include "FZoneCompartmentRenderer.h"
#include "FZoneItemRenderer.h"
#include "FVentRenderer.h"
#include "FMechVentRenderer.h"
#include "FFireRenderer.h"
#include "FCablesRenderer.h"

#include "FFireMaterial.h"
#include "FFire.h"
#include "FVent.h"
#include "FMechVent.h"
#include "FCFAST.h"

FFactory::FFactory(NEManager* m) : NEMultiNodeFactory(m)
{
    m_factories.append(new FEngineFactory(m) );
    m_factories.append(new FZoneFactory(m) );
    m_factories.append(new FZoneCompartmentFactory(m) );
    m_factories.append(new FZoneItemFactory(m) );
    m_factories.append(new FCablesFactory(m) );

    m_factories.append(new FScenarioFactory(m) );
    m_factories.append(new FFireMaterialFactory(m) );
    m_factories.append(new FFireFactory(m) );
    m_factories.append(new FVentFactory(m) );
    m_factories.append(new FMechVentFactory(m) );
    m_factories.append(new FCablesRendererFactory(m) );

    m_factories.append(new FCFASTFactory(m) );


    m_factories.append(new FEngineRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FZoneRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FZoneCompartmentRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FVentRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FMechVentRendererFactory( &NERENDERMAN ) );

    m_factories.append(new FZoneItemRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FFireRendererFactory( &NERENDERMAN ) );
    m_factories.append(new FCablesRendererFactory( &NERENDERMAN ) );

}

FFactory::~FFactory()
{
}


QString FFactory::nodeName() { return "Fire Factory Nodes"; }

uint FFactory::nodeType(){ return NE::kObjectGroup; }

QString FFactory::nodeInherits() { return "Fire";}

QString FFactory::objectName() { return "FFactory"; }

QString FFactory::nodeVersion() { return "0.1"; }

QString FFactory::nodeVendor() { return "F Dynamics"; }

QString FFactory::nodeHelp() { return "Fire Zone Nodes"; }

NEObject* FFactory::createInstance()
{
    return NULL;
}

