#pragma once

#include <QDialog>

namespace Ui {
class NESceneLoadingDialog;
}

class NESceneLoadingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NESceneLoadingDialog(QWidget *parent = 0);
    ~NESceneLoadingDialog();
    void setMainOperation(QString s);
    void setCurrentOperation(QString s);
    void setCurrentSubOperation(QString s);
    void setPercentage(int p);

private:
    Ui::NESceneLoadingDialog *ui;
};
