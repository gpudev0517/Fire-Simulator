#include "NESpatialEngine.h"





///
/// \brief NESpatialEngine::NESpatialEngine
/// \param factory
///

NESpatialEngine::NESpatialEngine(NENodeFactory* factory) : NESpatialGroup(factory)
{

}

///
/// \brief FEngine::~FEngine
///

NESpatialEngine::~NESpatialEngine()
{
    ;
}

void NESpatialEngine::nodeRenamed( NENode* node, QString name )
{

}

///
/// \brief FEngine::setActiveZone
/// \param zoneName
///


void NESpatialEngine::setActiveZone(QString zoneName)
{
    return;
}

void NESpatialEngine::setActiveScenario(QString scenarioName)
{
    return;
}

QList<QString> NESpatialEngine::zones()
{
    return QList<QString>();
}


QList<QString> NESpatialEngine::scenarios()
{
    return QList<QString>();
}

int NESpatialEngine::zoneCreated(NEZone *newZone)
{
    return 0;
}

int NESpatialEngine::zoneDeleted(NEZone *zone)
{
    return 0;
}

int NESpatialEngine::scenarioCreated(NEScenario *newScn)
{
    return 0;
}

int NESpatialEngine::scenarioDeleted(NEScenario *scn)
{
    return 0;
}


int NESpatialEngine::itemCreated(NEZoneItem *newZoneItem, NENode *parent)
{
    return 0;
}

int NESpatialEngine::itemDeleted(NEZoneItem *newZoneItem, bool force)
{
    return 0;
}

void NESpatialEngine::duplicateObject(NEZoneItem *item)
{
    return;
}

void NESpatialEngine::copyProperties(NEZoneItem *item)
{
    return;
}

void NESpatialEngine::pasteProperties(QList<NEZoneItem *> items, QString filter )
{
    return;
}

int NESpatialEngine::addCompartment(NENode *node, NENode *parent)
{
    return 0;
}




void NESpatialEngine::addItemToScenario(NEZoneItem *item, NENode *parent)
{
    return;
}

void NESpatialEngine::removeItemFromScenario(NEZoneItem *zoneItem, NENode *parent)
{
    return;
}

void NESpatialEngine::removeItemFromScenarios(NEZoneItem *zoneItem, QList<NENode *>parents)
{
    return;
}


QString NESpatialEngine::activeZoneName()
{
    return QString();
}


QString NESpatialEngine::activeScenarioName()
{
    return QString();
}

void NESpatialEngine::clear()
{
    return;
}

void NESpatialEngine::newData(QString filename)
{
    return;
}

void NESpatialEngine::load(QString filename)
{
    return;
}

void NESpatialEngine::save(QString filename)
{
    return;
}
void NESpatialEngine::quit()
{

}

void NESpatialEngine::saveZone(QString zonename, QString filename)
{
    return;
}

void NESpatialEngine::saveScenario(QString scenarioname, QString filename)
{
    return;
}


uint NESpatialEngine::compute()
{
    return NE::kReturnSuccess;
}



void NESpatialEngine::computeOptions()
{
    return ;
}


QString NESpatialEngine::nodeDescription()
{
    return factory().nodeName();
}

QList<NESurfaceMaterial> NESpatialEngine::getSurfaceMaterials()
{
    QList<NESurfaceMaterial> smats;
    return smats;
}

void NESpatialEngine::addSurfaceMaterial(NESurfaceMaterial sm)
{

}

void NESpatialEngine::removeSurfaceMaterial(NESurfaceMaterial sm)
{

}

void NESpatialEngine::assignSurfaceMaterial( NENode *zoneitem, NESurfaceMaterial sm, QString matKind )
{

}


QList<NEFireMaterial> NESpatialEngine::getFireMaterials()
{
    QList<NEFireMaterial> fmats;
    return fmats;
}

void NESpatialEngine::addFireMaterial(NEFireMaterial fm)
{

}

void NESpatialEngine::removeFireMaterial(NEFireMaterial fm)
{

}

void NESpatialEngine::assignFireMaterial(NENode *zoneitem, NEFireMaterial sm)
{

}

QList<NEFailureCriteria> NESpatialEngine::getFailureCriteria()
{
    QList<NEFailureCriteria> fails;

    return fails;
}

void NESpatialEngine::removeFailureCriteria(NEFailureCriteria fail)
{

}

void NESpatialEngine::addFailureCriteria(NEFailureCriteria fail)
{

}

void NESpatialEngine::assignFailureCriteria(NENode* node, NEFailureCriteria fail)
{

}

QList<NEVentCriteria> NESpatialEngine::getVentCriteria(uint zoneId)
{
    return QList<NEVentCriteria>();
}

void NESpatialEngine::removeVentCriteria(NEVentCriteria criterion)
{

}

void NESpatialEngine::addVentCriteria(NEVentCriteria criterion)
{

}

void NESpatialEngine::assignVentCriteria(NENode* node, NEVentCriteria criterion, QString criterionKind)
{

}

QList<NECableProperty> NESpatialEngine::getCableProperty()
{
    return QList<NECableProperty>();
}
void NESpatialEngine::addCableProperty(NECableProperty cable)
{

}
void NESpatialEngine::removeCableProperty(NECableProperty cable)
{

}
void NESpatialEngine::assignCableProperty(NENode* node, NECableProperty cable)
{

}


void NESpatialEngine::changeColorOfZoneItem(double elapsedTime)
{

}

void NESpatialEngine::simulateScenario(QString scnName, QString desc, QString simNodeName)
{

}

void NESpatialEngine::updateWorkingFolder(QString folderName)
{

}


