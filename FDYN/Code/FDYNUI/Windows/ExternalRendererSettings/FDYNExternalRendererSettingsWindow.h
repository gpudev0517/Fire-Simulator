#pragma once

namespace Ui {
class FDYNExternalRendererSettingsWindow;
}

class FDYNExternalRendererSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FDYNExternalRendererSettingsWindow(QWidget *parent = 0);
    ~FDYNExternalRendererSettingsWindow();

public slots:
    void changeRenderer();
    void refresh();

protected:
    void showEvent(QShowEvent * event);

private:
    Ui::FDYNExternalRendererSettingsWindow *ui;
};

