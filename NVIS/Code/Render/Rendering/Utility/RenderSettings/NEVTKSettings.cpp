#include "Rendering/Utility/RenderSettings/NEVTKSettings.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"


NEVTKSettings::NEVTKSettings()
{
    init();
}

NEVTKSettings::NEVTKSettings(NERenderSetting* common)
{
    m_pCommonSettings = common;
    setParent(common);
    setObjectName("VTKSettings");
    init();
}


NEVTKSettings::~NEVTKSettings()
{
}

void NEVTKSettings::init()
{
    m_PossibleFileTypes = 7;
    m_FileType = NERenderSetting::PNG;

}

/*
    For each property that should have a range, define a validator here.
*/




/*!
	Returns the render command
*/
QString NEVTKSettings::getBatchRenderCommand(QString filePath)
{
    QString  command("");
    return command;
}

/*!
    Returns the render command
*/
QString NEVTKSettings::getRenderCommand(QString filePath)
{
    QString  command(m_RenderExePath);
    return command;
}

QString NEVTKSettings::outputFileName(const int frameNr)
{
    QString fileName;
    QString frameNumber;
    frameNumber.sprintf("%06d", frameNr);
    fileName = frameNumber + ".vtk";

    QString outputImageName = frameNumber;
    setOutputImageName(outputImageName);

    return fileName;
}
