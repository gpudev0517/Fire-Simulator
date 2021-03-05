#pragma once

#include <QDialog>

namespace Ui {
class FDYNFailureCriteriaDialog;
}

#include "Utility/NEFailureCriteria.h"

struct FailureCriteria
{
    int     m_Index;
    QString m_Name;
    int m_ID;
    NEFailureCriteria::CriteriaType m_Type;
    double m_Threshold;

    FailureCriteria()
    {
    }
    FailureCriteria( const NEFailureCriteria& fail )
    {
       copy(fail);
    }
    void operator =(const NEFailureCriteria& fail )
    {
        copy( fail );
    }
    operator NEFailureCriteria()
    {
        NEFailureCriteria fail;
        fail.setID( m_ID );
        fail.setThreshold( m_Threshold );
        fail.setName( m_Name );
        fail.setType(m_Type);
        fail.setIndex( m_Index );
        return fail;
    }
    void copy(const NEFailureCriteria& fail)
    {
        m_ID = fail.ID();
        m_Threshold = fail.threshold();
        m_Index = fail.index();
        m_Type = fail.type();
        m_Name = fail.name();
    }
};

class NENode;

class FDYNFailureCriteriaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNFailureCriteriaDialog(QWidget *parent = 0);
    virtual ~FDYNFailureCriteriaDialog();

    QStringList failureCriteriaList();

    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type);

    void setDefaultFailureCriteriaProps(FailureCriteria &fail);
    void addRowToTable(FailureCriteria &fail);
    void clear();

signals:
    void sendAddFailureCriteria(FailureCriteria fail);
    void sendRemoveFailureCriteria(FailureCriteria fail);
    void sendAssignFailureCriteria(NENode *node, FailureCriteria fail);

public slots:
    void editedLineEdit();

    FailureCriteria createFailureCriteria();
    FailureCriteria createNullFailureCriteria();
    void addFailureCriteria();
    void addFailureCriteria(FailureCriteria& fail);
    void updateFailureCriteria(FailureCriteria& fail);
    void duplicateFailureCriteria();
    void removeFailureCriteria();
    void criteriaTypeChanged(int _idx);

    void updateRow(const QItemSelection&, const QItemSelection&);
    void updateItem(QTableWidgetItem *item);

    virtual void accept();
    virtual void reject();

private:
    Ui::FDYNFailureCriteriaDialog *ui;
    int m_prevSelectedIndex;
public:
    FailureCriteria *m_CurFailureCriteria;
    QList<FailureCriteria> m_FailureCriteriaList;
};
