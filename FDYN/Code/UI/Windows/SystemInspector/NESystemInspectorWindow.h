#pragma once

namespace Ui {
class NESystemInspectorWindow;
}

class NESystemInspectorWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit NESystemInspectorWindow(QWidget *parent = 0);
	~NESystemInspectorWindow();

	void addRoot(QObject* root);
	void update();
	void setNewCurrentItem(QObject* obj);

private:
	Ui::NESystemInspectorWindow *ui;
};
