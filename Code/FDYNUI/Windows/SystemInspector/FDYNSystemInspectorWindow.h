#pragma once

//namespace Ui {
//class NESystemInspectorWindow;
//}

class FDYNSystemInspector;

NE_FWD_DECL_1(class, NE_PRIVATE(FDYNSystemInspectorWindow))

class FDYNSystemInspectorWindow : public QMainWindow
{
	Q_OBJECT

public:
    explicit FDYNSystemInspectorWindow(QWidget *parent = 0);
    ~FDYNSystemInspectorWindow();

	void addRoot(QObject* root);
	void update();
	void setNewCurrentItem(QObject* obj);

    FDYNSystemInspector *systemInspector();

private:
    //Ui::NESystemInspectorWindow *ui;
    NE_DECL_PRIVATE(FDYNSystemInspectorWindow);
};
