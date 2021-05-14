#pragma once

namespace Ui {
class NESceneManagementDialog;
}

class NESceneManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NESceneManagementDialog(QWidget *parent = 0);
    ~NESceneManagementDialog();

private:
    Ui::NESceneManagementDialog *ui;
};
