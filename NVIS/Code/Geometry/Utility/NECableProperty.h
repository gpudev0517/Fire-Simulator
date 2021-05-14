#pragma once

class NECEXP_GEOMETRY NECableProperty
{
public:

    NECableProperty();
    ~NECableProperty(){}

    NECableProperty(QString name);

    QString name() const;
    void setName(QString val);

    QString cleanName() const;
    void setCleanName(QString val);


    int uniqueID() const;
    void setUniqueID(int val);

    int index() const;
    void setIndex(int val);

    QString manufacturer() const;
    void setManufacturer( QString _man );

    QString category() const;
    void setCategory( QString _cat );

    QString issues() const;
    void setIssues( QString _iss );

    QString model() const;
    void setModel(QString _model);

    int insulationType() const;
    void setInsulationType(int _type);

    int jacketType() const;
    void setJacketType(int _type);

    float outSideDiameterMM() const;
    void setOutsideDiameterMM(float _value);

    float outSideDiameterKgPerMeter() const;
    void setOutSideDiameterKgPerMeter(float _value);

    float jacketThicknesMM() const;
    void setJacketThicknessMM(float _value);

    float gauges() const;
    void setGauges(float _value);

    float conductors() const;
    void setConductors(float _value);

    QString description() const;
    void setDescription(QString _value);

    float grade() const;
    void setGrade(float val);

    float part() const;
    void setPart(float _value);

    float item() const;
    void setItem(float _value);


    operator QVariant() const;
    operator QString() const;
private:
    QString m_Name;
    int m_ID; //  Id assigned by the database
    int m_Index; // Index in m_MaterialList
    QString m_issues;

    QString m_manufacturer;
    QString m_category;

    QString m_Model;


    int m_InsulationType;
    int m_JacketType;
    float m_OutSideDiameterMM;
    float m_OutSideDiameterKgPerMeter;
    float m_JacketThicknessMM;
    float m_Gauges;
    float m_Conductors;
    QString m_Description;
    float m_Grade;
    float m_Part;
    float m_Item;

    QString m_CleanName;


};

Q_DECLARE_METATYPE(NECableProperty)
