#pragma once

#include "qtvariantproperty.h"
#include "Utility/FilePath.h"
#include "Utility/NEColorRamp.h"

class NECEXP_NEUIPROPERTIES NEVariantManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    NEVariantManager(QObject *parent = 0);
    ~NEVariantManager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    virtual QStringList attributes(int propertyType) const;
    virtual int attributeType(int propertyType, const QString &attribute) const;
    virtual QVariant attributeValue(const QtProperty *property, const QString &attribute) const;
    static int filePathTypeId();
    static int colorRampTypeId();
    static int NEBoolId();
public slots:
    virtual void setValue(QtProperty *property, const QVariant &val);
    virtual void setAttribute(QtProperty *property,
    const QString &attribute, const QVariant &value);

protected:
    virtual QString valueText(const QtProperty *property) const;
    virtual QIcon valueIcon(const QtProperty *property) const;
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);

private slots:
    void slotPropertyDestroyed(QtProperty *property);


private:
    struct FileData
    {
        QString value;
        QString filter;
        FilePath::DialogType type;
    };

    struct ColorRamp
    {
        QVector<QPair<qreal, QColor>> m_ColorList;
    };

    QMap<const QtProperty *, FileData> pathProperties;
    QMap<const QtProperty *, ColorRamp> colorRampProperties;
    QMap<const QtProperty *, uint> uintProperties;
    QMap<const QtProperty *, bool> boolProperties;
};
