#pragma once

namespace Ui {
class FDYNAboutDialog;
}

class FDYNAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNAboutDialog(QWidget *parent = 0);
    ~FDYNAboutDialog();

private:
    Ui::FDYNAboutDialog *ui;
};

