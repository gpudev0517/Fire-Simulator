#include "FDYNVariantManager.h"



FDYNVariantManager::FDYNVariantManager(QObject *parent)
    : QtVariantPropertyManager(parent)
{
//    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
//                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

void FDYNVariantManager::slotPropertyDestroyed(QtProperty *property)
{
    if(propertyType(property) == QVariant::UInt && uintProperties.contains(property))
    {
        uintProperties.remove(property);
    }
    else if (propertyType(property) == filePathTypeId() && pathProperties.contains(property))
    {
        pathProperties.remove(property);
    }
    else if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        colorRampProperties.remove(property);
    }
    else if (propertyType(property) == surfaceMaterialTypeId() && surfaceMaterialProperties.contains(property))
    {
        surfaceMaterialProperties.remove(property);
    }
    else if (propertyType(property) == cableMaterialTypeId() && cableMaterialProperties.contains(property))
    {
        cableMaterialProperties.remove(property);
    }
    else if (propertyType(property) == dynamicComboTypeId() && dynamicComboProperties.contains(property))
    {
        dynamicComboProperties.remove(property);
    }
    else if (propertyType(property) == fireMaterialTypeId() && fireMaterialProperties.contains(property))
    {
        fireMaterialProperties.remove(property);
    }
    else if (propertyType(property) == failureCriteriaTypeId() && failureCriteriaProperties.contains(property))
    {
        failureCriteriaProperties.remove(property);
    }
    else if (propertyType(property) == ventCriteriaTypeId() && ventCriteriaProperties.contains(property))
    {
        ventCriteriaProperties.remove(property);
    }
}

FDYNVariantManager::~FDYNVariantManager()
{

}

int FDYNVariantManager::filePathTypeId()
{
    //return qMetaTypeId<FilePath>();
    return getNEVarIds().filePathId;
}

int FDYNVariantManager::colorRampTypeId()
{
    //return qMetaTypeId<ColorRamp>();
    return getNEVarIds().colorRampId;
}

int FDYNVariantManager::NEBoolId()
{
    return getNEVarIds().neBoolId;
}

int FDYNVariantManager::dynamicComboTypeId()
{
    return getFDYNVarIds().dynamicComboId;
}

int FDYNVariantManager::surfaceMaterialTypeId()
{
    return getFDYNVarIds().surfaceMaterialId;
}

int FDYNVariantManager::cableMaterialTypeId()
{
    return getFDYNVarIds().cableMaterialId;
}

int FDYNVariantManager::fireMaterialTypeId()
{
    return getFDYNVarIds().fireMaterialId;
}

int FDYNVariantManager::failureCriteriaTypeId()
{
    return getFDYNVarIds().failureCriteriaId;
}

