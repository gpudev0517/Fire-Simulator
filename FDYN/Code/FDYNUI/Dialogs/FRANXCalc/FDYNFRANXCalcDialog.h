#ifndef FDYNFRANXCalcDialog_H
#define FDYNFRANXCalcDialog_H

#include <QDialog>

#include "Geometry/FEngine/FFRANXProperties.h"

namespace Ui {
    class FDYNFRANXCalcDialog;
}


class FDYNFRANXCalcDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNFRANXCalcDialog(QWidget *parent = nullptr);
    ~FDYNFRANXCalcDialog();

    QString selectedTempDataDir();

    void addRowToTable(FRANXProperties &material);
private:
    Ui::FDYNFRANXCalcDialog *ui;
    FRANXProperties *m_CurScenario;
    QList<FRANXProperties> m_ScenarioList;
    int m_prevSelectedIndex;
    QString m_CurrentScenarioKind;

signals:
    void sendFranxCalc();
    void sendUpdateComputeScenario( FRANXProperties fp );

public slots:
    void setTempDataDirAccept();
    void setTempDataDir(const QString tempDataDir);


//    void updateScenarios(FRANXProperties &fp);
    void addScenario(FRANXProperties &fp);
    void updateItem(QTableWidgetItem *item);
    void reject();
    void setDefaultProps(FRANXProperties &mat);
    void clear();
    QStringList scenarioNameList();
    void showWithPropertyValues(QWidget *propertyBrowser, QVariant idx, QVariant val, QVariant type, QVariant vName);
    void accept();

    void updateScenarios(QList<FRANXProperties> &fplist);
    void franxDataFinished();
};




#endif // FDYNFRANXCalcDialog_H
