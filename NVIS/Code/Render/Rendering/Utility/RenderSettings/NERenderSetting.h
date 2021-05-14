#pragma once

#include "Utility/FilePath.h"

class NECamera;
class NEImageViewer;
class NEFrameExportThread;
class NESpecificRenderSetting;
/*!
        Class stores render settings for external renderers.
        Stores common informations and a map of specific render settings for e.g. mental ray, povray.
*/
class NECEXP_NERENDERING NERenderSetting : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("ClassName", "General Render Settings")

    Q_PROPERTY(NEExternalRendererType Renderer READ Renderer WRITE SetRenderer RESET ResetRenderer)
    Q_CLASSINFO("name-Renderer", "Renderer")
    Q_CLASSINFO("help-Renderer", "Sets the current renderer.")


    Q_PROPERTY(FilePath OutputDir READ OutputDir WRITE SetOutputDir RESET ResetOutputDir)
    Q_CLASSINFO("name-OutputDir", "Render Output Directory")
    Q_CLASSINFO("help-OutputDir", "The camera to look through.")

    Q_PROPERTY(QString Camera READ Camera WRITE SetCamera RESET ResetCamera)
    Q_CLASSINFO("name-Camera", "Camera")
    Q_CLASSINFO("help-Camera", "The camera to look through.")

    Q_PROPERTY(QSize Resolution READ Resolution WRITE SetResolution RESET ResetResolution)
    Q_CLASSINFO("name-Resolution", "Resolution")
    Q_CLASSINFO("help-Resolution", "Sets the image resolution of the renderer.")

    Q_PROPERTY(int Threads READ Threads WRITE SetThreads RESET ResetThreads)
    Q_CLASSINFO("name-Threads", "Threads")
    Q_CLASSINFO("help-Threads", "Sets the number of threads.")

    Q_PROPERTY(double FrameRate READ FrameRate WRITE SetFrameRate RESET ResetFrameRate)
    Q_CLASSINFO("name-FrameRate", "Frame Rate")
    Q_CLASSINFO("help-FrameRate", "Sets frame rate of recording movie.")

    Q_PROPERTY(double SpeedUp READ SpeedUp WRITE SetSpeedUp RESET ResetSpeedUp)
    Q_CLASSINFO("name-SpeedUp", "Speed Up")
    Q_CLASSINFO("help-SppedUp", "Sets Speed of recorded movie")

    Q_PROPERTY(bool ToggleSeparate READ ToggleSeparate WRITE SetToggleSeparate RESET ResetToggleSeparate)
    Q_CLASSINFO("name-ToggleSeparate", "Separate/Merge")
    Q_CLASSINFO("help-ToggleSeparate", "Toggle Separate/Merge movie recording in multiple cameras.")

    Q_ENUMS(NEExternalRendererType OutputFileType)

public:
    NERenderSetting();


    enum NEExternalRendererType
    {
        OpenGL = 0,
        Vtk = 1,
        PovRay = 2
    };

    NECamera* camera() const;
    void setCamera(NECamera* cam);
    //! Returns the include file directive for the current renderer and the fileName
    QString includeFile(QString fileName);
    //! Returns the render command
    QString renderFrame(QString path, bool inBatchFile=false);
    NESpecificRenderSetting* specificSetting();
    void setspecificSetting(NEExternalRendererType Renderer);
    uint possibleFileTypes();
    QString staticIncludes(QString outDir);
    QString outputFormatToString(uint type) const;
    const QMap<NEExternalRendererType, NESpecificRenderSetting*>& availableSettings();
    QString outputFileName(const int frameNr);

public slots:


    virtual void init(QObject* parent);
    void deInit();


    NEExternalRendererType Renderer();
    uint SetRenderer(const NEExternalRendererType val);
    void ResetRenderer();
    const FilePath& OutputDir() const;
    uint SetOutputDir(const FilePath& path);
    void ResetOutputDir();
    int Threads() const;
    uint SetThreads(int val);
    void ResetThreads();
    QSize Resolution() const;
    uint SetResolution(QSize val);
    void ResetResolution();
    QString Camera() const;
    void SetCamera(QString arg);
    void ResetCamera();

    double FrameRate() const;
    uint SetFrameRate(double val);
    void ResetFrameRate();

    double SpeedUp() const;
    uint SetSpeedUp(double val);
    void ResetSpeedUp();

    bool ToggleSeparate() const;
    uint SetToggleSeparate(bool val);
    void ResetToggleSeparate();

    void renderFinished(int exitCode, QProcess::ExitStatus exitStatus);
    QProcess* renderProcess() const;

signals:
    void RendererChanged(NEExternalRendererType);
    void OutputDirChanged(const FilePath&);
    void CameraChanged(QString);
    void ResolutionChanged(QSize);
    void ThreadsChanged(int);
    void FrameRateChanged(double);
    void SpeedUpChanged(double);
    void ToggleSeparateChanged(bool);

private:
    uint loadExternalRenderers();
    NEExternalRendererType m_Renderer;
    //!The folder path were the rendered images are stored.
    FilePath m_OutputDir;
    //! Pointer to the camera for rendering
    NECamera* m_pCamera;
    //! Number of threads used for rendering
    int m_Threads;
    QSize m_Resolution;
    QString m_Camera;
    double m_FrameRate;
    double m_SpeedUp;
    bool m_ToggleSeparate;
    //! Map that stores renderer specific settings
    QMap<NEExternalRendererType, NESpecificRenderSetting*> m_SpecificSettings;
    //! Stores the pointer to the current render settings object
    NESpecificRenderSetting* m_CurrentSetting;
    NEImageViewer* m_pImageViewer;
    QTimer* m_pTimer;
    bool m_UseImfDisp;
    bool m_ImfDispOpen;
    QProcess* m_pRenderProcess;
};