int FDYNVariantManager::ventCriteriaTypeId()
{
    return getFDYNVarIds().ventCriteriaId;
}
bool FDYNVariantManager::isPropertyTypeSupported(int propertyType) const
{
    if(propertyType == QVariant::UInt)
        return true;
    else if (propertyType == filePathTypeId())
        return true;
    else if (propertyType == colorRampTypeId())
        return true;
    else if (propertyType == surfaceMaterialTypeId())
        return true;
    else if (propertyType == cableMaterialTypeId())
        return true;
    else if (propertyType == dynamicComboTypeId())
        return true;
    else if (propertyType == fireMaterialTypeId())
        return true;
    else if( propertyType == NEBoolId() )
        return true;
    else if( propertyType == failureCriteriaTypeId() )
        return true;
    else if( propertyType == ventCriteriaTypeId() )
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

int FDYNVariantManager::valueType(int propertyType) const
{
    if(propertyType == QVariant::UInt)
        return QVariant::UInt;
    else if (propertyType == filePathTypeId())
        return QVariant::String;
    else if (propertyType == colorRampTypeId())
        return colorRampTypeId();
    else if (propertyType == surfaceMaterialTypeId())
        return surfaceMaterialTypeId();
    else if (propertyType == cableMaterialTypeId())
        return cableMaterialTypeId();
    else if (propertyType == dynamicComboTypeId())
        return dynamicComboTypeId();
    else if (propertyType == fireMaterialTypeId())
        return fireMaterialTypeId();
    else if (propertyType == failureCriteriaTypeId())
        return failureCriteriaTypeId();
    else if (propertyType == ventCriteriaTypeId())
        return ventCriteriaTypeId();
    return QtVariantPropertyManager::valueType(propertyType);
}

QVariant FDYNVariantManager::value(const QtProperty *property) const
{
    if(propertyType(property) == QVariant::UInt && uintProperties.contains(property))
    {
        return uintProperties[property];
    }
    else if (propertyType(property) == filePathTypeId() && pathProperties.contains(property))
    {
        return pathProperties[property].value;
    }
    else if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        NEColorRamp colorRamp;
        colorRamp.setColorRamp(colorRampProperties[property].m_ColorList);
        return colorRamp;
    }
    else if (propertyType(property) == surfaceMaterialTypeId() && surfaceMaterialProperties.contains(property))
    {
        NESurfaceMaterial material;
        material.setName(surfaceMaterialProperties[property].name);
        material.setUniqueID(surfaceMaterialProperties[property].uniqueId);
        material.setIndex(surfaceMaterialProperties[property].index);
        material.setConductivity(surfaceMaterialProperties[property].conductivity);
        material.setHeat(surfaceMaterialProperties[property].heat);
        material.setDensity(surfaceMaterialProperties[property].density);
        material.setThickness(surfaceMaterialProperties[property].thickness);
        material.setEmissivity(surfaceMaterialProperties[property].emissivity);
        return material;
    }
    else if (propertyType(property) == cableMaterialTypeId() && cableMaterialProperties.contains(property))
    {
        NECableProperty material;
        material.setName(cableMaterialProperties[property].name);
        material.setUniqueID(cableMaterialProperties[property].uniqueId);
        material.setIndex(cableMaterialProperties[property].index);

        material.setModel(cableMaterialProperties[property].model);
        material.setInsulationType(cableMaterialProperties[property].insulationType);
        material.setJacketType(cableMaterialProperties[property].jacketType);
        material.setOutsideDiameterMM(cableMaterialProperties[property].outSideDiameterMM);
        material.setOutSideDiameterKgPerMeter(cableMaterialProperties[property].outSideDiameterKgPerMeter);
        material.setJacketThicknessMM(cableMaterialProperties[property].jacketThicknessMM);
        material.setGauges(cableMaterialProperties[property].gauges);
        material.setConductors(cableMaterialProperties[property].conductors);
        material.setDescription(cableMaterialProperties[property].description);
        material.setGrade(cableMaterialProperties[property].grade);
        material.setPart(cableMaterialProperties[property].part);
        material.setItem(cableMaterialProperties[property].item);
        return material;
    }
    else if (propertyType(property) == fireMaterialTypeId() && fireMaterialProperties.contains(property))
    {
        NEFireMaterial material;
        material.setName(fireMaterialProperties[property].name);
        material.setUniqueID(fireMaterialProperties[property].uniqueId);
        material.setIndex(fireMaterialProperties[property].index);
        material.setC(fireMaterialProperties[property].C);
        material.setN(fireMaterialProperties[property].N);
        material.setH(fireMaterialProperties[property].H);
        material.setCI(fireMaterialProperties[property].Cl);
        material.setO(fireMaterialProperties[property].O);
        material.setCombuHeat(fireMaterialProperties[property].combuHeat);
        material.setRadFraction(fireMaterialProperties[property].radFraction);
        material.setInfos(fireMaterialProperties[property].infos);
        return material;
    }
    else if (propertyType(property) == dynamicComboTypeId() && dynamicComboProperties.contains(property))
    {
        NEDynamicCombo dycombo(dynamicComboProperties[property].nameList, dynamicComboProperties[property].index);
        return dycombo;
    }
    else if (propertyType(property) == failureCriteriaTypeId() && failureCriteriaProperties.contains(property))
    {
        NEFailureCriteria fail;
        fail.setName(failureCriteriaProperties[property].name);
        fail.setID(failureCriteriaProperties[property].uniqueId);
        fail.setIndex(failureCriteriaProperties[property].index);
        fail.setType(failureCriteriaProperties[property].type);
        fail.setThreshold(failureCriteriaProperties[property].threshold);
        return fail;
    }
    else if (propertyType(property) == ventCriteriaTypeId() && ventCriteriaProperties.contains(property))
    {
        NEVentCriteria vent;
        vent.setName(ventCriteriaProperties[property].name);
        vent.setID(ventCriteriaProperties[property].uniqueId);
        vent.setIndex(ventCriteriaProperties[property].index);
        vent.setType(ventCriteriaProperties[property].type);
        vent.setValues(ventCriteriaProperties[property].values);
        return vent;
    }

    return QtVariantPropertyManager::value(property);
}

