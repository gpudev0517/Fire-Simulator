#pragma once

#include <QDialog>


namespace Ui {
class FDYNSimulateScenarioDialog;
}

class NENode;

class FDYNSimulateScenarioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNSimulateScenarioDialog(QWidget *parent = 0, NENode *simengine = 0);
    virtual ~FDYNSimulateScenarioDialog();

    QString  ScenarioName();
    QString  Desc();

    void setDefaultLineEdit();

public slots:
    void editedLineEdit();
    void SetScenarioName(QString scnName);
private:
    Ui::FDYNSimulateScenarioDialog *ui;

    double m_SimTime;
    double m_IntTemperature;
    double m_Humidity;
    double m_ExtTemperature;
    double m_Pressure;

    double m_MaxTimeStep;
    double m_LowerOxyLimit;

    double m_OutputInterval;
    NENode *m_simEngine;

};
