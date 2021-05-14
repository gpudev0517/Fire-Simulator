#include "NEVariantManager.h"


NEVariantManager::NEVariantManager(QObject *parent)
    : QtVariantPropertyManager(parent)
{
//    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
//                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

void NEVariantManager::slotPropertyDestroyed(QtProperty *property)
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
}

NEVariantManager::~NEVariantManager()
{

}

int NEVariantManager::filePathTypeId()
{
    //return qMetaTypeId<FilePath>();
    return getNEVarIds().filePathId;
}

int NEVariantManager::colorRampTypeId()
{
    //return qMetaTypeId<ColorRamp>();
    return getNEVarIds().colorRampId;
}

int NEVariantManager::NEBoolId()
{
    return getNEVarIds().neBoolId;
}

bool NEVariantManager::isPropertyTypeSupported(int propertyType) const
{
    if(propertyType == QVariant::UInt)
        return true;
    else if (propertyType == filePathTypeId())
        return true;
    else if (propertyType == colorRampTypeId())
        return true;
    else if( propertyType == NEBoolId() )
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

int NEVariantManager::valueType(int propertyType) const
{
    if(propertyType == QVariant::UInt)
        return QVariant::UInt;
    else if (propertyType == filePathTypeId())
        return QVariant::String;
    else if (propertyType == colorRampTypeId())
        return colorRampTypeId();
    return QtVariantPropertyManager::valueType(propertyType);
}

QVariant NEVariantManager::value(const QtProperty *property) const
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

    return QtVariantPropertyManager::value(property);
}

QStringList NEVariantManager::attributes(int propertyType) const
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
    return QtVariantPropertyManager::attributes(propertyType);
}

int NEVariantManager::attributeType(int propertyType, const QString &attribute) const
{
    if (propertyType == filePathTypeId()) {
        if (attribute == QLatin1String("filter") || attribute == QLatin1String("type"))
            return QVariant::String;
        return 0;
    }

    if (propertyType == colorRampTypeId()) {
        return colorRampTypeId();
    }

    return QtVariantPropertyManager::attributeType(propertyType, attribute);
}

QVariant NEVariantManager::attributeValue(const QtProperty *property, const QString &attribute) const
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
    else if( boolProperties.contains(property) )
    {
        if( attribute == QLatin1String("button") )
        {
            return boolProperties[property];
        }
    }
    return QtVariantPropertyManager::attributeValue(property, attribute);
}

QString NEVariantManager::valueText(const QtProperty *property) const
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

QIcon NEVariantManager::valueIcon(const QtProperty *property) const
{
    if (propertyType(property) == colorRampTypeId() && colorRampProperties.contains(property))
    {
        static const QIcon colorRampIcon = drawColorRampIcon();
        return colorRampIcon;
    }

    return QtVariantPropertyManager::valueIcon(property);
}

void NEVariantManager::setValue(QtProperty *property, const QVariant &val)
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

    QtVariantPropertyManager::setValue(property, val);
}

void NEVariantManager::setAttribute(QtProperty *property,
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
    else if(boolProperties.contains(property))
    {

    }
    QtVariantPropertyManager::setAttribute(property, attribute, val);
}

void NEVariantManager::initializeProperty(QtProperty *property)
{
    // Initialize the property
    if(propertyType(property) == QVariant::UInt)
        uintProperties[property] = 0;

    else if (propertyType(property) == filePathTypeId())
        pathProperties[property] = FileData();

    else if (propertyType(property) == colorRampTypeId())
        colorRampProperties[property] = ColorRamp();

    else if (propertyType(property) == QVariant::Bool)
        boolProperties[property] = false;

    QtVariantPropertyManager::initializeProperty(property);
}

void NEVariantManager::uninitializeProperty(QtProperty *property)
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
    else if( propertyType(property) == QVariant::Bool)
    {
        boolProperties.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}