QStringList FDYNVariantManager::attributes(int propertyType) const
{
    if (propertyType == filePathTypeId()) {
        QStringList attr;
        attr << QLatin1String("filter") << QLatin1String("type");
        return attr;
    }
    if (propertyType == colorRampTypeId()) {
        QStringList attr;
        attr << QLatin1String("ColorRamp");
        return attr;
    }
    if (propertyType == surfaceMaterialTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("SurfaceMaterialData");
        return attr;
    }
    if (propertyType == cableMaterialTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("CableMaterialData");
        return attr;
    }
    if (propertyType == dynamicComboTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("DynamicComboData");
        return attr;
    }
    if (propertyType == fireMaterialTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("FireMaterialData");
        return attr;
    }
    if (propertyType == failureCriteriaTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("FailureCriteriaData");
        return attr;
    }
    if (propertyType == ventCriteriaTypeId()) {
        // To da change.
        QStringList attr;
        attr << QLatin1String("VentCriteriaData");
        return attr;
    }
    return QtVariantPropertyManager::attributes(propertyType);
}

int FDYNVariantManager::attributeType(int propertyType, const QString &attribute) const
{
    if (propertyType == filePathTypeId()) {
        if (attribute == QLatin1String("filter") || attribute == QLatin1String("type"))
            return QVariant::String;
        return 0;
    }

    if (propertyType == colorRampTypeId()) {
        return colorRampTypeId();
    }
    if(propertyType == surfaceMaterialTypeId()){
        return surfaceMaterialTypeId();
    }
    if(propertyType == cableMaterialTypeId()){
        return cableMaterialTypeId();
    }
    if(propertyType == dynamicComboTypeId()){
        return dynamicComboTypeId();
    }
    if(propertyType == fireMaterialTypeId()){
        return fireMaterialTypeId();
    }
    if(propertyType == failureCriteriaTypeId()){
        return failureCriteriaTypeId();
    }
    if(propertyType == ventCriteriaTypeId()){
        return ventCriteriaTypeId();
    }

    return QtVariantPropertyManager::attributeType(propertyType, attribute);
}

