#ifndef FDYNSettingsDialog_H
#define FDYNSettingsDialog_H

#include <QDialog>

namespace Ui {
class FDYNSettingsDialog;
}

class FDYNSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDYNSettingsDialog(QWidget *parent = nullptr);
    ~FDYNSettingsDialog();

    QString selectedTempDataDir();
    QString selectedUIDataDir();

private:
    Ui::FDYNSettingsDialog *ui;


public slots:
    void setTempDataDirAccept();
    void setTempDataDir(QString tempDataDir);

    bool loadUIData();
    bool saveUIData();
    void setLoadUIData( bool val );
    void setSaveUIData( bool val );

    void setUIDataDirAccept();
    void setUIDataDir(QString UIDataDir);
};

#endif // FDYNSettingsDialog_H
