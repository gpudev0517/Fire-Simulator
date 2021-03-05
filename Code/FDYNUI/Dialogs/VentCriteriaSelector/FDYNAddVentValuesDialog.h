#pragma once

#include <QDialog>
#include "Utility/NEVentCriteria.h"

namespace Ui {
class FDYNAddVentValuesDialog;
}

#include "Utility/NEVentCriteria.h"
class FDYNAddVentValuesDialog: public QDialog{
    Q_OBJECT
public:
    explicit FDYNAddVentValuesDialog(NEVentCriteria::CriteriaType type, QList<NEVentCriteria::VentValues> values, QWidget *parent = 0); //
    virtual ~FDYNAddVentValuesDialog(){}

    void drawGraph();

    QList<NEVentCriteria::VentValues> m_Values;
    NEVentCriteria::CriteriaType m_Type;

public slots:
    virtual void accept();
    virtual void reject();
    void addValue();
    void removeValue();
    void updateValues(QTableWidgetItem* item);
private:
    Ui::FDYNAddVentValuesDialog *ui;

};