QVariant FDYNVariantManager::attributeValue(const QtProperty *property, const QString &attribute) const
{
    if( pathProperties.contains(property) ) {
        if (attribute == QLatin1String("filter"))
            return pathProperties[property].filter;
		else if(attribute == QLatin1String("type"))
            return pathProperties[property].type;
        return QVariant();
    }
    else if( colorRampProperties.contains(property) )
    {
        NEColorRamp colorRamp;
        colorRamp.setColorRamp(colorRampProperties[property].m_ColorList);
        return colorRamp;
    }
    else if( surfaceMaterialProperties.contains(property) ) {
        NESurfaceMaterial material;
        material.setName(surfaceMaterialProperties[property].name);
        material.setUniqueID(surfaceMaterialProperties[property].uniqueId);
        material.setIndex(surfaceMaterialProperties[property].index);
        material.setConductivity(surfaceMaterialProperties[property].conductivity);
        material.setHeat(surfaceMaterialProperties[property].heat);
        material.setDensity(surfaceMaterialProperties[property].density);
        material.setThickness(surfaceMaterialProperties[property].thickness);
        material.setEmissivity(surfaceMaterialProperties[property].emissivity);
        return material;
    }
    else if( cableMaterialProperties.contains(property) ) {
        NECableProperty material;
        material.setName(cableMaterialProperties[property].name);
        material.setUniqueID(cableMaterialProperties[property].uniqueId);
        material.setIndex(cableMaterialProperties[property].index);

        material.setModel(cableMaterialProperties[property].model);
        material.setInsulationType(cableMaterialProperties[property].insulationType);
        material.setJacketType(cableMaterialProperties[property].jacketType);
        material.setOutsideDiameterMM(cableMaterialProperties[property].outSideDiameterMM);
        material.setOutSideDiameterKgPerMeter(cableMaterialProperties[property].outSideDiameterKgPerMeter);
        material.setJacketThicknessMM(cableMaterialProperties[property].jacketThicknessMM);
        material.setGauges(cableMaterialProperties[property].gauges);
        material.setConductors(cableMaterialProperties[property].conductors);
        material.setDescription(cableMaterialProperties[property].description);
        material.setGrade(cableMaterialProperties[property].grade);
        material.setPart(cableMaterialProperties[property].part);
        material.setItem(cableMaterialProperties[property].item);
        return material;
    }
    else if( dynamicComboProperties.contains(property) )
    {
        NEDynamicCombo dycombo(dynamicComboProperties[property].nameList, dynamicComboProperties[property].index);
        return dycombo;
    }
    else if( fireMaterialProperties.contains(property) ) {
        NEFireMaterial material;
        material.setName(fireMaterialProperties[property].name);
        material.setUniqueID(fireMaterialProperties[property].uniqueId);
        material.setIndex(fireMaterialProperties[property].index);
        material.setC(fireMaterialProperties[property].C);
        material.setN(fireMaterialProperties[property].N);
        material.setH(fireMaterialProperties[property].H);
        material.setCI(fireMaterialProperties[property].Cl);
        material.setO(fireMaterialProperties[property].O);
        material.setCombuHeat(fireMaterialProperties[property].combuHeat);
        material.setRadFraction(fireMaterialProperties[property].radFraction);
        material.setInfos(fireMaterialProperties[property].infos);
        return material;
    }
    else if( failureCriteriaProperties.contains(property) ) {
        NEFailureCriteria fail;
        fail.setName(failureCriteriaProperties[property].name);
        fail.setID(failureCriteriaProperties[property].uniqueId);
        fail.setIndex(failureCriteriaProperties[property].index);
        fail.setType(failureCriteriaProperties[property].type);
        fail.setThreshold(failureCriteriaProperties[property].threshold);
        return fail;
    }
    else if( ventCriteriaProperties.contains(property) ) {
        NEVentCriteria vent;
        vent.setName(ventCriteriaProperties[property].name);
        vent.setID(ventCriteriaProperties[property].uniqueId);
        vent.setIndex(ventCriteriaProperties[property].index);
        vent.setType(ventCriteriaProperties[property].type);
        vent.setValues(ventCriteriaProperties[property].values);
        return vent;
    }
    else if( boolProperties.contains(property) )
    {
        if( attribute == QLatin1String("button") )
        {
            return boolProperties[property];
        }
    }
    return QtVariantPropertyManager::attributeValue(property, attribute);
}

QString FDYNVariantManager::valueText(const QtProperty *property) const
{
    if (propertyType(property) == QVariant::UInt && uintProperties.contains(property))
    {
        return QString("%1").arg(QString::number(uintProperties[property]));
    }
    else if (propertyType(property) == filePathTypeId() && pathProperties.contains(property))
    {
        return pathProperties[property].value;
    }
    else if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        return QString();
    }
    else if (propertyType(property) == surfaceMaterialTypeId() && surfaceMaterialProperties.contains(property))
    {
        return surfaceMaterialProperties[property].name;
    }
    else if (propertyType(property) == cableMaterialTypeId() && cableMaterialProperties.contains(property))
    {
        return cableMaterialProperties[property].name;
    }
    else if (propertyType(property) == dynamicComboTypeId() && dynamicComboProperties.contains(property))
    {
        return dynamicComboProperties[property].curName();
    }
    else if (propertyType(property) == fireMaterialTypeId() && fireMaterialProperties.contains(property))
    {
        return fireMaterialProperties[property].name;
    }
    else if (propertyType(property) == failureCriteriaTypeId() && failureCriteriaProperties.contains(property))
    {
        return failureCriteriaProperties[property].name;
    }
    else if (propertyType(property) == ventCriteriaTypeId() && ventCriteriaProperties.contains(property))
    {
        return ventCriteriaProperties[property].name;
    }

    return QtVariantPropertyManager::valueText(property);
}

