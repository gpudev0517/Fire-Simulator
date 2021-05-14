#include "NEVentCriteria.h"

NEVentCriteria::NEVentCriteria()
{
    m_Name = QString("");
    m_Index = 0;
    m_ID = 0;
    m_Type = Temperature;
}

NEVentCriteria::NEVentCriteria(const QString& name)
{
    m_Name = name;
    m_Index = 0;
    m_ID = 0;
    m_Type = Temperature;
}

const QString& NEVentCriteria::name() const
{
    return m_Name;
}
void NEVentCriteria::setName(const QString& val)
{
    m_Name = val;
}

int NEVentCriteria::ID() const
{
    return m_ID;
}
void NEVentCriteria::setID(int val)
{
    m_ID = val;
}

int NEVentCriteria::index() const
{
    return m_Index;
}
void NEVentCriteria::setIndex(int index)
{
    m_Index = index;
}

NEVentCriteria::CriteriaType NEVentCriteria::type() const
{
    return m_Type;
}
void NEVentCriteria::setType(CriteriaType type)
{
    m_Type = type;
}

void NEVentCriteria::setValues(const QList<VentValues>& values)
{
    m_Values = values;
}
const QList<NEVentCriteria::VentValues>& NEVentCriteria::values() const
{
    return m_Values;
}

NEVentCriteria::operator QVariant() const
{
    return QVariant(getFDYNVarIds().ventCriteriaId, this);
}

NEVentCriteria::operator QString() const
{
    return m_Name;
}
