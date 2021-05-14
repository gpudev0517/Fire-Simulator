#include "NEFireMaterial.h"

NEFireMaterial::NEFireMaterial()
{
}

NEFireMaterial::NEFireMaterial(QString name)
{
    m_Name = name;
}

QString NEFireMaterial::name() const
{
    return m_Name;
}

void NEFireMaterial::setName(QString val)
{
    m_Name = val;
}

int NEFireMaterial::uniqueID() const
{
    return m_uniqueID;
}
void NEFireMaterial::setUniqueID(int val)
{
    m_uniqueID = val;
}

int NEFireMaterial::index() const
{
    return m_Index;
}
void NEFireMaterial::setIndex(int val)
{
    m_Index = val;
}

double NEFireMaterial::C() const
{
    return m_C;
}
void NEFireMaterial::setC(double val)
{
    m_C= val;
}

double NEFireMaterial::N() const
{
    return m_N;
}
void NEFireMaterial::setN(double val)
{
    m_N = val;
}

double NEFireMaterial::H() const
{
    return m_H;

}
void NEFireMaterial::setH(double val)
{
    m_H = val;
}

double NEFireMaterial::CI() const
{
    return m_CI;
}
void NEFireMaterial::setCI(double val)
{
    m_CI = val;
}

double NEFireMaterial::O() const
{
    return m_O;
}
void NEFireMaterial::setO(double val)
{
    m_O = val;
}

double NEFireMaterial::combuHeat() const
{
    return m_CombuHeat;
}
void NEFireMaterial::setCombuHeat(double val)
{
    m_CombuHeat = val;
}

double NEFireMaterial::radFraction() const
{
    return m_RadFraction;
}
void NEFireMaterial::setRadFraction(double val)
{
    m_RadFraction = val;
}

QList<FireMaterialInfo> NEFireMaterial::Infos() const
{
    return m_Infos;
}
void NEFireMaterial::setInfos(QList<FireMaterialInfo> _infos)
{
    m_Infos = _infos;
}

NEFireMaterial::operator QVariant() const
{
    return QVariant(getFDYNVarIds().fireMaterialId, this);
}

NEFireMaterial::operator QString() const
{
    return m_Name;
}
