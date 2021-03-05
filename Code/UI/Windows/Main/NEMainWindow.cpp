#include "Utilities/ganalytics.h"
#include "NEMainWindow.h"

#include "ui_FDYN.h"
#include "Managers/NEGUIManager.h"
#include "Managers/NEGUIManager_p.h"
#include "Utilities/NEGUIStyle.h"

#include "Base/NEBase.h"

#include "Widgets/Timeline/NETimeline.h"

#include "Base/NESceneManager.h"
#include "Utility/FilePath.h"
#include "Rendering/Manager/NERenderManager.h"

#include "NEGLWidget.h"
#include "Windows/GLMain/NEGLMainWindow.h"
#include "Widgets/Dock/NEDockWidget.h"
#include "Dialogs/SceneManagement/NESceneManagementDialog.h"
#include "Dialogs/About/NEAboutDialog.h"
#include "Dialogs/File/NEFileDialog.h"
#include "Widgets/Menu/NEMenu.h"
#include "Dialogs/Process/NEProcessDialog.h"
#include "Spatial/NESpatial.h"

#define MAINWINDOW_DEBUG 0

class NEMainWindowPrivate
{
public:
    NEMainWindowPrivate(NEMainWindow* s) : S(s) {}

    //! Does some tweaks on the user interface which are not possible in the Qt designer
    void tweakUI();
    //!
    bool okToContinue();

    QActionGroup* m_pTransformActions;      /**< Pointer to tranfrom actions */
    NETimeline* m_pTimeline;                /**< Pointer to the timeline */
    QTimer m_ResizeTimer;                   /**< Pointer to window resize timer */
    QSplashScreen* m_pSceneLoadingSplash;   /**< Pointer to scene loading splash */
    NEAboutDialog* m_pAboutDialog;          /**< Pointer to about dialog */
    NESceneLoadingDialog* m_pSceneLoadingDialog;
    QPixmap* splashPixmap;

    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);

    enum { MaxRecentFiles = 5 };
    void setCurrentFile(const QString &fileName);
    QString &getCurrentFile()
    {
        return m_curFile;
    }
    QAction *recentFileActs[MaxRecentFiles];
    QMenu *recentFilesMenu;
    QAction *separatorAct;
    QString m_curFile;

    GAnalytics m_tracker;
private:
    NEMainWindow* S;
};

NEMainWindow::NEMainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      ui(new Ui::GUI)
{
    P = new NEMainWindowPrivate(this);
    ui->setupUi(this);   	// Setup the user interface

    //	setWindowFlags(windowFlags() ^ Qt::WindowMaximizeButtonHint);
    // Prepare scene loading splash
    P->splashPixmap = new QPixmap(":/Neutrino/Resources/splash/neutrino-splash.png");
    P->m_pSceneLoadingSplash = new QSplashScreen( *P->splashPixmap );
    P->m_pSceneLoadingDialog = new NESceneLoadingDialog(this);

    P->m_pSceneLoadingDialog->hide();

    P->m_pAboutDialog = new NEAboutDialog( this );

    // Unset the central widget, Neutrino is completely dock based
    this->setCentralWidget(0);
    P->m_pTimeline = new NETimeline(this);   // Create the timeline widget

    P->m_pTransformActions = new QActionGroup(this);

    P->m_ResizeTimer.setParent(this);
    connect(&P->m_ResizeTimer, &QTimer::timeout, this, &NEMainWindow::resizeTimeout);

    ui->menuEdit->addAction(P->m_pTransformActions->addAction(ui->actionCursor));
    ui->menuEdit->addAction(P->m_pTransformActions->addAction(ui->actionTranslate));
    ui->menuEdit->addAction(P->m_pTransformActions->addAction(ui->actionRotate));
    ui->menuEdit->addAction(P->m_pTransformActions->addAction(ui->actionScale));

    for (int i = 0; i < NEMainWindowPrivate::MaxRecentFiles; ++i)
    {
        P->recentFileActs[i] = new QAction(this);
        P->recentFileActs[i]->setVisible( false );
        connect(P->recentFileActs[i], &QAction::triggered, this, &NEMainWindow::openRecentFile);
    }
    for (int i = 0; i < NEMainWindowPrivate::MaxRecentFiles; ++i)
        ui->menuFile->addAction( P->recentFileActs[i] );

    P->separatorAct = ui->menuFile->addSeparator();

    P->updateRecentFileActions();

    // Create the tracker
    P->m_tracker.setTrackingID("UA-108244854-1");

    //    addActions(m_pTransformActions->actions());

    P->tweakUI();

    this->setWindowTitle("");
    // Shorten the interval
    P->m_tracker.setSendInterval( 5 * 1000 );

    P->m_tracker.startSession();
}

