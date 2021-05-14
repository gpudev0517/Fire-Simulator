#include "Rendering/Utility/RenderSettings/NE3DeLightSettings.h"
#include "Rendering/Utility/RenderSettings/NERenderSetting.h"
#include "Rendering/Manager/NERenderManager.h"

NE_PROP_DEF(bool,NE3DeLightSettings,MotionBlur,false)

NE3DeLightSettings::NE3DeLightSettings()
{
    m_MotionBlur = NE_DEFAULT(MotionBlur);
    init();
}

NE3DeLightSettings::NE3DeLightSettings(NERenderSetting* common)
{
    m_MotionBlur = NE_DEFAULT(MotionBlur);
    m_pCommonSettings = common;
    setParent(common);
    setObjectName("ThreeDeLightSettings");
    init();
}

NE3DeLightSettings::~NE3DeLightSettings()
{
}

const bool& NE3DeLightSettings::MotionBlur() const
{
    return m_MotionBlur;
}

uint NE3DeLightSettings::SetMotionBlur(const bool &val)
{
    NE_PROP_SET_AND_NOTIFY(MotionBlur,this,val);
    return NE::kReturnSuccess;
}

void NE3DeLightSettings::init()
{
    m_PossibleFileTypes = 7;
    m_FileType = NERenderSetting::JPEG;
}

QString NE3DeLightSettings::outputFileName(const int frameNr)
{
    QString fileName;
    QString frameNumber;
    frameNumber.sprintf("%06d",frameNr);
    fileName = frameNumber;

    QString outputImageName = frameNumber;
    setOutputImageName(outputImageName);
    return fileName;
}
