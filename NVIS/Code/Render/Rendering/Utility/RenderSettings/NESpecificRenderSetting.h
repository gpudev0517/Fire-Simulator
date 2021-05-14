#pragma once


class NERenderSetting;
class NESceneManager;

class NESpecificRenderSetting : public QObject
{
    Q_OBJECT

public:
    NESpecificRenderSetting();
    NESpecificRenderSetting(NERenderSetting* common);
    virtual ~NESpecificRenderSetting();

    virtual void init()=0;
    void setRenderPath(QString path);
    //! Returns the render command
    virtual QString getRenderCommand(QString filePath);
    virtual QString getBatchRenderCommand(QString filePath);

    //! Returns the include file directive for the current renderer and the fileName
    virtual QString includeFile(QString fileName);

    virtual QString outputFileName(const int frameNr)= 0;
    virtual QString outputImageName() const;
    void setOutputImageName(QString name);

    uint fileType() const;
    void setFileType(const uint val);
    uint possibleFileTypes();
    virtual QString createStaticIncludes(QString path);
    QValidator* validator(int index);

    void addValidator(QString propertyName, QVariant min, QVariant max);

protected:	
    QString m_RenderExePath;
    QString m_OutputImageName;
    QString m_OutputImagePrefix;
    //!
    uint m_FileType;
    //!
    uint m_PossibleFileTypes;

    NERenderSetting* m_pCommonSettings;

    QMap<int,QValidator*> m_ValidatorMap;
};