NEMainWindow::~NEMainWindow()
{
    P->m_tracker.endSession();


    //    delete m_pSceneLoadingDialog;
    delete P->splashPixmap;
    delete P->m_pSceneLoadingSplash;
    delete ui;
    delete P;
}


void NEMainWindow::setWindowTitle(QString scene)
{
    QString arch("arch-unknown");
    QString bits("bit-unknown");
    QString compile("version-unknown");
    QString openmp("");
	QString toolset("");
#ifdef _DEBUG
    compile = QString("Debug");
#else
    compile = QString("Release");
#endif

#ifdef _OPENMP
    openmp = QString("OpenMP");
#endif

#ifdef NE_BUILD_64
    bits = QString("64-bit");
#else
    bits = QString("32-bit");
#endif

#if defined(_MSC_VER) && defined(__INTEL_COMPILER)
	if (__INTEL_COMPILER >= 1800)
		toolset = QString("ICC-18.0");
	else if (__INTEL_COMPILER >= 1700)
		toolset = QString("ICC-17.0");
#elif defined(_MSC_VER)
	if (_MSC_VER == 1900)
		toolset = QString("VC-140");
	else if (_MSC_VER >= 1910)
		toolset = QString("VC-141");
#endif


#if defined(NE_FOR_WINDOWS)
    arch = QString("Windows");
#elif defined(NE_FOR_LINUX)
    arch = QString("Linux");
#elif defined(NE_FOR_MAC)
    arch = QString("Mac");
#endif


}


bool NEMainWindow::newScene()
{
    QString fileName;
    NEFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Save scene"), tr("Scene files (*.fdyn)"), NEFileDialog::SAVE);
    if (dlg.exec() == QDialog::Accepted)
        fileName = dlg.selectedFile();
    else
        return false;

    if(!fileName.isEmpty())
    {
        bool bIsCopyCache = dlg.isCopyCache();
        QString strippedFileName = QFileInfo(fileName).baseName();
        QString path = QFileInfo(fileName).path();
        QString dataPath = dlg.dataPath();
        if (!dataPath.isEmpty() && QDir(dataPath).exists())
            path = dataPath;
        if (bIsCopyCache)
            NEBASE.save(strippedFileName, path, true);
        else
        {
            NEBASE.save(strippedFileName, path, false);
            bool bIsUseExistingCache = dlg.isUseExistingCache();
            if (bIsUseExistingCache)
            {
                FilePath oldCacheDir = NEBASE.CacheDir();
                NEBASE.SetCacheDir(oldCacheDir);
            }
            else
            {
                QString dir = QFileInfo(fileName).baseName();
                NEBASE.SetCacheDir(path + "/" + dir);
            }
        }

        P->setCurrentFile(path + "/" + strippedFileName + ".fdyn");
        NEBASE.create(fileName);
        return true;
    }
    else
        showSceneManagementDialog();

    return false;
}

void NEMainWindow::open(QString filePath)
{
    if( !filePath.isNull() && filePath == P->getCurrentFile()){
        return;
    }
    QString dataPath = QString();
    if (filePath.isNull())
    {
        QSettings settings("F Dynamics", "FDYN");
        NEFileDialog dlg(this, settings.value("lastOpenScenePath").toByteArray(), tr("Open Scene"), tr("FDYN Scene files (*.fdyn)"), NEFileDialog::OPEN);
        if (dlg.exec() == QDialog::Accepted)
            filePath = dlg.selectedFile();
        else
            return;

        dataPath = dlg.dataPath();
        NEBASE.SetThreads(dlg.openMPThreads());
        NEBASE.SetOptimizeNetCache(dlg.isOptimizeNetCache());
        NEBASE.SetCompressFluidCache(dlg.isCompressFluidCaches());
    }

    if (!filePath.isEmpty() && QFile::exists(filePath))
    {
        NEBASE.open(filePath, dataPath);
    }
    else if(NEBASE.SceneFilePath().path().isEmpty())
    {
        showSceneManagementDialog();
    }

    if (!filePath.isEmpty())
        P->setCurrentFile(filePath);
}

void NEMainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        open(action->data().toString());
}

bool NEMainWindow::save()
{
    if(NEBASE.SceneFilePath().path().isEmpty())
        return saveAs();
    else
    {
        NEBASE.save();
        return true;
    }
}

