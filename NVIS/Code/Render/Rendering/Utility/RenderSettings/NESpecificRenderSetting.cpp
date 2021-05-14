#include "NESpecificRenderSetting.h"



NESpecificRenderSetting::NESpecificRenderSetting()
{

}

NESpecificRenderSetting::NESpecificRenderSetting(NERenderSetting* common) { }

NESpecificRenderSetting::~NESpecificRenderSetting()
{
    for(QValidator* v : m_ValidatorMap)
    {
        delete v;
    }
}

void NESpecificRenderSetting::setRenderPath(QString path)
{
    m_RenderExePath = path;
}

QString NESpecificRenderSetting::outputImageName() const
{
    return m_OutputImageName;
}

void NESpecificRenderSetting::setOutputImageName(QString name)
{
    m_OutputImageName = name;
}

uint NESpecificRenderSetting::fileType() const
{
    return m_FileType;
}

void NESpecificRenderSetting::setFileType(const uint val)
{
    m_FileType = val;
}

uint NESpecificRenderSetting::possibleFileTypes()
{
    return m_PossibleFileTypes;
}

QValidator* NESpecificRenderSetting::validator(int index)
{
    QMap<int, QValidator*>::const_iterator i = m_ValidatorMap.find(index);
    if(i != m_ValidatorMap.end() && i.key() == index)
        return i.value();
    else
        return NULL;
}

void NESpecificRenderSetting::addValidator(QString propertyName, QVariant min, QVariant max)
{
    int index = metaObject()->indexOfProperty(propertyName.toUtf8());
    if(index == -1)
        return;
    uint type = metaObject()->property(index).type();
    switch(type)
    {
    case QVariant::Int:
    case QVariant::UInt:
    {
        //QIntValidator* validator = new QIntValidator(min.toInt(),max.toInt(),this);
        QIntValidator* validator = new QIntValidator();
        validator->setRange(min.toInt(), max.toInt());
        m_ValidatorMap.insert(index, validator);
    }
        break;
    case QVariant::Double:
    {
        QDoubleValidator* validator = new QDoubleValidator();
        validator->setRange(min.toDouble(), max.toDouble(), 4);
        m_ValidatorMap.insert(index, validator);
    }
        break;
    }
}

QString NESpecificRenderSetting::getRenderCommand(QString filePath)
{
    return "";
}

QString NESpecificRenderSetting::getBatchRenderCommand(QString filePath)
{
    return "";
}

QString NESpecificRenderSetting::includeFile(QString fileName)
{
    return "";
}

QString NESpecificRenderSetting::createStaticIncludes(QString path)
{
    return "";
}
