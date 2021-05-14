#ifndef NEPLUGINMANAGERWINDOW_H
#define NEPLUGINMANAGERWINDOW_H

namespace Ui {
class NEPluginManagerWindow;
}

class NEPluginManagerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEPluginManagerWindow(QWidget *parent = 0);
    ~NEPluginManagerWindow();

private:
    Ui::NEPluginManagerWindow *ui;
};

#endif // NEPLUGINMANAGERWINDOW_H