bool NEMainWindow::saveAs()
{
    QString fileName;
    NEFileDialog dlg(this, NEBASE.SaveDir().path(), tr("Save scene as"), tr("Scene files (*.fdyn)"), NEFileDialog::SAVE, QFileDialog::AnyFile );
    if (dlg.exec() == QDialog::Accepted)
        fileName = dlg.selectedFile();
    else
        return false;

    if (fileName.isEmpty())
        return false;

    bool bIsCopyCache = dlg.isCopyCache();
    QString strippedFileName = QFileInfo(fileName).baseName();
    QString path = QFileInfo(fileName).path();
    QString dataPath = dlg.dataPath();
    if (!dataPath.isEmpty() && QDir(dataPath).exists())
        path = dataPath;
    //    if (val == QMessageBox::Yes)
    if (bIsCopyCache)
        NEBASE.save(strippedFileName, path, true);
    else
    {
        NEBASE.save(strippedFileName, path, false);
        bool bIsUseExistingCache = dlg.isUseExistingCache();
        if (bIsUseExistingCache)
        {
            FilePath oldCacheDir = NEBASE.CacheDir();
            NEBASE.SetCacheDir(oldCacheDir);
        }
        else
        {
            QString dir = QFileInfo(fileName).baseName();
            NEBASE.SetCacheDir(path + "/" + dir);
        }
    }

    P->setCurrentFile(path + "/" + strippedFileName + ".fdyn");
    return true;
}

bool NEMainWindow::archive()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Archive Scene As"), NEBASE.SaveDir().path(), tr("Archive (*.zip)") );

    if( fileName.isEmpty() )
        return false;


    int val = QMessageBox::Yes;
    QString strippedFileName = QFileInfo( fileName ).baseName();
    QString path = QFileInfo(fileName).path();

    if( val == QMessageBox::Yes )
        NEBASE.archive( fileName, path, true );
    else
        NEBASE.archive( fileName, path, false );

    return true;
}

bool NEMainWindow::import()
{
    QSettings settings( "F Dynamics", "FDYN" );
    QStringList fileNames;

    NEFileDialog dlg(this, settings.value("lastOpenImportPath").toByteArray(), tr("Import"), ReferenceFilter(), NEFileDialog::IMPORT, QFileDialog::ExistingFiles);
    if (dlg.exec() == QDialog::Accepted)
        fileNames = dlg.selectedFiles();
    else
        return false;

    if( fileNames.size() > 0 )
        settings.setValue("lastOpenImportPath", QFileInfo(fileNames[0]).path());

    bool retval = true;
    for( int i = 0; i < fileNames.size(); ++i )
    {
        retval =  NEBASE.import( fileNames.at(i), vec3f(dlg.xScale(), dlg.yScale(), dlg.zScale()), dlg.isMeshesAsOneOBject());
    }

    return retval;
}

bool NEMainWindow::importFRANX()
{

    return true;
}

bool NEMainWindow::exportAll()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Export as"), NEBASE.SaveDir().path(),
                                                     tr("Neutrino (*.fdyn)") );

    if( fileName.isEmpty() )
        return false;

    return exportAll( fileName );
}

bool NEMainWindow::exportSelected()
{
    QString fileName = QFileDialog::getSaveFileName( this, tr("Export Selected as"), NEBASE.SaveDir().path(),
                                                     ReferenceFilter() );

    if( fileName.isEmpty() )
        return false;

    return exportSelected( fileName );
}


void NEMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    //
    // check for our needed mime type, here a file or a list of files
    //
    if( mimeData->hasUrls() )
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        // extract the local paths of the files
        for( int i = 0; i < urlList.size() && i < 32; ++i )
        {
            pathList.append( urlList.at(i).toLocalFile() );
            qDebug() << "File Name " << pathList[i].toUtf8() ;
            NEBASE.import( pathList[i] );
        }
    }
}


bool NEMainWindow::reference()
{
    QStringList fileNames;
    QSettings settings( "F Dynamics", "FDYN" );


    NEFileDialog dlg( this, settings.value("lastOpenImportPath").toByteArray(),
                      tr("Reference"),
                      ReferenceFilter(),
                      NEFileDialog::REFER, QFileDialog::ExistingFiles );
    if( dlg.exec() == QDialog::Accepted )
        fileNames = dlg.selectedFiles();
    else
        return false;

    if( fileNames.size() > 0 )
        settings.setValue( "lastOpenImportPath", QFileInfo( fileNames[0] ).path() );

    bool retval = true;
    for( int i = 0; i < fileNames.size(); ++i )
    {
        retval = NEBASE.reference( fileNames.at(i), vec3f(dlg.xScale(), dlg.yScale(), dlg.zScale()));
    }

    return true;

}

