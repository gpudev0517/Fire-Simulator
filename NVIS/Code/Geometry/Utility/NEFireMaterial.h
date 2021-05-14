#pragma once

struct NECEXP_GEOMETRY FireMaterialInfo{
    double m_Time;
    double m_HRR;
    double m_Height;
    double m_Area;
    double m_CO;
    double m_Soot;
    double m_HCN;
    double m_TS;

    FireMaterialInfo(){
        m_Time = m_Height = m_HRR = m_Area = m_CO = m_Soot = m_HCN = m_TS = 0;
    }

    bool operator==(const FireMaterialInfo& r) const
    {
        if(m_Time == r.m_Time && m_HRR == r.m_HRR && m_Height == r.m_Height && m_Area == r.m_Area && m_CO == r.m_CO && m_Soot == r.m_Soot && m_HCN == r.m_HCN && m_TS == r.m_TS)
            return true;
        else
            return false;
    }
};

class NECEXP_GEOMETRY NEFireMaterial
{
public:

    NEFireMaterial();
    ~NEFireMaterial(){}

    NEFireMaterial(QString name);

    QString name() const;
    void setName(QString val);

    int uniqueID() const;
    void setUniqueID(int val);

    int index() const;
    void setIndex(int val);

    double C() const;
    void setC(double val);

    double N() const;
    void setN(double val);

    double H() const;
    void setH(double val);

    double CI() const;
    void setCI(double val);

    double O() const;
    void setO(double val);

    double combuHeat() const;
    void setCombuHeat(double val);

    double radFraction() const;
    void setRadFraction(double val);

    QList<FireMaterialInfo> Infos() const;
    void setInfos(QList<FireMaterialInfo> _infos);

	operator QVariant() const;
	operator QString() const;

    bool operator==(const NEFireMaterial& r) const
    {
        if( m_uniqueID == r.m_uniqueID && m_Name == r.m_Name && m_C == r.m_C && m_N == r.m_N && m_H == r.m_H &&
                m_CI == r.m_CI && m_O == r.m_O && m_CombuHeat == r.m_CombuHeat && m_RadFraction == r.m_RadFraction && m_Infos == r.m_Infos)
            return true;
        else
            return false;
    }

private:
    QString m_Name;
    int m_uniqueID;
    int m_Index;
    double m_C;
    double m_N;
    double m_H;
    double m_CI;
    double m_O;
    double m_CombuHeat;
    double m_RadFraction;

    QList<FireMaterialInfo> m_Infos;
};

Q_DECLARE_METATYPE(NEFireMaterial)

