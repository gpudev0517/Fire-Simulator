#pragma once

class NECEXP_GEOMETRY NEDynamicCombo
{
public:

    NEDynamicCombo();
    NEDynamicCombo(const QString& name);
    NEDynamicCombo(const QStringList& nameList, int index);
    ~NEDynamicCombo(){}

    int curIndex() const;
    QString curName() const;

    void setCurIndex(int index);
    void setCurIndexFromName(const QString& name);

    const QStringList& nameList() const;
    void setNameList(const QStringList& nameList);

    void add(const QString& name);
    void insert(const QString& name, int index);
    void remove(const QString& name);
    void clear();

    operator QVariant() const;
    operator QString() const;

private:
    QStringList m_NameList;
    int m_CurIndex;
};

Q_DECLARE_METATYPE(NEDynamicCombo)