bool NEMainWindow::import(QString filePath)
{
    NEBASE.import( filePath);

    return true;
}

bool NEMainWindow::reference(QString filePath)
{
    NEBASE.reference( filePath );

    return true;
}

bool NEMainWindow::exportAll(QString filePath )
{
    QString strippedFileName = QFileInfo( filePath ).baseName();
    QString path = QFileInfo( filePath ).path();

    NEBASE.exportAll( filePath );

    return true;
}

bool NEMainWindow::exportSelected(QString filePath)
{
    NEBASE.exportSelected( filePath );
    return true;
}

QString NEMainWindowPrivate::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void NEMainWindowPrivate::setCurrentFile(const QString &fileName)
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll( fileName );
    if (QFile::exists( fileName ))
        files.prepend( fileName );
    while( files.size() > MaxRecentFiles )
        files.removeLast();

    settings.setValue( "recentFileList", files );


    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        NEMainWindow *mainWin = qobject_cast<NEMainWindow *>(widget);
        if (mainWin)
        {
            updateRecentFileActions();
        }
    }

    m_curFile = fileName;
}

void NEMainWindowPrivate::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin( files.size(), (int)MaxRecentFiles );

    for( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = QString("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText( text );
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }


    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {

        recentFileActs[j]->setVisible( false );
    }


    separatorAct->setVisible( numRecentFiles > 0 );
}


bool NEMainWindow::exportCurrentFrame()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())
        return false;

    NEBASE.exportCurrentFrame(dir);
    Log() << "Exporting frame " << NESCENE.CurrentFrame();
    EndLog(NE::kLogInfo);
    return true;
}

void NEMainWindow::renderCurrentFrame()
{
    QString script = NERENDERMAN.objectName()+".exportFrameToExternalRenderer()";
    NEGUI.EvalPyGUI(script);
}

void NEMainWindow::renderSequence()
{
    QString startFrame = QString::number(NEGUI.P->timeline()->playbackStartSpinBox->value());
    QString endFrame = QString::number(NEGUI.P->timeline()->playbackEndSpinBox->value());
    QString script = NERENDERMAN.objectName()+".exportSequenceToExternalRenderer("+startFrame+","+endFrame+")";
    NEGUI.EvalPyGUI(script);
}

void NEMainWindow::renderDeleteSequence()
{
    QString startFrame = QString::number(NEGUI.P->timeline()->playbackStartSpinBox->value());
    QString endFrame = QString::number(NEGUI.P->timeline()->playbackEndSpinBox->value());
    QString script = NERENDERMAN.objectName()+".exportSequenceRenderDelete("+startFrame+","+endFrame+")";
    NEGUI.EvalPyGUI(script);
}

void NEMainWindow::about()
{
    P->m_pAboutDialog->exec();
    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    QPoint dialogCenter(P->m_pAboutDialog->width(), P->m_pAboutDialog->height());
}

void NEMainWindow::closeEvent(QCloseEvent* event)
{
    P->m_tracker.endSession();

    if (P->okToContinue() || !NEBASE.initialized())
    {
        event->accept();
        NEBASE.quit();
    }
    else
        event->ignore();
}

void NEMainWindow::showSceneManagementDialog()
{
    if (!NEBASE.SceneFilePath().path().isEmpty())
        return;

    NESceneManagementDialog selectDialog(this);

    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    QPoint dialogCenter(selectDialog.width(), selectDialog.height());
    selectDialog.exec();
    selectDialog.raise();

    selectDialog.move(windowCenter-dialogCenter);
}

void NEMainWindow::showSceneLoadingWidget()
{
    QPoint windowCenter(this->width()*0.5, this->height()*0.5);
    windowCenter += this->pos();
    QPoint dialogCenter(P->m_pSceneLoadingDialog->width()*0.5, P->m_pSceneLoadingDialog->height()*0.5);

    P->m_pSceneLoadingDialog->move(windowCenter-dialogCenter);
    P->m_pSceneLoadingDialog->show();
    qApp->processEvents();
}



void NEMainWindow::showSceneLoadingSplash(QString filename)
{
    P->m_pSceneLoadingSplash->show();
    P->m_pSceneLoadingSplash->showMessage("Loading: " + filename, Qt::AlignBottom, Qt::white);

    qApp->processEvents();
}

