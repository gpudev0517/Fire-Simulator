#include "Rendering/Resource/Material/NEToneMaterial.h"

NEToneMaterial::NEToneMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setShininess( 2.0f );

    setShaderProgram(new QOpenGLShaderProgram());

    //shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/tone.frag");
    //std::cout << shaderProgram()->log().toStdString() << std::endl;

    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/tone.frag") == false )
    {
        qWarning() << " tone.frag : " << defaultFS()->log();
        std::cerr << " tone.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());

    setBlueShift( 0.4 );
    setBlueLuminance( 0.4 );
    setYellowShift( 0.2 );
    setYellowLuminance( 0.6 );
}

NEToneMaterial::~NEToneMaterial()
{
    // To prevent crashes on Mac
    if( !NeutrinoInitialized() )
        return;

    shaderProgram()->removeAllShaders();
}

void NEToneMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();

    shaderProgram()->setUniformValue( "blueShift", blueShift() );
    shaderProgram()->setUniformValue( "blueLuminance", blueLuminance() );
    shaderProgram()->setUniformValue( "yellowShift", yellowShift() );
    shaderProgram()->setUniformValue( "yellowLuminance", yellowLuminance() );
}

float NEToneMaterial::blueShift() const
{
    return m_blueShift;
}

float NEToneMaterial::blueLuminance() const
{
    return m_blueLuminance;
}

float NEToneMaterial::yellowShift() const
{
    return m_yellowShift;
}

float NEToneMaterial::yellowLuminance() const
{
    return m_yellowLuminance;
}

float NEToneMaterial::shininess() const { return m_Shininess; }

void NEToneMaterial::setShininess(float val) { m_Shininess = val; }

void NEToneMaterial::setBlueShift(float blueShift)
{
    m_blueShift = blueShift;
}

void NEToneMaterial::setBlueLuminance(float blueLuminance)
{
    m_blueLuminance = blueLuminance;
}

void NEToneMaterial::setYellowShift(float yellowShift)
{
    m_yellowShift = yellowShift;
}

void NEToneMaterial::setYellowLuminance(float yellowLuminance)
{
    m_yellowLuminance = yellowLuminance;
}


NEToneMaterialFactory::NEToneMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEToneMaterialFactory::~NEToneMaterialFactory() {}

QString NEToneMaterialFactory::nodeName() { return "Tone Material"; }

uint NEToneMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEToneMaterialFactory::nodeInherits() { return "Materials";}

QString NEToneMaterialFactory::objectName() { return "Tone"; }

QString NEToneMaterialFactory::nodeVersion() { return "0.1"; }

QString NEToneMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEToneMaterialFactory::nodeHelp() { return "Tone material"; }

NEObject*NEToneMaterialFactory::createInstance() { return new NEToneMaterial(this); }
