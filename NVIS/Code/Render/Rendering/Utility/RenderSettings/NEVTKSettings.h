#pragma once

#include "Rendering/Utility/RenderSettings/NESpecificRenderSetting.h"

class NERenderSetting;

/*!
    VTK settings.
*/
class NEVTKSettings : public NESpecificRenderSetting
{
    Q_OBJECT

public:

    NEVTKSettings();
    NEVTKSettings(NERenderSetting* common);
    ~NEVTKSettings();

    virtual void init();

    virtual QString includeFile(QString fileName)
    {
        return QString ("");
    }
    //! Returns the render command
    virtual QString getRenderCommand(QString filePath);
    virtual QString getBatchRenderCommand(QString filePath);

    virtual QString createStaticIncludes(QString path)
    {
        return QString ("");
    }

    virtual QString outputFileName(const int frameNr);


protected:

};
