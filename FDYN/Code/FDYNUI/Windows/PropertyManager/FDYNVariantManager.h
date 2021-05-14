#pragma once

#include "Windows/PropertyManager/qtvariantproperty.h"
#include "Utility/FilePath.h"
#include "Utility/NECustomVariants.h"
#include "Utility/NESurfaceMaterial.h"
#include "Utility/NEFireMaterial.h"
#include "Utility/NEFailureCriteria.h"
#include "Utility/NEVentCriteria.h"
#include "Utility/NECableProperty.h"
#include "Utility/NEColorRamp.h"

class FDYNVariantManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    FDYNVariantManager(QObject *parent = 0);
    ~FDYNVariantManager();

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    virtual QStringList attributes(int propertyType) const;
    virtual int attributeType(int propertyType, const QString &attribute) const;
    virtual QVariant attributeValue(const QtProperty *property, const QString &attribute) const;
    static int filePathTypeId();
    static int colorRampTypeId();
    static int dynamicComboTypeId();
    static int surfaceMaterialTypeId();
    static int fireMaterialTypeId();
    static int failureCriteriaTypeId();
    static int ventCriteriaTypeId();
    static int cableMaterialTypeId();

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

    struct SurfaceMaterialData
    {
        QString name;
        int uniqueId;
        int index;
        double conductivity;
        double heat;
        double density;
        double thickness;
        double emissivity;
    };

    struct FireMaterialData
    {
        QString name;
        int uniqueId;
        int index;
        double C;
        double N;
        double H;
        double Cl;
        double O;
        double combuHeat;
        double radFraction;
        QList<FireMaterialInfo> infos;
    };

    struct FailureCriteriaData
    {
        QString name;
        int uniqueId;
        int index;
        NEFailureCriteria::CriteriaType type;
        double threshold;
    };

    struct VentCriteriaData
    {
        QString name;
        int uniqueId;
        int index;
        NEVentCriteria::CriteriaType type;
        QList<NEVentCriteria::VentValues> values;
    };

    struct CableMaterialData
    {
        QString name;
        int uniqueId; //  Id assigned by the database
        int index; // Index in m_MaterialList

        QString model;
        int insulationType;
        int jacketType;
        float outSideDiameterMM;
        float outSideDiameterKgPerMeter;
        float jacketThicknessMM;
        float gauges;
        float conductors;
        QString description;
        float grade;
        float part;
        float item;
    };

    struct DynamicComboData
    {
        DynamicComboData()
        {
            index = -1;
        }

        QString curName() const
        {
            if (index < 0 || index >= nameList.size())
                return QString("");

            return nameList.at(index);
        }

        QStringList nameList;
        int index;
    };

    QMap<const QtProperty *, FileData> pathProperties;
    QMap<const QtProperty *, ColorRamp> colorRampProperties;
    QMap<const QtProperty *, uint> uintProperties;
    QMap<const QtProperty *, bool> boolProperties;
public:
    QMap<const QtProperty *, SurfaceMaterialData> surfaceMaterialProperties;
    QMap<const QtProperty *, CableMaterialData> cableMaterialProperties;
    QMap<const QtProperty *, FireMaterialData> fireMaterialProperties;
    QMap<const QtProperty *, FailureCriteriaData> failureCriteriaProperties;
    QMap<const QtProperty *, VentCriteriaData> ventCriteriaProperties;
    QMap<const QtProperty *, DynamicComboData> dynamicComboProperties;
};
