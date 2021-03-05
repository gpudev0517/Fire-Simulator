#pragma once

#include <QWidget>

namespace Ui {
    class FDYNTitlebarWidget;
}

class FDYNDockWidget;

class FDYNTitlebarWidget : public QWidget
{
    Q_OBJECT
    friend class FDYNDockWidget;

public:
    explicit FDYNTitlebarWidget(QWidget *parent = 0);
    virtual ~FDYNTitlebarWidget();
public slots:
    void dock(bool val);
    void maximizeDock(bool val);
    void hideDock();
    void closeDock();

private:
    Ui::FDYNTitlebarWidget *ui;
    FDYNDockWidget* m_DockWidget;
    bool wasFloating;
    QByteArray uiStateBeforeMaxed;
};
