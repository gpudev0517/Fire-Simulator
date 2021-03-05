#pragma once

#include <QQuickWidget>

class FDYNTitlebarWidget;
class FDYNMenuBar;

class FDYNDockWidget : public QDockWidget
{
    Q_OBJECT
    friend class FDYNTitlebarWidget;
public:

    explicit FDYNDockWidget(QWidget *parent = 0);
    virtual ~FDYNDockWidget();

    void setContainedMainWindow(QMainWindow* mw);
    void setMenubar(FDYNMenuBar* bar);
    void setTitleText(QString val);

    QMainWindow* containedMainWindow();
    QString titleText();


    FDYNMenuBar *menubar();
protected:
    void closeEvent(QCloseEvent* event);
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

signals:
    void dockVisible(bool val);

public slots:
    void resizeTimeout();

    void tlChanged(bool flag);
private:
    QMainWindow* m_ContainedMainWindow;
    FDYNTitlebarWidget* m_Titlebar;
    QIcon m_Icon;
    QSize m_DockedMaxSize;

    QTimer m_ResizeTimer;

    FDYNMenuBar *m_MenuBar;
public:
};



