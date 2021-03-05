#ifndef FDYNCREATEPROJECTDIALOG_H
#define FDYNCREATEPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class FDYNCreateProjectDialog;
}

class FDYNCreateProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNCreateProjectDialog(QWidget *parent = nullptr);
    ~FDYNCreateProjectDialog();

    QString selectedProjectFile();
    QString selectedFRANXFile();
    QString selectedPDMSFile();

private:
    Ui::FDYNCreateProjectDialog *ui;


public slots:
    void newProject();
    void chooseFRANXLocation();
    void choosePDMSLocation();
};

#endif // FDYNCREATEPROJECTDIALOG_H