static QIcon drawColorRampIcon()
{
    QPixmap pixmap = QPixmap(50, 30);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);

    QLinearGradient grad;
    QRect crec(0, 0, 50, 30);
    grad = QLinearGradient(0, 0, 50, 0);

    grad.setColorAt(0, QColor(Qt::blue));
    grad.setColorAt(0.5, QColor(Qt::green));
    grad.setColorAt(1, QColor(Qt::red));

    painter.fillRect(crec, grad);
    painter.drawRect(crec);
    painter.end();

    return QIcon(pixmap);
}

QIcon FDYNVariantManager::valueIcon(const QtProperty *property) const
{
    if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        static const QIcon colorRampIcon = drawColorRampIcon();
        return colorRampIcon;
    }

    return QtVariantPropertyManager::valueIcon(property);
}

void FDYNVariantManager::setValue(QtProperty *property, const QVariant &val)
{
    // Check whether the property is the uint type and the map contains the property
    if (propertyType(property) == QVariant::UInt && uintProperties.contains(property))
    {
        // Early return if the incoming variant's type is not compatible with this type
        if(val.type() != QVariant::UInt && !val.canConvert(QVariant::UInt))
            return;
        // Convert the variant to uint
        uint uintVal = val.value<uint>();
        // Check if an old value exists in the map
        uint oldVal = uintProperties[property];
        // If old and new values are equivalent, return early
        if(oldVal == uintVal)
            return;
        // Otherwise, assign the new value of the property
        uintProperties[property] = uintVal;
        // Emit the propertyChanged signal
        emit propertyChanged(property);
        // Emit the value changed signal
        emit valueChanged(property, uintVal);
        return;
    }
    else if (propertyType(property) == filePathTypeId() && pathProperties.contains(property))
    {
        if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
            return;
        QString str = val.value<QString>();
        FileData d = pathProperties[property];
        if (d.value == str)
            return;
        d.value = str;
        pathProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, str);
        return;
    }
    else if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        NEColorRamp colorRamp = val.value<NEColorRamp>();
        ColorRamp d = colorRampProperties[property];
        if (d.m_ColorList == colorRamp.getColorRamp())
            return;
        d.m_ColorList = colorRamp.getColorRamp();
        colorRampProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, colorRamp);
        return;
    }
    else if (propertyType(property) == surfaceMaterialTypeId() && surfaceMaterialProperties.contains(property))
    {
        NESurfaceMaterial material = val.value<NESurfaceMaterial>();
        SurfaceMaterialData d = surfaceMaterialProperties[property];

        if (d.uniqueId == 0 && d.name == "" && material.uniqueID() == 0 && material.name() == "")
        {
            emit propertyChanged(property);
            return;
        }
        else if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index() && d.conductivity == material.conductivity()
                && d.heat == material.heat() && d.density == material.density() && d.thickness == material.thickness() && d.emissivity == material.emissivity()){
            return;
        }

        d.name = material.name();
        d.uniqueId = material.uniqueID();
        d.index = material.index();
        d.conductivity = material.conductivity();
        d.heat = material.heat();
        d.density = material.density();
        d.thickness = material.thickness();
        d.emissivity = material.emissivity();

        surfaceMaterialProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, material);

        return;
    }
    else if (propertyType(property) == cableMaterialTypeId() && cableMaterialProperties.contains(property))
    {
        NECableProperty material = val.value<NECableProperty>();
        CableMaterialData d = cableMaterialProperties[property];

        if (d.uniqueId == 0 && d.name == "" && material.uniqueID() == 0 && material.name() == "")
        {
            emit propertyChanged(property);
            return;
        }
        else if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index() && d.model == material.model() && d.insulationType == material.insulationType()
                    && d.jacketType == material.jacketType() && d.outSideDiameterMM == material.outSideDiameterMM() && d.outSideDiameterKgPerMeter == material.outSideDiameterKgPerMeter()
                    && d.jacketThicknessMM == material.jacketThicknesMM() && d.gauges == material.gauges() && d.conductors == material.conductors() && d.description == material.description()
                    && d.grade == material.grade() && d.part == material.part() && d.item == material.item())
                return;

            d.name = material.name();
            d.uniqueId = material.uniqueID();
            d.index = material.index();

            d.model = material.model();
            d.insulationType = material.insulationType();
            d.jacketType = material.jacketType();
            d.outSideDiameterMM = material.outSideDiameterMM();
            d.outSideDiameterKgPerMeter = material.outSideDiameterKgPerMeter();
            d.jacketThicknessMM = material.jacketThicknesMM();
            d.gauges = material.gauges();
            d.conductors = material.conductors();
            d.description= material.description();
            d.grade = material.grade();
            d.part= material.part();
            d.item = material.item();


        cableMaterialProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, material);

        return;
    }
    else if (propertyType(property) == dynamicComboTypeId() && dynamicComboProperties.contains(property))
    {
        NEDynamicCombo dycombo = val.value<NEDynamicCombo>();
        DynamicComboData d = dynamicComboProperties[property];

        if (d.nameList == QStringList() && dycombo.nameList() == QStringList())
        {
            emit propertyChanged(property);
            return;
        }
        else if (d.nameList == dycombo.nameList() && d.index == dycombo.curIndex())
            return;

        d.nameList = dycombo.nameList();
        d.index = dycombo.curIndex();

        dynamicComboProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, dycombo);

        return;
    }
    else if (propertyType(property) == fireMaterialTypeId() && fireMaterialProperties.contains(property))
    {
        NEFireMaterial material = val.value<NEFireMaterial>();
        FireMaterialData d = fireMaterialProperties[property];

        if (d.uniqueId == 0 && d.name == "" && material.uniqueID() == 0 && material.name() == "")
        {
            emit propertyChanged(property);
            return;
        }
        else if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index()
                && d.combuHeat == material.combuHeat() && d.radFraction == material.radFraction()
                && d.C == material.C() && d.N == material.N() && d.H == material.H()
                && d.Cl == material.CI() && d.O == material.O() && d.infos == material.Infos())
            return;

        d.name = material.name();
        d.uniqueId = material.uniqueID();
        d.index = material.index();
        d.C = material.C();
        d.N = material.N();
        d.H = material.H();
        d.Cl = material.CI();
        d.O = material.O();
        d.combuHeat = material.combuHeat();
        d.radFraction = material.radFraction();
        d.infos = material.Infos();

        fireMaterialProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, material);
        return;
    }
    else if (propertyType(property) == failureCriteriaTypeId() && failureCriteriaProperties.contains(property))
    {
        NEFailureCriteria material = val.value<NEFailureCriteria>();
        FailureCriteriaData d = failureCriteriaProperties[property];

        if (d.uniqueId == 0 && d.name == "" && material.ID() == 0 && material.name() == "")
        {
            emit propertyChanged(property);
            return;
        }
        else if(d.name == material.name() && d.uniqueId == material.ID() && d.index == material.index() && d.type == material.type() && d.threshold == material.threshold() )
            return;

        d.name = material.name();
        d.uniqueId = material.ID();
        d.index = material.index();
        d.type = material.type();
        d.threshold = material.threshold();

        failureCriteriaProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, material);
    }
    else if (propertyType(property) == ventCriteriaTypeId() && ventCriteriaProperties.contains(property))
    {
        NEVentCriteria vent = val.value<NEVentCriteria>();
        VentCriteriaData d = ventCriteriaProperties[property];

        if (d.uniqueId == 0 && d.name == "" && vent.ID() == 0 && vent.name() == "")
        {
            emit propertyChanged(property);
            return;
        }
        else if(d.name == vent.name() && d.uniqueId == vent.ID() && d.index == vent.index() && d.type == vent.type() && d.values == vent.values() )
            return;

        d.name = vent.name();
        d.uniqueId = vent.ID();
        d.index = vent.index();
        d.type = vent.type();
        d.values = vent.values();

        ventCriteriaProperties[property] = d;
        emit propertyChanged(property);
        emit valueChanged(property, vent);
    }

    QtVariantPropertyManager::setValue(property, val);
}