void NEMainWindow::hideSceneLoadingSplash()
{
    P->m_pSceneLoadingSplash->close();
}

void NEMainWindow::hideEverythingForStartup()
{
    foreach(QObject* obj, children())
    {
        if(QWidget* w = qobject_cast<QWidget*>(obj))
        {
            w->hide();
        }
    }
    ui->menuBar->show();
    ui->statusBar->show();
}

void NEMainWindow::hideAllWindows()
{
    ui->actionActivateConnectionInspector->setChecked(false);
    ui->actionActivateExpressionEditor->setChecked(false);
    ui->actionActivateExpressionInspector->setChecked(false);
    ui->actionActivateZoneInspector->setChecked(false);
    ui->actionActivateMessageWindow->setChecked(false);
    ui->actionActivatePropertyEditor->setChecked(false);
    ui->actionActivateSceneBuilder->setChecked(false);
    ui->actionActivateSceneInspector->setChecked(false);
    ui->actionActivateZoneInspector->setChecked(false);
    ui->actionActivateScriptEditor->setChecked(false);
    ui->actionActivateSystemInspector->setChecked(false);
    ui->actionActivateTimeline->setChecked(false);
    ui->actionActivateGPUBufInspector->setChecked(false);
    ui->actionActivateConsole_Inspector->setChecked( false );
    ui->actionVisualize->setChecked( false );
}

void NEMainWindow::showAllWindows()
{
    ui->actionActivateConnectionInspector->setChecked(true);
    ui->actionActivateExpressionEditor->setChecked(true);
    ui->actionActivateExpressionInspector->setChecked(true);
    ui->actionActivateZoneInspector->setChecked(true);
    ui->actionActivateSceneBuilder->setChecked(true);
    ui->actionActivateSystemInspector->setChecked(true);
    ui->actionActivateTimeline->setChecked(true);
    ui->actionActivateSceneInspector->setChecked(true);
    ui->actionActivateZoneInspector->setChecked(true);
    ui->actionActivatePropertyEditor->setChecked(true);
    ui->actionActivateScriptEditor->setChecked(true);
    ui->actionActivateMessageWindow->setChecked(true);
    ui->actionActivateConsole_Inspector->setChecked( true );
    ui->actionVisualize->setChecked( true );
    ui->sceneInspectorDock->raise();
    ui->propertyEditorDock->raise();
    ui->messageDock->raise();
}

void NEMainWindow::showGlWindow()
{
    foreach(QObject* obj, children())
    {
        NEDockWidget* dock = qobject_cast<NEDockWidget*>(obj);
        if(dock)
        {

            NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(dock->containedMainWindow());
            if(glmw)
            {
                if(dock->isVisible())
                    return;
            }
        }
    }
    NEGUI.showMainGLWidget();
}

void NEMainWindow::maximizeGLWindowUnderCursor()
{
    QPoint p = mapFromGlobal(QCursor::pos());

    if(NEGLWidget* glw =  qobject_cast<NEGLWidget*>(childAt(p)))
    {
        //qDebug() << Q_FUNC_INFO;
        if(NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(glw->parentWidget()->parentWidget()))
        {
            //glmw->show();
            glmw->maximizeToggle();
        }
    }
}


