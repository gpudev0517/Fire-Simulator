#include "NEFailureCriteria.h"

NEFailureCriteria::NEFailureCriteria()
{
    m_Name = QString("");
    m_Index = 0;
    m_ID = 0;
    m_Type = Temperature;
    m_Threshold = 0.0;
}

NEFailureCriteria::NEFailureCriteria(QString name)
{
    m_Name = name;
}

const QString& NEFailureCriteria::name() const
{
    return m_Name;
}
void NEFailureCriteria::setName(const QString& val)
{
    m_Name = val;
}

int NEFailureCriteria::ID() const
{
    return m_ID;
}
void NEFailureCriteria::setID(int val)
{
    m_ID = val;
}

int NEFailureCriteria::index() const
{
    return m_Index;
}
void NEFailureCriteria::setIndex(int index)
{
    m_Index = index;
}

NEFailureCriteria::CriteriaType NEFailureCriteria::type() const
{
    return m_Type;
}
void NEFailureCriteria::setType(CriteriaType type)
{
    m_Type = type;
}

double NEFailureCriteria::threshold() const
{
    return m_Threshold;
}
void NEFailureCriteria::setThreshold(double val)
{
    m_Threshold = val;
}

NEFailureCriteria::operator QVariant() const
{
    return QVariant(getFDYNVarIds().failureCriteriaId, this);
}

NEFailureCriteria::operator QString() const
{
    return m_Name;
}
