#pragma once

namespace Ui {
class FDYNGLMainWindow;
}

class FDYNGUIManager;
class FDYNMainWindow;
struct GLDockWidgetBundle;
class NECamera;
class QActionGroup;
class QQuickWidget;

class FDYNGLMainWindow : public QMainWindow
{
    Q_OBJECT

    friend class FDYNGUIManager;
    friend class FDYNGUIManagerPrivate;
    friend class FDYNMainWindow;

public:
    explicit FDYNGLMainWindow(QWidget *parent = 0);
    ~FDYNGLMainWindow();

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

    void selectCursorMode(bool checked);
    void selectTranslationMode(bool checked);
    void selectRotationMode(bool checked);
    void selectScaleMode(bool checked);

protected:
    GLDockWidgetBundle* m_pBundle;

private:

    QMenu* camMenu();

    //Ui::FDYNGLMainWindow *ui;

    QMenu* m_pViewMenu;

    QAction *actionGrid;
    QAction *actionDraw_render_area;
    QAction *actionSplit_horizontally;
    QAction *actionSplit_vertically;
    QAction *actionSplit_to_quad;
    QAction *actionShow_OSD;
    QAction *actionClip;
    QAction *actionClose;
    QAction *actionShow_Axes;
    QAction *actionFrameSelection;
    QAction *actionHide;
    QAction *actionMaximize;
    QAction *actionDynamicQuality;
    QAction *actionResetCamera;

    QMenu* m_pCamMenu;
    QMap<NECamera*, QAction*> m_CameraToAction;
    QActionGroup* m_pActionGroup;
    QByteArray uiStateBeforeMaxed;

    QObject *btnCursor;
    QObject *btnTranslate;
    QObject *btnRotate;
    QObject *btnScale;
    QObject *btnResetCamera;

    QObject *btnSingleView;
    QObject *btnHorizontalSplit;
    QObject *btnVerticalSplit;
    QObject *btnGridSplit;

    QObject *btnWireFrame;
    QObject *btnShading;

    QQuickWidget *m_qWidget;
};
