#pragma once

#include "NEObject.h"
#include "Utility/FilePath.h"

NE_FWD_DECL_3(class, FilePath, NEApplication, NE_PRIVATE(NEBase))

/*! \brief NEBase class is responsible for initializing the basic components of Neutrino.
 *
    It initializes:
    Python scripting subsystem,
    The NESceneManager, which is responsible for creating and managing of all scene items,
    The OSD subsystem, which is responsible for logging and managing and showing 2D text on the OpenGL screen.

    It handles:
    Saving and loading scene files,
    Exporting current frame,
    Scene file related information,
    Number of used threads,
*/

class NEApplication;
struct aiScene;
class NENode;
class ICallInterfaceFromBaseToGUI;

class NECEXP_CORE NEBase final : public NEObject
{
    Q_OBJECT
    NE_CLASS("Basic Settings")

    NE_PROP_RW(int, Threads, "Num. OpenMP Threads", "The number of OpenMP threads Nuetrino should use.", true,)
    NE_PROP_RW(int, ServerPort, "Server Port", "The Server Port Number to use for socket based external commands issued to Neutrino", true,)
    NE_PROP_RW(FilePath, SceneFilePath, "Scene Path", "The path of the scene file.", true,)
    NE_PROP_RW(FilePath, SaveDir, "Data Write Directory", "The directory where the newly computed data is going to be saved.", true,)
    NE_PROP_RW(FilePath, CacheDir, "Data Read Directory", "The directory where the data is going to be loaded.", true,)
    NE_PROP_RW(bool, OptimizeNetCache, "Optimize Net Cache Writes", "Speeds up writing of caches on the network file systems using bigger block size writes", true,)
    NE_PROP_RW(bool, CompressFluidCache, "Compress Fluid Caches", "Compresses Fluid Cache Writes/Reads", true,)

    Q_ENUMS(CacheVersion)

public:

    static NEBase& Instance();

    //! Initializes the application
    bool init( NEApplication* app, ICallInterfaceFromBaseToGUI* aGUIInterface );

    void resetProperties() override;

    //! Returns whether NEBase is initialized or not
    bool initialized() const;
    bool serverMode() const;
    bool guiMode() const;
    void setGuiMode(bool val);
    //! Print entry and exit of computational heavy method
    void printEntryInfo(QString objName, QString methodName);
    void printExitInfo(QString objName, QString methodName);

    bool pyShellMode() const;


    const NEApplication* app() const;

    void deInit();
    ICallInterfaceFromBaseToGUI* getGUICallInterface() const { return m_pGUICallInterface; }


public slots:
    //! Opens an existing scene file
    void open(QString filePath, QString folderName = QString());
    //! Creates a new scene file
    void create(QString filePath);
    //! Saves the scene to a scene file.
    uint save(QString fileName, QString folderName, bool copyCache = true);
    //! Overwrites the current file.
    uint save();
    //! Archives the current scene.
    uint archive(QString fileName, QString folderName, bool copyCache = true);
    //! Exports the selected object to a separate file
    uint exportSelected(QString name);
    //! Imports another scene into the current scene
    uint import(QString name, vec3f scale = vec3f(1.0, 1.0, 1.0), bool asOneObject = false);
    //! Reference another scene into the current scene
    uint reference(QString name, vec3f scale = vec3f(1.0, 1.0, 1.0));
    //! Exports the current frame of the scene.
    uint exportCurrentFrame(QString folderName);

    //!  Sets the path to currently open file
    uint SetCurrentFilePath( const FilePath& filePath );

    //! File Extension of save/open
    void SetFileExt( QString fileExt );
    const QString FileExt() const;


    //!  Returns the path to the currently open scene file
    const FilePath& CurrentFilePath();
    //!  Returns the path to the scene file
    const FilePath& SceneFilePath();
    //! Returns the stripped scene file name without path or extension
    QString sceneName();
    //! Returns the path to the data directory of the scene
    QString sceneDataPath();



    uint copyDir(QString source, QString dest);
    //! Exports everything in the scene
    uint exportAll(QString name);
    //! Saves the object's current properties as a preset to the supplied path
    uint saveObjectPreset(QObject* o, QString path);
    //! Loads the object's current properties from a preset at the supplied path
    uint loadObjectPreset(QObject* o, QString path);
    //! Quits the application with proper de-initialization
    void quit(int returnCode = 0);

    //! Localize the specific asset by copying it to inside the current scene
    //! Used by archive
    QString localize(QString filename, QString prefixDir);

signals:

    void startServer( unsigned int );
    void stopServer();

    void initUI();
    void initOSD();
    void quitApp( int returnCode );

    void showMainWindow();
    void hideSceneLoadingSplash();
    void showSceneLoadingWidget();
    void hideSceneLoadingWidget();
    void showSceneManagementDialog();
    void postInit();
    void layout();


    void readAllSimConnections( QXmlStreamReader& xmlReader,
                                QString sceneFormatVersion,
                                std::vector<QString>& m_ConnStrings,
                                std::vector<QString>& m_ConnFromStrings,
                                std::vector<QString>& m_ConnToStrings );

//    void updateInitFlagForNodeGraph(bool aInitFlag);
    void deInitUI();
    void deInitSim();
    void unloadSim();
    void cleanSimData();

    void setCurrentOperation( QString msg );
    void setCurrentSubOperation( QString msg );
    void selectObject( QObject* obj );
    void updateSceneInspector();
    void updateSystemInspector();
    void updatePropertyEditor();
    void updatePropertyEditorsAtPostInit();
    void updateAllConnectionTables();
    void updateExpressionInspector();
    void updateGLWindows();
    void updateNodeGraphEditor();
    void updateWindowVisibilityFlags();
    void updateNodeGraphEdtior();

    void closeAllPropertyEditors();
    void closeAllGLDocks();

    void loadGUIState();

    void deleteMainWindow();

    void setWindowTitle( QString title );
    void writeAllGUICreations( QXmlStreamWriter* xmlWriter );
    void writeAllGUIState( QXmlStreamWriter* xmlWriter );
    void saveGUIState();

    void setParentUI( QObject* varRoot );

    void importAssScene( const aiScene *scene );
//    void updateNodeRename( NENode *node );

    void createGLWindow( QString name );
    void createGLWindowOnLoad( QString name );
    void createPropertyEditor( QString name );
//    void restoreGUIState( QXmlStreamReader* xmlReader );

private:
    friend int main(int, char**);
    ICallInterfaceFromBaseToGUI *m_pGUICallInterface;
    NE_DECL_PRIVATE_SINGLETON(NEBase)
};

#define NEBASE NEBase::Instance()
