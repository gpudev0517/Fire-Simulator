#include "Rendering/Resource/Material/NETransparentMaterial.h"


NETransparentMaterial::NETransparentMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    SetShininess( 400.0f );

    setShaderProgram( new QOpenGLShaderProgram() );
    shaderProgram()->addShaderFromSourceFile( QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/phong.frag" );
    //shaderProgram()->shaders()[shaderProgram()->shaders().size() - 1]->setObjectName("Phong FS");

    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/phong.frag") == false )
    {
        qWarning() << " phong.frag : " << defaultFS()->log();
        std::cerr << " phong.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());


    SetColor( QColor(255,255,255,255) );

}

NETransparentMaterial::~NETransparentMaterial()
{
    shaderProgram()->removeAllShaders();
}


void NETransparentMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("Shininess", (float)Shininess());
}

const double& NETransparentMaterial::Shininess() const { return m_Shininess; }
uint NETransparentMaterial::SetShininess(const double& val) { m_Shininess = val; return 1; }
void NETransparentMaterial::ResetShininess() { m_Shininess = 400.0f; return; }


const double& NETransparentMaterial::IndexOfRefraction() const { return m_IndexOfRefraction; }
uint NETransparentMaterial::SetIndexOfRefraction(const double& val) { m_IndexOfRefraction = val; return 1;}
void NETransparentMaterial::ResetIndexOfRefraction() { m_IndexOfRefraction = 1.33f; return; }

