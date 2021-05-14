#pragma once

class NECEXP_GEOMETRY NESurfaceMaterial
{
public:

    NESurfaceMaterial();
    ~NESurfaceMaterial(){}

    NESurfaceMaterial(QString name);

    QString name() const;
    void setName(QString val);

    int uniqueID() const;
    void setUniqueID(int val);

    int index() const;
    void setIndex(int val);

    double conductivity() const;
    void setConductivity(double val);

    double heat() const;
    void setHeat(double val);

    double density() const;
    void setDensity(double val);

    double thickness() const;
    void setThickness(double val);

    double emissivity() const;
    void setEmissivity(double val);

	operator QVariant() const;
	operator QString() const;
    bool operator==(const NESurfaceMaterial& other) const;

private:
    QString m_Name;
    int m_uniqueID;
    int m_Index;
    double m_Conductivity;
    double m_Heat;
    double m_Density;
    double m_Thickness;
    double m_Emissivity;
};

Q_DECLARE_METATYPE(NESurfaceMaterial)