bool NEMainWindowPrivate::okToContinue()
{
    if(!NEBASE.SceneFilePath().path().isEmpty())
    {
        int r = QMessageBox::warning(S, QString("FDYN"),
                                     QString("Do you want to save your changes?"),
                                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if( r == QMessageBox::Yes)
            return S->save();
        else if(r == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void NEMainWindow::resizeTimeout()
{
    P->m_ResizeTimer.stop();
    foreach(QObject* obj, children())
    {
        NEDockWidget* dock = qobject_cast<NEDockWidget*>(obj);
        if(dock)
        {
            NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(dock->containedMainWindow());
            if(!glmw)
            {
                {

                    dock->setMaximumWidth(16384);
                    dock->setMinimumWidth(50);
                }
            }
        }
    }

}

void NEMainWindow::lockDocks(Qt::DockWidgetArea area)
{
    foreach(QObject* obj, children())
    {
        NEDockWidget* dock = qobject_cast<NEDockWidget*>(obj);
        if(dock && dockWidgetArea(dock) == area)
        {
            NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(dock->containedMainWindow());
            if(!glmw)
            {
                dock->setMaximumWidth(dock->width());
                dock->setMinimumWidth(dock->width());
            }
        }
    }
}

void NEMainWindow::unlockDocks(Qt::DockWidgetArea area)
{
    foreach(QObject* obj, children())
    {
        NEDockWidget* dock = qobject_cast<NEDockWidget*>(obj);
        if(dock && dockWidgetArea(dock) == area)
        {
            NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(dock->containedMainWindow());
            if(!glmw)
            {
                dock->setMaximumWidth(16384);
                dock->setMinimumWidth(50);
            }
        }
    }
}

NEMenu *NEMainWindow::creationMenu(uint nodeType)
{
    switch(nodeType)
    {
#if NE_GRIDFLUID
    case NE::kObjectGridFluidSolver:
        return ui->menuGridFluidSolvers;
    case (NE::kObjectParticleFluidSolver + NE::kObjectExperimental):
        return ui->menuParticleFluidExperimental;
    case (NE::kObjectSecondaryParticles):
        return ui->menuSecondaryParticles;
#endif
    case (NE::kObjectMaterial):
        return ui->menuMaterials;
    case (NE::kObjectCamera):
        return ui->menuCameras;
    case (NE::kObjectLight):
        return ui->menuLights;
    //case (NE::kObjectMeshPP):
    //    return ui->menuMeshPostProcessors;
#if 0
    case (NE::kObjectMSS):
        return ui->menuMassSpringSystems;
    case (NE::kObjectParticleSand):
        return ui->menuParticleSand;
#endif
    case (NE::kObjectOSD):
        return ui->menuOSD;
    case (NE::kObjectClipPlane):
        return ui->menuVisual;
    case (NE::kObjectGizmo):
        return ui->menuGizmo;
    default:
        return ui->menuAdd;
    }
}


NESceneLoadingDialog* NEMainWindow::sceneLoadingWidget() const
{
    return P->m_pSceneLoadingDialog;
}

void NEMainWindow::updateWindowVisibilityFlags()
{
    ui->actionActivateSceneInspector->blockSignals(true);
    ui->actionActivateSceneInspector->setChecked(ui->sceneInspectorDock->isVisible());
    ui->actionActivateSceneInspector->blockSignals(false);

    ui->actionActivateZoneInspector->blockSignals(true);
    ui->actionActivateZoneInspector->setChecked(ui->sceneInspectorDock->isVisible());
    ui->actionActivateZoneInspector->blockSignals(false);

    ui->actionActivateSystemInspector->blockSignals(true);
    ui->actionActivateSystemInspector->setChecked(ui->systemInspectorDock->isVisible());
    ui->actionActivateSystemInspector->blockSignals(false);

    ui->actionActivateMessageWindow->blockSignals(true);
    ui->actionActivateMessageWindow->setChecked(ui->messageDock->isVisible());
    ui->actionActivateMessageWindow->blockSignals(false);

    ui->actionActivateSceneBuilder->blockSignals(true);
    ui->actionActivateSceneBuilder->setChecked(ui->sceneBuilderDock->isVisible());
    ui->actionActivateSceneBuilder->blockSignals(false);

    ui->actionActivatePropertyEditor->blockSignals(true);
    ui->actionActivatePropertyEditor->setChecked(ui->propertyEditorDock->isVisible());
    ui->actionActivatePropertyEditor->blockSignals(false);





}

void NEMainWindowPrivate::tweakUI()
{
    // Set the corner occupation for dock widgets
    S->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    S->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    // Tabify dock widgets

    S->ui->propertyEditorDock->raise();

    S->tabifyDockWidget(S->ui->sceneInspectorDock, S->ui->systemInspectorDock);
    S->tabifyDockWidget(S->ui->systemInspectorDock, S->ui->sceneBuilderDock);

    S->ui->sceneInspectorDock->raise();
    S->ui->messageDock->raise();

    // Add the timeline widget into the timeline toolbar
    S->ui->timelineToolbar->addWidget(m_pTimeline);


    S->ui->sceneInspectorDock->setContainedMainWindow(S->ui->sceneInspector);
    S->ui->zoneInspectorDock->setContainedMainWindow(S->ui->zoneInspector);
    S->ui->systemInspectorDock->setContainedMainWindow(S->ui->systemInspector);
    S->ui->messageDock->setContainedMainWindow(S->ui->messageWindow);
    S->ui->sceneBuilderDock->setContainedMainWindow(S->ui->sceneBuilder);
    S->ui->propertyEditorDock->setContainedMainWindow(S->ui->propertyEditor);

    S->connect(S->ui->actionActivateSceneInspector, &QAction::triggered, S->ui->sceneInspectorDock, &QWidget::setVisible);
    S->connect(S->ui->sceneInspectorDock, &NEDockWidget::dockVisible, S->ui->actionActivateSceneInspector, &QAction::setChecked);

    S->connect(S->ui->actionActivateZoneInspector, &QAction::triggered, S->ui->zoneInspectorDock, &QWidget::setVisible);
    S->connect(S->ui->zoneInspectorDock, &NEDockWidget::dockVisible, S->ui->actionActivateZoneInspector, &QAction::setChecked);

    S->connect(S->ui->actionActivateSystemInspector, &QAction::triggered, S->ui->systemInspectorDock, &QWidget::setVisible);
    S->connect(S->ui->systemInspectorDock, &NEDockWidget::dockVisible, S->ui->actionActivateSystemInspector, &QAction::setChecked);

    S->connect(S->ui->actionActivateMessageWindow, &QAction::triggered, S->ui->messageDock, &QWidget::setVisible);
    S->connect(S->ui->messageDock, &NEDockWidget::dockVisible, S->ui->actionActivateMessageWindow, &QAction::setChecked);

    S->connect(S->ui->actionActivateSceneBuilder, &QAction::triggered, S->ui->sceneBuilderDock, &QWidget::setVisible);
    S->connect(S->ui->sceneBuilderDock, &NEDockWidget::dockVisible, S->ui->actionActivateSceneBuilder, &QAction::setChecked);

    S->connect(S->ui->actionActivatePropertyEditor, &QAction::triggered, S->ui->propertyEditorDock, &QWidget::setVisible);
    S->connect(S->ui->propertyEditorDock, &NEDockWidget::dockVisible, S->ui->actionActivatePropertyEditor, &QAction::setChecked);


    S->connect(S->ui->actionActivateMainWindow, &QAction::triggered, S, &NEMainWindow::showGlWindow);

    S->connect(S->ui->actionShowAllWindows, &QAction::triggered, S, &NEMainWindow::showAllWindows);
    S->connect(S->ui->actionHideAllWindows, &QAction::triggered, S, &NEMainWindow::hideAllWindows);

    S->connect(S->ui->actionAbout, &QAction::triggered, S, &NEMainWindow::about);
    S->connect(S->ui->actionReload_Stylesheet, &QAction::triggered, &NEGUISTYLE, &NEGUIStyle::reloadAndApplyStyleSheet);

    S->connect(S->ui->actionRemove, &QAction::triggered, S->ui->sceneInspector, &NESceneInspectorWindow::removeSelectedItem);



    S->connect(S->ui->actionHelp, &QAction::triggered, &NEGUI, &NEGUIManager::showHelp);
    S->connect(S->ui->actionOpenGL_update, &QAction::toggled, &NEGUI, &NEGUIManager::setOpenGLUpdate);
    //connect(mainWindow()->actionGL_Buffer&NEGUI, SIGNAL(toggled(bool)), thi&NEGUI, SLOT(setShowGLBuffers(bool)));
    S->connect(S->ui->actionCursor, &QAction::toggled, &NEGUI, &NEGUIManager::selectCursorMode);
    S->connect(S->ui->actionTranslate, &QAction::toggled, &NEGUI, &NEGUIManager::selectTranslationMode);
    S->connect(S->ui->actionRotate, &QAction::toggled, &NEGUI, &NEGUIManager::selectRotationMode);
    S->connect(S->ui->actionScale, &QAction::toggled, &NEGUI, &NEGUIManager::selectScaleMode);
    S->connect(S->ui->actionSet_Selected_Active, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedActive);
    S->connect(S->ui->actionSet_Selected_Cache, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedCache);
    S->connect(S->ui->actionSet_Selected_Inactive, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedInactive);
    S->connect(S->ui->actionSet_Selected_Visible, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedVisibile);
    S->connect(S->ui->actionSet_Selected_Hidden, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedHidden);
    S->connect(S->ui->actionSet_Selected_Bounding_Box, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedBoundingBox);
    S->connect(S->ui->actionWireframe, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedWireframe);
    S->connect(S->ui->actionSet_Selected_Flat_Shaded, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedFlatShaded);
    S->connect(S->ui->actionSet_Selected_Smooth_Shaded, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedSmoothShaded);
    S->connect(S->ui->actionGroup, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedGroup);
    S->connect(S->ui->actionUngroup, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedUngroup);
    S->connect(S->ui->actionParent, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedParent);
    S->connect(S->ui->actionUnParent, &QAction::triggered, &NEGUI, &NEGUIManager::setSelectedUnparent);
    S->connect(S->ui->actionFull_screen, &QAction::toggled, &NEGUI, &NEGUIManager::fullscreenToggle);
    //    S->connect(S->ui->actionMaximize, &QAction::triggered, &NEGUI, &NEGUIManager::maximize);
    S->connect(S->ui->actionUndo, &QAction::triggered, &NEGUI, &NEGUIManager::undo);
    S->connect(S->ui->actionRedo, &QAction::triggered, &NEGUI, &NEGUIManager::redo);
}


void NEMainWindow::keyPressEvent( QKeyEvent* event )
{
    if(event->isAutoRepeat())
        return;

    if(event->modifiers() == Qt::ControlModifier)
    {
        //if(event->modifiers() == Qt::KeypadModifier)
        //{
        QStringList tmp;
        switch(event->key())
        {
        case Qt::Key_Right:
            NESCENE.startUpdateTimer(false);
            if(MAINWINDOW_DEBUG)
            {
                Log() << "Ctrl + Right pressed.";
                EndLog(NE::kLogInfo);
            }
            //update scene
            NESCENE.update();
            //update property editor
            NEGUI.updatePropertyEditor();
            //update gl content
            NEGUI.updateGLWindows();
            break;
        case Qt::Key_Down:
            NESCENE.startUpdateTimer(true);
            break;
        case Qt::Key_K:
            tmp.push_back("Position");
            tmp.push_back("Orientation");
            tmp.push_back("Scale");
            NESCENE.setKeyFrameForSelectedNode(tmp, NE::kLinear);
            break;
        case Qt::Key_L:
            tmp.push_back("Position");
            tmp.push_back("Orientation");
            tmp.push_back("Scale");
            NESCENE.setKeyFrameForSelectedNode(tmp, NE::kSinusoidal);
            break;


        }
    }
    else
    {
        switch(event->key())
        {
        case Qt::Key_Escape:
            if(NEGUI.P->timeline()->playButton->isChecked())
            {
                NEGUI.P->timeline()->playButton->toggle();
                Log() << "Simulation/Playback stopped";
                EndLog(NE::kLogInfo);
            }
            break;

        case Qt::Key_Space:
            maximizeGLWindowUnderCursor();
            break;

        }
    }
    event->ignore();

}

void NEMainWindow::resizeEvent(QResizeEvent *event)
{
    if(!NEBASE.initialized())
    {
        return QMainWindow::resizeEvent(event);
    }

    P->m_ResizeTimer.start(100);

    foreach(QObject* obj, children())
    {
        NEDockWidget* dock = qobject_cast<NEDockWidget*>(obj);
        if(dock)
        {
            NEGLMainWindow* glmw = qobject_cast<NEGLMainWindow*>(dock->containedMainWindow());
            if(!glmw)
            {
                {
                    dock->setMaximumWidth(dock->width());
                    dock->setMinimumWidth(dock->width());
                }
            }
        }
    }
    QMainWindow::resizeEvent(event);
}

void NEMainWindow::on_actionDuplicate_triggered()
{
    NESCENE.duplicateSelectedObjects();
}

NETimeline* NEMainWindow::Timeline() const
{
    return P->m_pTimeline;
}

NEShelfBar*NEMainWindow::shelfBar()
{
    return ui->shelfBar;
}


NEPropertyEditorWindow* NEMainWindow::propertyEditorWindow()
{
    return ui->propertyEditor;
}

NESceneInspectorWindow* NEMainWindow::sceneInspectorWindow()
{
    return ui->sceneInspector;
}

NESystemInspectorWindow* NEMainWindow::systemInspectorWindow()
{
    return ui->systemInspector;
}

FZoneInspectorWindow *NEMainWindow::fZoneInspectorWindow()
{
    return ui->zoneInspector;
}



NESceneBuilderWindow*NEMainWindow::sceneBuilder()
{
    return ui->sceneBuilder;
}


NEMessageWindow* NEMainWindow::messageWindow()
{
    return ui->messageWindow;
}



NEDockWidget* NEMainWindow::messageDock()
{
    return ui->messageDock;
}

NEDockWidget*NEMainWindow::sceneInspectorDock()
{
    return ui->sceneInspectorDock;
}

NEDockWidget*NEMainWindow::propertyEditorDock()
{
    return ui->propertyEditorDock;
}


void NEMainWindow::registerRemoveAction(QAction* a)
{
    connect(a, &QAction::triggered, ui->actionRemove, &QAction::triggered);
}

