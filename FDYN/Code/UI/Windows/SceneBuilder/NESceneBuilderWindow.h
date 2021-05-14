#pragma once


namespace Ui {
class NESceneBuilderWindow;
}

class NESceneBuilderWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit NESceneBuilderWindow(QWidget *parent = 0);
    ~NESceneBuilderWindow();

    //! Adds the given action as a button into the scene builder
    void addCreateAction(QAction* action, uint nodeType);

    void removeCreateAction(QAction* action, uint nodeType);

private:
    Ui::NESceneBuilderWindow *ui;
};

