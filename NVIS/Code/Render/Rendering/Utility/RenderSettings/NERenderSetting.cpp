#include "NERenderSetting.h"
#include "Base/NEBase.h"
#include "Utility/Threads/NEFrameExportThread.h"
#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"
#include "Rendering/Utility/RenderSettings/NEPovRaySettings.h"
#include "Rendering/Utility/RenderSettings/NEOpenGLSettings.h"
#if NE_VTK
#include "Rendering/Utility/RenderSettings/NEVTKSettings.h"
#endif
#include "Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"

NE_PROP_DEF(NERenderSetting::NEExternalRendererType, NERenderSetting, Renderer, NERenderSetting::OpenGL)
NE_PROP_DEF(FilePath, NERenderSetting, OutputDir, QString("RenderData/"))
NE_PROP_DEF(QString, NERenderSetting, Camera, QString("CamPersp"))
NE_PROP_DEF(QSize, NERenderSetting, Resolution, QSize(720, 480))
NE_PROP_DEF(int, NERenderSetting, Threads, 8)
NE_PROP_DEF(double, NERenderSetting, FrameRate, 25.0)
NE_PROP_DEF(double, NERenderSetting, SpeedUp, 1.0)
NE_PROP_DEF(double, NERenderSetting, ToggleSeparate, true)

//class NERenderSettingPrivate
//{
//	NERenderSettingPrivate(NERenderSetting)
//};

static void initProperties()
{
    NE_INIT_PROP(NERenderSetting, Renderer);
    NE_INIT_PROP(NERenderSetting, OutputDir);
    NE_INIT_PROP(NERenderSetting, Camera);
    NE_INIT_PROP(NERenderSetting, Resolution);
    NE_INIT_PROP(NERenderSetting, Threads);
    NE_INIT_PROP(NERenderSetting, FrameRate);
    NE_INIT_PROP(NERenderSetting, SpeedUp);
    NE_INIT_PROP(NERenderSetting, ToggleSeparate);
}

NERenderSetting::NERenderSetting()
{
    QObject::connect( &NESCENE, SIGNAL(initRenderSettings(QObject *)), this, SLOT(init(QObject *)), Qt::DirectConnection );
    QObject::connect( &NESCENE, SIGNAL(deInitRenderSettings()), this, SLOT(deInit()), Qt::DirectConnection );
}


void NERenderSetting::init(QObject* parent)
{
    CALL_ONCE(initProperties);

    setParent(parent);
    setObjectName("RenderSettings");
    loadExternalRenderers();

    SetRenderer( NE_DEFAULT(Renderer) );
    m_OutputDir.setDialogType(FilePath::DialogType::kOpenPathType);

    m_Camera = NE_DEFAULT(Camera);
    m_Resolution = NE_DEFAULT(Resolution);
    m_Threads = NEBASE.Threads();
    m_FrameRate = NE_DEFAULT(FrameRate);
    m_SpeedUp = NE_DEFAULT(SpeedUp);
    m_ToggleSeparate = NE_DEFAULT(ToggleSeparate);
    QDir().mkpath(NESCENE.renderDataSaveDirectory());
    m_OutputDir.setPath(NE_DEFAULT(OutputDir));
    //m_pImageViewer = new NEImageViewer(NULL);
    m_pRenderProcess = new QProcess();

    setCamera( NERENDERMAN.defaultCamera() );
}

void NERenderSetting::deInit()
{
    //delete m_pImageViewer;
    delete m_pRenderProcess;
}


NERenderSetting::NEExternalRendererType NERenderSetting::Renderer()
{
    return m_Renderer;
}

int NERenderSetting::Threads() const
{
    return m_Threads;
}

uint NERenderSetting::SetThreads(int val)
{
    NE_PROP_SET_AND_NOTIFY(Threads, this, val);
    return NE::kReturnSuccess;
}

double NERenderSetting::FrameRate() const
{
    return m_FrameRate;
}

uint NERenderSetting::SetFrameRate(double val)
{
    NE_PROP_SET_AND_NOTIFY(FrameRate, this, val);
    return NE::kReturnSuccess;
}

double NERenderSetting::SpeedUp() const
{
    return m_SpeedUp;
}

uint NERenderSetting::SetSpeedUp(double val)
{
    NE_PROP_SET_AND_NOTIFY(SpeedUp, this, val);
    return NE::kReturnSuccess;
}

bool NERenderSetting::ToggleSeparate() const
{
    return m_ToggleSeparate;
}
uint NERenderSetting::SetToggleSeparate(bool val)
{
    NE_PROP_SET_AND_NOTIFY(ToggleSeparate, this, val);
    return NE::kReturnSuccess;
}

NECamera* NERenderSetting::camera() const
{
    return m_pCamera;
}

void NERenderSetting::setCamera(NECamera* cam)
{
    m_pCamera = cam;
}

