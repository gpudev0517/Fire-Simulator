#ifndef NEDOCKWIDGET_H
#define NEDOCKWIDGET_H

class NETitlebarWidget;
class NEMenuBar;

class NEDockWidget : public QDockWidget
{
    Q_OBJECT
    friend class NEMenuBar;
    friend class NETitlebarWidget;
public:

    explicit NEDockWidget(QWidget *parent = 0);
    //virtual ~NEDockWidget();

    void setContainedMainWindow(QMainWindow* mw);
    void setMenubar(NEMenuBar* bar);
    void setTitleText(QString val);

    QMainWindow* containedMainWindow();
    NEMenuBar* menubar();
    QString titleText();

protected:
    void closeEvent(QCloseEvent* event);
    void hideEvent(QHideEvent* event);
    void showEvent(QShowEvent* event);
    void resizeEvent(QResizeEvent* event);

signals:
    void dockVisible(bool val);

public slots:
    void tlChanged(bool);
    void resizeTimeout();

private:
    QMainWindow* m_ContainedMainWindow;
    NETitlebarWidget* m_Titlebar;
    NEMenuBar* m_MenuBar;
    QIcon m_Icon;
    QSize m_DockedMaxSize;

    QTimer m_ResizeTimer;
};




#endif // NEDOCKWIDGET_H
