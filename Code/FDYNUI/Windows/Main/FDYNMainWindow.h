#pragma once

NE_FWD_DECL_5(class, FDYNGUIManager, FDYNDockWidget, FDYNTimeline, QSplashScreen, FDYNAboutDialog)
NE_FWD_DECL_3(class, FDYNSceneLoadingDialog, NE_PRIVATE(FDYNMainWindow), FDYNMenu)
NE_FWD_DECL_6(class, FDYNPropertyEditorWindow, FDYNSceneInspectorWindow, FDYNSystemInspectorWindow, FDYNDockWidget, FDYNCADModelingTool, NEMessageWindow)
NE_FWD_DECL_1(class, FDYNZoneInspectorWindow)
NE_FWD_DECL_1(class, FDYNScenarioInspectorWindow)
NE_FWD_DECL_1(class, FDYNSettingsDialog)
NE_FWD_DECL_1(class, FDYNFRANXCalcDialog)

NE_FWD_DECL_1(class, FDYNShelfBar)
NE_FWD_DECL_1(class, FDYNMessageWindow)
NE_FWD_DECL_6(class, FDYNLoadingDialog, FDYNSurfaceMaterialDialog, FDYNFireMaterialDialog, FDYNFailureCriteriaDialog, FDYNVentCriteriaDialog, FDYNCableDialog)
NE_FWD_DECL_1(class, FDYNFRANXInspectorWindow)

/*! \brief NEMainWindow class is responsible for initializing the  main window.
*/


class NECEXP_FDYNUI FDYNMainWindow : public QMainWindow
{
	Q_OBJECT

public:
    FDYNMainWindow(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~FDYNMainWindow();
	void setWindowTitle(QString scenePath);

    FDYNLoadingDialog *loadingWidget() const;

    FDYNPropertyEditorWindow* propertyEditorWindow();
    FDYNTimeline* Timeline() const;
    //QToolBar *timelineToolbar();

    QMenu* creationMenu(uint nodeType);

    void registerRemoveAction(QAction* a);

	//! Locks the docks in the given area
	void lockDocks(Qt::DockWidgetArea area);
	//! Unlocks the docks in the given area
	void unlockDocks(Qt::DockWidgetArea area);

    FDYNDockWidget *sceneInspectorDock();
    FDYNSceneInspectorWindow *sceneInspectorWindow();

    FDYNDockWidget *zoneInspectorDock();
    FDYNZoneInspectorWindow* zoneInspectorWindow();
    FDYNFRANXInspectorWindow* franxInspectorWindow();

    QDockWidget *CADDock();
    FDYNCADModelingTool *CADModelingTool();

    FDYNDockWidget *scenarioInspectorDock();
    FDYNScenarioInspectorWindow* scenarioInspectorWindow();

    FDYNSystemInspectorWindow* systemInspectorWindow();

    FDYNMessageWindow* messageWindow();

    FDYNSurfaceMaterialDialog* surfaceMaterialDialog();
    FDYNFireMaterialDialog* fireMaterialDialog();
    FDYNFailureCriteriaDialog* failureCriteriaDialog();
    FDYNVentCriteriaDialog* ventCriteriaDialog();
    FDYNCableDialog *cableMaterialDialog();
    FDYNSettingsDialog *settingsDialog();
    FDYNFRANXCalcDialog *franxCalcDialog();

    void connectGLDockWidgetToAtcion(FDYNDockWidget *glDock);

    void removeSelectedItem();


public slots:
	bool newScene();
	void open(QString filePath = QString());

	void openRecentFile();
	bool save();

	bool saveAs();

	void dropEvent(QDropEvent* event);

	void about();
	//! Exports the current frame data with the scene file as a new scene
	bool exportCurrentFrame();
	//! Renders the current frame with the specified external ray tracer / renderer.
	void renderCurrentFrame();
	//! Transforms the sequence (min to max frame) into a format readable by the specified external ray tracer / renderer.
	void renderSequence();
	void renderDeleteSequence();

    void showLoadingWidget();
    void showPropertyShortCutDlg(QWidget* glWidget, QObject* spatial, uint mode);
    void showPropertyShortCutDlg(QWidget* glWidget);
    void updateWindowVisibilityFlags();

	void hideEverythingForStartup();
	//! Hides all windows
	void hideAllWindows();
	//! Shows all windows
	void showAllWindows();
	//! For maximizing the OpenGL window under cursor using spacebar
	void maximizeGLWindowUnderCursor();
    //! Show Main GL windows
    void showGlWindow();

    void selectCursorMode();
    void selectTranslationMode();
    void selectRotationMode();
    void selectScaleMode();

    void setCADModelingMode();
    void exitCADModelingMode();

    void checkGLInspectorAct(bool checked);

    QDockWidget *topDockWidget();
    void setTopDockWidget(QDockWidget *activatedWidget);




protected:
	//! Hanled when the user closes the windows
	void closeEvent(QCloseEvent* event);
	//! Handles when the user presses a key
	void keyPressEvent(QKeyEvent* event);
	//! Handled when the user resizes the main window
	void resizeEvent(QResizeEvent* event);

private slots:
	void resizeTimeout();
	void on_actionDuplicate_triggered();

private:

    NE_DECL_PRIVATE(FDYNMainWindow)

};
