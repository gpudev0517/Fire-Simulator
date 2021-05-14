#include "NECableProperty.h"


NECableProperty::NECableProperty()
{
}

NECableProperty::NECableProperty(QString name)
{
    m_Name = name;
}



QString NECableProperty::name() const
{
    return m_Name;
}
void NECableProperty::setName(QString val)
{
    m_Name = val;
}

QString NECableProperty::cleanName() const
{
    return m_CleanName;
}

void NECableProperty::setCleanName(QString val)
{
    m_CleanName = val;
}

int NECableProperty::uniqueID() const
{
    return m_ID;
}
void NECableProperty::setUniqueID(int val)
{
   m_ID = val;
}

int NECableProperty::index() const
{
    return m_Index;
}
void NECableProperty::setIndex(int val)
{
    m_Index = val;
}

QString NECableProperty::manufacturer() const
{
    return m_manufacturer ;
}

void NECableProperty::setManufacturer(QString _man)
{
    m_manufacturer = _man;
}

QString NECableProperty::category() const
{
    return m_category;
}

void NECableProperty::setCategory(QString _cat)
{
    m_category = _cat;
}

QString NECableProperty::issues() const
{
    return m_issues;
}

void NECableProperty::setIssues(QString _iss)
{
    m_issues = _iss;
}

QString NECableProperty::model() const
{
    return m_Model;
}
void NECableProperty::setModel(QString _model)
{
    m_Model = _model;
}

int NECableProperty::insulationType() const
{
    return m_InsulationType;
}
void NECableProperty::setInsulationType(int _type)
{
    m_InsulationType = _type;
}

int NECableProperty::jacketType() const
{
    return m_JacketType;
}
void NECableProperty::setJacketType(int _type)
{
    m_JacketType = _type;
}

float NECableProperty::outSideDiameterMM() const
{
    return m_OutSideDiameterMM;
}
void NECableProperty::setOutsideDiameterMM(float _value)
{
    m_OutSideDiameterMM = _value;
}

float NECableProperty::outSideDiameterKgPerMeter() const
{
    return m_OutSideDiameterKgPerMeter;
}
void NECableProperty::setOutSideDiameterKgPerMeter(float _value)
{
    m_OutSideDiameterKgPerMeter = _value;
}

float NECableProperty::jacketThicknesMM() const
{
    return m_JacketThicknessMM;
}
void NECableProperty::setJacketThicknessMM(float _value)
{
    m_JacketThicknessMM = _value;
}

float NECableProperty::gauges() const
{
    return m_Gauges;
}
void NECableProperty::setGauges(float _value)
{
    m_Gauges = _value;
}

float NECableProperty::conductors() const
{
    return m_Conductors;
}
void NECableProperty::setConductors(float _value)
{
    m_Conductors = _value;
}

QString NECableProperty::description() const
{
    return m_Description;
}
void NECableProperty::setDescription(QString _value)
{
    m_Description = _value;
}

float NECableProperty::grade() const
{
    return m_Grade;
}
void NECableProperty::setGrade(float val)
{
    m_Grade = val;
}

float NECableProperty::part() const
{
    return m_Part;
}
void NECableProperty::setPart(float _value)
{
    m_Part = _value;
}

float NECableProperty::item() const
{
    return m_Item;
}
void NECableProperty::setItem(float _value)
{
    m_Item = _value;
}


NECableProperty::operator QVariant() const
{
    return QVariant(getFDYNVarIds().cableMaterialId, this);
}

NECableProperty::operator QString() const
{
    return m_Name;
}
