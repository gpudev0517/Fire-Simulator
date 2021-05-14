#pragma once

#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"
#include "Core/Utility/NEMocDefs.h"

class NERenderSetting;

class NE3DeLightSettings : public NESpecificRenderSetting
{
    Q_OBJECT

    NE_PROP_RW(bool,MotionBlur,"Motion Blur","Enable motion blur",true,)

public:
    NE3DeLightSettings();
    NE3DeLightSettings(NERenderSetting* common);
    ~NE3DeLightSettings();
    virtual void init();
    virtual QString outputFileName(const int frameNr);

private:
    bool m_MotionBlur;
};