void FDYNVariantManager::setAttribute(QtProperty *property,
                const QString &attribute, const QVariant &val)
{
    if (pathProperties.contains(property)) {
        if (attribute == QLatin1String("filter")) 
		{
            if (val.type() != QVariant::String && !val.canConvert(QVariant::String))
                return;
            QString str = val.value<QString>();
            FileData d = pathProperties[property];
            if (d.filter == str)
                return;
            d.filter = str;
            pathProperties[property] = d;
            emit attributeChanged(property, attribute, str);
        }
		else if (attribute == QLatin1String("type"))
		{
			if (val.type() != QVariant::Int && !val.canConvert(QVariant::Int))
				return;
			int str = val.value<int>();
            FileData d = pathProperties[property];
			if (d.type == str)
				return;
			d.type = (FilePath::DialogType)str;
            pathProperties[property] = d;
			emit attributeChanged(property, attribute, str);
		}
        return;
    }
    else if (colorRampProperties.contains(property)) {
        NEColorRamp colorRamp = val.value<NEColorRamp>();
        ColorRamp d = colorRampProperties[property];
        if (d.m_ColorList == colorRamp.getColorRamp())
            return;
        d.m_ColorList == colorRamp.getColorRamp();
        colorRampProperties[property] = d;
        emit attributeChanged(property, attribute, colorRamp);

        return;
    }
    else if(dynamicComboProperties.contains(property)){
        NEDynamicCombo dycombo = val.value<NEDynamicCombo>();
        DynamicComboData d = dynamicComboProperties[property];

        if (d.nameList == dycombo.nameList() && d.index == dycombo.curIndex())
            return;

        d.nameList = dycombo.nameList();
        d.index = dycombo.curIndex();

        dynamicComboProperties[property] = d;
        emit attributeChanged(property, attribute, dycombo);
        return;
    }
    else if(surfaceMaterialProperties.contains(property)){
        NESurfaceMaterial material = val.value<NESurfaceMaterial>();
        SurfaceMaterialData d = surfaceMaterialProperties[property];

        if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index() && d.conductivity == material.conductivity()
           && d.heat == material.heat() && d.density == material.density() && d.thickness == material.thickness() && d.emissivity == material.emissivity())
            return;

        d.name = material.name();
        d.uniqueId = material.uniqueID();
        d.index = material.index();
        d.conductivity = material.conductivity();
        d.heat = material.heat();
        d.density = material.density();
        d.thickness = material.thickness();
        d.emissivity = material.emissivity();

        surfaceMaterialProperties[property] = d;
        emit attributeChanged(property, attribute, material);
        return;
    }
    else if(cableMaterialProperties.contains(property)){
        NECableProperty material = val.value<NECableProperty>();
        CableMaterialData d = cableMaterialProperties[property];

        if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index() && d.model == material.model() && d.insulationType == material.insulationType()
                && d.jacketType == material.jacketType() && d.outSideDiameterMM == material.outSideDiameterMM() && d.outSideDiameterKgPerMeter == material.outSideDiameterKgPerMeter()
                && d.jacketThicknessMM == material.jacketThicknesMM() && d.gauges == material.gauges() && d.conductors == material.conductors() && d.description == material.description()
                && d.grade == material.grade() && d.part == material.part() && d.item == material.item())
            return;

        d.name = material.name();
        d.uniqueId = material.uniqueID();
        d.index = material.index();

        d.model = material.model();
        d.insulationType = material.insulationType();
        d.jacketType = material.jacketType();
        d.outSideDiameterMM = material.outSideDiameterMM();
        d.outSideDiameterKgPerMeter = material.outSideDiameterKgPerMeter();
        d.jacketThicknessMM = material.jacketThicknesMM();
        d.gauges = material.gauges();
        d.conductors = material.conductors();
        d.description= material.description();
        d.grade = material.grade();
        d.part= material.part();
        d.item = material.item();


        cableMaterialProperties[property] = d;
        emit attributeChanged(property, attribute, material);
        return;
    }
    else if(fireMaterialProperties.contains(property)){
        NEFireMaterial material = val.value<NEFireMaterial>();
        FireMaterialData d = fireMaterialProperties[property];

        if(d.name == material.name() && d.uniqueId == material.uniqueID() && d.index == material.index()
           && d.C == material.C() && d.N == material.N() && d.H == material.H() && d.Cl == material.CI()
           && d.combuHeat == material.combuHeat() && d.radFraction == material.radFraction())
            return;

        d.name = material.name();
        d.uniqueId = material.uniqueID();
        d.index = material.index();
        d.C = material.C();
        d.N = material.N();
        d.H = material.H();
        d.Cl = material.CI();
        d.O = material.O();
        d.combuHeat = material.combuHeat();
        d.radFraction = material.radFraction();
        d.infos = material.Infos();

        fireMaterialProperties[property] = d;
        emit attributeChanged(property, attribute, material);
        return;
    }
    else if(failureCriteriaProperties.contains(property)){
        NEFailureCriteria material = val.value<NEFailureCriteria>();
        FailureCriteriaData d = failureCriteriaProperties[property];

        if(d.name == material.name() && d.uniqueId == material.ID() && d.index == material.index() && d.type == material.type() && d.threshold == material.threshold() )
            return;

        d.name = material.name();
        d.uniqueId = material.ID();
        d.index = material.index();
        d.type = material.type();
        d.threshold = material.threshold();
    }
    else if(ventCriteriaProperties.contains(property)){
        NEVentCriteria vent = val.value<NEVentCriteria>();
        VentCriteriaData d = ventCriteriaProperties[property];

        if(d.name == vent.name() && d.uniqueId == vent.ID() && d.index == vent.index() && d.type == vent.type() && d.values == vent.values() )
            return;

        d.name = vent.name();
        d.uniqueId = vent.ID();
        d.index = vent.index();
        d.type = vent.type();
        d.values = vent.values();
    }
    else if(boolProperties.contains(property))
    {

    }
    QtVariantPropertyManager::setAttribute(property, attribute, val);
}

