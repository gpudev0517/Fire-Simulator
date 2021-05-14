#include "NESurfaceMaterial.h"

NESurfaceMaterial::NESurfaceMaterial()
{
}

NESurfaceMaterial::NESurfaceMaterial(QString name)
{
    m_Name = name;
}

QString NESurfaceMaterial::name() const
{
    return m_Name;
}
void NESurfaceMaterial::setName(QString val)
{
    m_Name = val;
}

int NESurfaceMaterial::uniqueID() const
{
    return m_uniqueID;
}
void NESurfaceMaterial::setUniqueID(int val)
{
    m_uniqueID = val;
}

int NESurfaceMaterial::index() const
{
    return m_Index;
}
void NESurfaceMaterial::setIndex(int val)
{
    m_Index = val;
}

double NESurfaceMaterial::conductivity() const
{
    return m_Conductivity;
}
void NESurfaceMaterial::setConductivity(double val)
{
    m_Conductivity = val;
}

double NESurfaceMaterial::heat() const
{
    return m_Heat;
}
void NESurfaceMaterial::setHeat(double val)
{
    m_Heat = val;
}

double NESurfaceMaterial::density() const
{
    return m_Density;
}
void NESurfaceMaterial::setDensity(double val)
{
    m_Density = val;
}

double NESurfaceMaterial::thickness() const
{
    return m_Thickness;
}
void NESurfaceMaterial::setThickness(double val)
{
    m_Thickness = val;
}

double NESurfaceMaterial::emissivity() const
{
    return m_Emissivity;
}
void NESurfaceMaterial::setEmissivity(double val)
{
    m_Emissivity = val;
}

NESurfaceMaterial::operator QVariant() const
{
    return QVariant(getFDYNVarIds().surfaceMaterialId, this);
}

NESurfaceMaterial::operator QString() const
{
    return m_Name;
}
bool NESurfaceMaterial::operator==(const NESurfaceMaterial& other) const {
    const NESurfaceMaterial& d = *this;
    const NESurfaceMaterial& material = other;
    return (d.name() == material.name()
            && d.uniqueID() == material.uniqueID()
            && d.index() == material.index()
            && d.conductivity() == material.conductivity()
            && d.heat() == material.heat()
            && d.density() == material.density()
            && d.thickness() == material.thickness()
            && d.emissivity() == material.emissivity());
}
