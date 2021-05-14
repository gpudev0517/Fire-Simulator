#pragma once

#include <QDialog>
#include <QItemSelection>
#include <QTableWidgetItem>

#include "Utility/NEFireMaterial.h"

namespace Ui {
class FDYNFireAddT2Dialog;
}

struct T2Input
{
    double timeToPeak;
    double peakHRR;
    double steadyBurningPeriod;
    double decayTime;
    T2Input() {
        timeToPeak = peakHRR = steadyBurningPeriod = decayTime = 0.0;
    }
};

class FDYNFireAddT2Dialog : public QDialog
{
    Q_OBJECT

    enum GROWTH_RATE{
        Custom,
        Slow,
        Medium,
        Fast,
        UltraFast
    };

public:
    explicit FDYNFireAddT2Dialog(QWidget *parent = 0);
    virtual ~FDYNFireAddT2Dialog();

    T2Input m_CurInputValue;
    void setDefaultValues(int index);

    void updateFireMaterialInfos(T2Input inputValue, QList<FireMaterialInfo> &infos);

    QList<FireMaterialInfo> m_infos;

private:
    Ui::FDYNFireAddT2Dialog *ui;

public slots:
    void fireGrowthRateChanged(int);
    void lineEditingFinished();


};