QString NERenderSetting::includeFile(QString fileName)
{
    return m_CurrentSetting->includeFile(fileName);
}


QString NERenderSetting::renderFrame(QString path, bool inBatchFile)
{
    QString imagePath = m_OutputDir.path() + m_CurrentSetting->outputImageName() + "." + "png";
    if(!inBatchFile)
    {
        //m_pImageViewer->clear();
        //m_pImageViewer->setFileFormat(fileFormat);
        //m_pImageViewer->setFilePath(imagePath);
    }
    //connect(pThread, SIGNAL(renderedSuccessfully()), m_pImageViewer, SLOT(refreshImage()));

    QString command;

    //
    if(!inBatchFile)
    {
        command = m_CurrentSetting->getRenderCommand(path);
        m_pRenderProcess->start(command);

        connect(m_pRenderProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(renderFinished(int, QProcess::ExitStatus)));
    }
    else
    {
        command = m_CurrentSetting->getBatchRenderCommand(path);
    }

    return command;
}

NESpecificRenderSetting* NERenderSetting::specificSetting()
{
    return m_CurrentSetting;
}

void NERenderSetting::renderFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    disconnect(m_pRenderProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(renderFinished(int, QProcess::ExitStatus)));
    if(exitCode == 0)
    {
        //m_pImageViewer->stopImf();
        Log() << "Rendering successfully finished!";
        EndLog(NE::kLogInfo);
        //m_pImageViewer->refreshImage();
        //m_pImageViewer->showImage();
    }
    else
    {
        Log() << "Rendering failed:";
        EndLog(NE::kLogWarning);
        Log() << m_pRenderProcess->readAllStandardError();
        EndLog(NE::kLogWarning);
    }

}

QProcess* NERenderSetting::renderProcess() const
{
    return m_pRenderProcess;
}

uint NERenderSetting::SetResolution(QSize val)
{
    NE_PROP_SET_AND_NOTIFY(Resolution, this, val);
    return NE::kReturnSuccess;
}

void NERenderSetting::setspecificSetting(NEExternalRendererType renderer)
{
    if(m_SpecificSettings.find(renderer) != m_SpecificSettings.end())
        m_CurrentSetting = m_SpecificSettings.find(renderer).value();
    else
        m_CurrentSetting = NULL;
}

/*!
*/
uint NERenderSetting::SetRenderer(const NEExternalRendererType val)
{
    NERenderSetting::NEExternalRendererType newVal = val;
    if(val > 2 || val < 0)
        newVal = NERenderSetting::OpenGL;
    NE_PROP_SET(Renderer, this, newVal);
    setspecificSetting(m_Renderer);
    emit RendererChanged(newVal);
    return NE::kReturnSuccess;
}

const FilePath& NERenderSetting::OutputDir() const
{
    return m_OutputDir;
}

uint NERenderSetting::SetOutputDir(const FilePath& path)
{
    NE_PROP_SET_AND_NOTIFY(OutputDir, this, const_cast<FilePath&>(path));
    return NE::kReturnSuccess;
}

/*!
        Parses the preferences file and creates specific render settings instances for the found renderer.
*/
uint NERenderSetting::loadExternalRenderers()
{
    NEPovRaySettings* pPovRay = new NEPovRaySettings(this);
    m_SpecificSettings.insert(PovRay, pPovRay);
    pPovRay->setRenderPath("");

    NEOpenGLSettings* pOpenGL = new NEOpenGLSettings(this);
    m_SpecificSettings.insert(OpenGL, pOpenGL);
    pOpenGL->setRenderPath("");

#if NE_VTK
    NEVTKSettings* pVTK = new NEVTKSettings(this);
    m_SpecificSettings.insert(VTK, pVTK);
#endif
    if(m_SpecificSettings.empty())
        return NE::kReturnFileNotFound;

    return NE::kReturnSuccess;
}

uint NERenderSetting::possibleFileTypes()
{
    return m_CurrentSetting->possibleFileTypes();
}

QString NERenderSetting::staticIncludes(QString outDir)
{
    return m_CurrentSetting->createStaticIncludes(outDir);
}




const QMap<NERenderSetting::NEExternalRendererType, NESpecificRenderSetting*>& NERenderSetting::availableSettings()
{
    return m_SpecificSettings;
}


QSize NERenderSetting::Resolution() const
{
    return m_Resolution;
}

QString NERenderSetting::Camera() const
{
    return m_Camera;
}

void NERenderSetting::SetCamera(QString arg)
{
    if(!m_Camera.isEmpty() && m_Camera == arg)
        return;

    if(NERENDERMAN.cameras().contains(arg))
    {
        m_Camera = arg;
        emit CameraChanged(arg);
        return;
    }
    return;
}

QString NERenderSetting::outputFileName(const int frameNr)
{
    return m_CurrentSetting->outputFileName(frameNr);
}
