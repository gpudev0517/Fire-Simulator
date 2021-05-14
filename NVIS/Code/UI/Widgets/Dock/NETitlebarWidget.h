#pragma once


namespace Ui {
class NETitlebarWidget;
}

class QMenuBar;
class NEDockWidget;

class NETitlebarWidget : public QWidget
{
	Q_OBJECT
	friend class NEDockWidget;

public:
	explicit NETitlebarWidget(QWidget *parent = 0);
	virtual ~NETitlebarWidget();
public slots:
    void dock(bool val);
    void maximizeDock(bool val);
	void hideDock();
    void closeDock();

private:
	Ui::NETitlebarWidget *ui;
	NEDockWidget* m_DockWidget;
    bool wasFloating;
    QByteArray uiStateBeforeMaxed;
};
