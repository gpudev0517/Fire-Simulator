#pragma once

#include <QDialog>

namespace Ui {
class FDYNVentCriteriaDialog;
}

#include "Utility/NEVentCriteria.h"


struct VentCriteria
{

    int     m_Index;
    QString m_Name;
    int m_ID;
    NEVentCriteria::CriteriaType m_Type;
    QList<NEVentCriteria::VentValues> m_Values;

    VentCriteria()
    {
        m_Index = 0;
        m_ID = 0;
        m_Type = NEVentCriteria::Temperature;
    }
    VentCriteria( const NEVentCriteria& crit )
    {
       copy(crit);
    }
    void operator =(const NEVentCriteria& crit )
    {
        copy(crit );
    }
    operator NEVentCriteria()
    {
        NEVentCriteria criterion;
        criterion.setID( m_ID );
        criterion.setName( m_Name );
        criterion.setType(m_Type);
        criterion.setIndex( m_Index );
        criterion.setValues(m_Values);
        return criterion;
    }
    void copy(const NEVentCriteria& criterion)
    {
        m_ID = criterion.ID();
        m_Index = criterion.index();
        m_Type = criterion.type();
        m_Name = criterion.name();
        m_Values = criterion.values();
    }
};

class NENode;

class FDYNVentCriteriaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNVentCriteriaDialog(QWidget *parent = 0);
    virtual ~FDYNVentCriteriaDialog();

    QStringList ventCriteriaList();

    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type, QVariant vName);

    void setDefaultVentCriteriaProps(VentCriteria &criterion);
    void addRowToTable(VentCriteria &criterion);
    void clear();

signals:
    void sendAddVentCriteria(VentCriteria criterion);
    void sendRemoveVentCriteria(VentCriteria criterion);
    void sendAssignVentCriteria(NENode *node, VentCriteria criterion, QString criterionKind);

public slots:
    void editedLineEdit();

    VentCriteria createVentCriteria();
    VentCriteria createNullVentCriteria();
    void addVentCriteria();
    void addVentCriteria(VentCriteria& criterion);
    void updateVentCriteria(VentCriteria& criterion);
    void duplicateVentCriteria();
    void removeVentCriteria();
    void criteriaTypeChanged(int _idx);
    void criteriaValueClicked();

    void updateRow(const QItemSelection&, const QItemSelection&);
    void updateItem(QTableWidgetItem *item);

    virtual void accept();
    virtual void reject();

private:
    Ui::FDYNVentCriteriaDialog *ui;
    int m_prevSelectedIndex;
    QString m_CurCriterionKind;

public:
    VentCriteria *m_CurVentCriteria;
    QList<VentCriteria> m_VentCriteriaList;
};


