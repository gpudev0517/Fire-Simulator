#pragma once

class NECEXP_GEOMETRY NEFailureCriteria
{
public:

    enum CriteriaType
    {
      Temperature = 0,
      Time = 1
    };

    NEFailureCriteria();
    ~NEFailureCriteria(){}

    NEFailureCriteria(QString name);

    const QString& name() const;
    void setName(const QString& val);

    int ID() const;
    void setID(int val);

    int index() const;
    void setIndex(int index);

    CriteriaType type() const;
    void setType(CriteriaType type);

    double threshold() const;
    void setThreshold(double val);

    operator QVariant() const;
    operator QString() const;

private:
    QString m_Name;
    int     m_ID;
    int     m_Index;
    CriteriaType m_Type;
    double m_Threshold;
};


Q_DECLARE_METATYPE(NEFailureCriteria)