void FDYNVariantManager::initializeProperty(QtProperty *property)
{
    // Initialize the property
    if(propertyType(property) == QVariant::UInt)
        uintProperties[property] = 0;

    else if (propertyType(property) == filePathTypeId())
        pathProperties[property] = FileData();
    else if (propertyType(property) == colorRampTypeId())
        colorRampProperties[property] = ColorRamp();
    else if( propertyType(property) == dynamicComboTypeId())
        dynamicComboProperties[property] = DynamicComboData();
    else if( propertyType(property) == surfaceMaterialTypeId())
        surfaceMaterialProperties[property] = SurfaceMaterialData();
    else if( propertyType(property) == cableMaterialTypeId())
        cableMaterialProperties[property] = CableMaterialData();
    else if( propertyType(property) == fireMaterialTypeId())
        fireMaterialProperties[property] = FireMaterialData();
    else if( propertyType(property) == failureCriteriaTypeId())
        failureCriteriaProperties[property] = FailureCriteriaData();
    else if( propertyType(property) == ventCriteriaTypeId())
        ventCriteriaProperties[property] = VentCriteriaData();
    else if (propertyType(property) == QVariant::Bool)
        boolProperties[property] = false;
    QtVariantPropertyManager::initializeProperty(property);
}

void FDYNVariantManager::uninitializeProperty(QtProperty *property)
{
    if(propertyType(property) == QVariant::UInt)
    {
        uintProperties.remove(property);
    }
    else if(propertyType(property) == filePathTypeId())
    {
        pathProperties.remove(property);
    }
    else if(propertyType(property) == colorRampTypeId())
    {
        colorRampProperties.remove(property);
    }
    else if(propertyType(property) == dynamicComboTypeId())
    {
        dynamicComboProperties.remove(property);
    }
    else if(propertyType(property) == surfaceMaterialTypeId())
    {
        surfaceMaterialProperties.remove(property);
    }
    else if(propertyType(property) == cableMaterialTypeId())
    {
        cableMaterialProperties.remove(property);
    }
    else if(propertyType(property) == fireMaterialTypeId())
    {
        fireMaterialProperties.remove(property);
    }
    else if(propertyType(property) == failureCriteriaTypeId())
    {
        failureCriteriaProperties.remove(property);
    }
    else if(propertyType(property) == ventCriteriaTypeId())
    {
        ventCriteriaProperties.remove(property);
    }
    else if( propertyType(property) == QVariant::Bool)
    {
        boolProperties.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}
