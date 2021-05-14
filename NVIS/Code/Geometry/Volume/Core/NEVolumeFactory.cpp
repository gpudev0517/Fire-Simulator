#include "NEVolumeFactory.h"
#include "Spatial/Volume/NEVolumeFilter.h"
#include "Spatial/Volume/NEVolumeCombine.h"
#include "Spatial/Volume/NEVolumeFile.h"

NEVolumeFactory::NEVolumeFactory(NEManager* m) : NEMultiNodeFactory(m)
{
    m_factories.append(new NEVolumeFileFactory(m) );
    m_factories.append(new NEVolumeFilterFactory(m) );
    m_factories.append(new NEVolumeCombineFactory(m) );
}

NEVolumeFactory::~NEVolumeFactory()
{
}


QString NEVolumeFactory::nodeName() { return "Volume Nodes"; }

uint NEVolumeFactory::nodeType(){ return NE::kObjectVolumeOP;}

QString NEVolumeFactory::nodeInherits() { return "Volumes";}

QString NEVolumeFactory::objectName() { return "VolumeFactory"; }

QString NEVolumeFactory::nodeVersion() { return "0.1"; }

QString NEVolumeFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEVolumeFactory::nodeHelp() { return "Volume Nodes"; }

NEObject* NEVolumeFactory::createInstance()
{
    return NULL;
}

