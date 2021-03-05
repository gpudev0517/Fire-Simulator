#pragma once

#include <QDialog>

namespace Ui {
class FDYNLoadingDialog;
}

class FDYNLoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNLoadingDialog(QWidget *parent = 0);
    ~FDYNLoadingDialog();
    void setMainOperation(QString s);
    void setCurrentOperation(QString s);
    void setCurrentSubOperation(QString s);
    void setPercentage(int p);

private:
    Ui::FDYNLoadingDialog *ui;
    QMovie *movie;

};
