#pragma once

class NECEXP_GEOMETRY NEVentCriteria
{
public:
    enum CriteriaType
    {
      Temperature = 0,
      Time = 1
    };

    struct VentValues
    {
        double m_Threshold;
        double m_Percent;
        VentValues()
        {
            m_Threshold = 0.0;
            m_Percent = 0.0;
        }

        bool operator==(const VentValues& r) const
        {
            return m_Threshold == r.m_Threshold && m_Percent == r.m_Percent;
        }
    };

    NEVentCriteria();
    NEVentCriteria(const QString& name);
    ~NEVentCriteria(){}

    const QString& name() const;
    void setName(const QString& val);

    int ID() const;
    void setID(int val);

    int index() const;
    void setIndex(int index);

    CriteriaType type() const;
    void setType(CriteriaType type);

    const QList<VentValues>& values() const;
    void setValues(const QList<VentValues>& values);

    operator QVariant() const;
    operator QString() const;

    bool operator==(const NEVentCriteria& r) const
    {
        return m_Name == r.m_Name && m_ID == r.m_ID && m_Type == r.m_Type && m_Values == r.m_Values;
    }

private:
    QString m_Name;
    int     m_ID;
    int     m_Index;
    CriteriaType m_Type;
    QList<VentValues> m_Values;
};

Q_DECLARE_METATYPE(NEVentCriteria)

