#include <QJsonObject>

#include "FZoneCompartment.h"
#include "Mesh/NESpatialMesh.h"

#include "Base/NEBase.h"
#include "Base/NESceneManager.h"

#include "FSubZone.h"



FSubZone::FSubZone(NENodeFactory* factory) : FZone(factory)
{
}


FSubZone::~FSubZone()
{
}



QString FSubZone::categoryType()
{
    return "Sub Compartments";
}



FSubZoneFactory::FSubZoneFactory(NEManager* m) : NENodeFactory(m)
{

}

FSubZoneFactory::~FSubZoneFactory()
{

}

QString FSubZoneFactory::nodeName()
{
    return "Fire Sub Zone";
}

QStringList FSubZoneFactory::nodeAltNames()
{
    QStringList aNames;
    aNames.append("FSubZone");

    return aNames;
}

uint FSubZoneFactory::nodeType()
{
    return NE::kObjectGroup;
}

QString FSubZoneFactory::nodeInherits()
{
    return "Geometry";
}

QString FSubZoneFactory::objectName()
{
    return "SubBoundary";
}

QString FSubZoneFactory::nodeVersion()
{
    return "0.1";
}

QString FSubZoneFactory::nodeVendor()
{
    return "FDYN";
}

QString FSubZoneFactory::nodeHelp()
{
    return "Describing a Fire Zone Boundary";
}

NEObject* FSubZoneFactory::createInstance()
{
    return new FSubZone(this);
}


