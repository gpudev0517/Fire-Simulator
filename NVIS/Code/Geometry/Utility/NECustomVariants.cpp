#include "NECustomVariants.h"

FDYNVariantIds FDYNVarIds;

struct FDYNVariantIds& getFDYNVarIds()
{
    return FDYNVarIds;
}

NEDynamicCombo::NEDynamicCombo()
{
    m_CurIndex = -1;
}

NEDynamicCombo::NEDynamicCombo(const QString& name)
{
    m_CurIndex = 0;
    m_NameList.append(name);
}

NEDynamicCombo::NEDynamicCombo(const QStringList& nameList, int index)
{
    m_CurIndex = index;
    m_NameList = nameList ;
}

int NEDynamicCombo::curIndex() const
{
    return m_CurIndex;
}

QString NEDynamicCombo::curName() const
{
    if (m_CurIndex < 0 || m_CurIndex >= m_NameList.size())
        return QString("");

    return m_NameList.at(m_CurIndex);
}

void NEDynamicCombo::setCurIndex(int index)
{
    m_CurIndex = index;
}

void NEDynamicCombo::setCurIndexFromName(const QString& name)
{
    m_CurIndex = m_NameList.indexOf(name);
}

const QStringList& NEDynamicCombo::nameList() const
{
    return m_NameList;
}

void NEDynamicCombo::setNameList(const QStringList& nameList)
{
    m_NameList = nameList;
}

void NEDynamicCombo::add(const QString& name)
{
    m_NameList.append(name);
}

void NEDynamicCombo::insert(const QString& name, int index)
{
    m_NameList.insert(index, name);

    if (index <= m_CurIndex)
        m_CurIndex++;
}

void NEDynamicCombo::remove(const QString& name)
{
    int curIndex = m_CurIndex;

    int index = m_NameList.lastIndexOf(name);
    while (index != -1)
    {
        if (index <= curIndex)
            curIndex--;

        m_NameList.removeAt(index);
        index = m_NameList.lastIndexOf(name);
    }

    m_CurIndex = curIndex;
}

void NEDynamicCombo::clear()
{
    m_CurIndex = -1;
    m_NameList.clear();
}

NEDynamicCombo::operator QVariant() const
{
    return QVariant(getFDYNVarIds().dynamicComboId, this);
}

NEDynamicCombo::operator QString() const
{
    return curName();
}
