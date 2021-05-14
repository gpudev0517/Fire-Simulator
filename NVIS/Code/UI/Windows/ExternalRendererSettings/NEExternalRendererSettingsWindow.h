#pragma once

namespace Ui {
class NEExternalRendererSettingsWindow;
}

class NEExternalRendererSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEExternalRendererSettingsWindow(QWidget *parent = 0);
    ~NEExternalRendererSettingsWindow();

public slots:
    void changeRenderer();
    void refresh();

protected:
    void showEvent(QShowEvent * event);

private:
    Ui::NEExternalRendererSettingsWindow *ui;
};

