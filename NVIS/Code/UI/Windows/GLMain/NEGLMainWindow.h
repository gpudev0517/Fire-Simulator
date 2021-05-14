#pragma once

namespace Ui {
class NEGLMainWindow;
}

class NEGUIManager;
class NEMainWindow;
struct GLDockWidgetBundle;
class NECamera;
class QActionGroup;

class NEGLMainWindow : public QMainWindow
{
	Q_OBJECT

	friend class NEGUIManager;
    friend class NEGUIManagerPrivate;
    friend class NEMainWindow;

public:
	explicit NEGLMainWindow(QWidget *parent = 0);
	~NEGLMainWindow();

	//! Add the given camera to the OpenGL window
	void addCamera(NECamera* cam);
	//! Add multiple cameras to the OpenGL window
	void addCameras(QMap<QString, NECamera *> cams);
	//! Remove the given cameras from the OpenGL window
	void removeCamera(NECamera *cam);
	//! Establishes connections
	void setupConnections();


public slots:
	//! Sets the given camera as the OpenGL window's active camera
	void setCamera(QObject *cam);
    void updateUIForCurrentCamera(QObject *cam);

    void setContextMenuPolicyFromGLWidget(int policy);

    void maximizeGLWindow(bool val);

    bool isMaximized();

    void maximizeToggle();


protected:
    GLDockWidgetBundle* m_pBundle;

private:

	QMenu* camMenu();

    Ui::NEGLMainWindow *ui;

	QMenu* m_pCamMenu;
	QMap<NECamera*, QAction*> m_CameraToAction;
	QActionGroup* m_pActionGroup;
    QByteArray uiStateBeforeMaxed;
};
