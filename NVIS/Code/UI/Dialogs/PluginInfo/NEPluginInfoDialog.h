#pragma once

class NEPreferencesDialog;

namespace Ui {
class NEPluginInfoDialog;
}

class NEPluginInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NEPluginInfoDialog(QWidget *parent = 0);
    ~NEPluginInfoDialog();

    void setName(QString s);
    void setPath(QString s);
    void setHelp(QString s);
    void setVendor(QString s);
    void setFilename(QString s);
    void setVersion(QString s);

public slots:
    void reloadPlugin();

private:
    Ui::NEPluginInfoDialog *ui;
    NEPreferencesDialog* m_pPD;
};



