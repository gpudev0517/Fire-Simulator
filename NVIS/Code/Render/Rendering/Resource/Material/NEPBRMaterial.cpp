#include "Rendering/Resource/Material/NEPBRMaterial.h"
#include "Base/NEBase.h"
#include "Rendering/Manager/NERenderManager.h"

NEPBRMaterial::NEPBRMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setMetallic(0.5);
    setRoughness(0.5);
    setEnvironmentScale(1.0);
    setSpecularIntensity(1.0);

    m_Texture = 0;
    m_Textured = false;
    m_NormalTexture = 0;
    m_NormalTextured = false;
    m_SpecTexture = 0;
    m_SpecTextured = false;
    m_flipU = false;
    m_flipV = false;
    m_swapUV = false;

    m_MentalTexRepeatX = 1;
    m_MentalTexRepeatY = 1;

    if (NEBASE.guiMode())
    {
        setShaderProgram(new QOpenGLShaderProgram());
//        QString shaderPath = "System/Shaders/Material/PerPixel/pbr.frag";
//        if (!shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, shaderPath))
//        {
//            qWarning() << shaderPath <<  " " << shaderProgram()->log();
//            std::cerr << shaderPath.toStdString() << " " << shaderProgram()->log().toStdString() << std::endl;
//        }


        setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
        if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/pbr.frag") == false )
        {
            qWarning() << " pbr.frag : " << defaultFS()->log();
            std::cerr << " pbr.frag : "  << defaultFS()->log().toStdString() << std::endl;
        }
        shaderProgram()->addShader(defaultFS());
    }

    m_ImgName.setDialogType(FilePath::kOpenFileType);
//    m_ImgName.setFilter("Image files (*.jpg,*.png,*.bmp)");
    m_ImgName.setPath("");

    m_NormalImgName.setDialogType(FilePath::kOpenFileType);
//    m_NormalImgName.setFilter("Image files (*.jpg,*.png,*.bmp)");
    m_NormalImgName.setPath("");

    m_SpecImgName.setDialogType(FilePath::kOpenFileType);
//    m_SpecImgName.setFilter("Image files (*.jpg,*.png,*.bmp)");
    m_SpecImgName.setPath("");
}

NEPBRMaterial::~NEPBRMaterial()
{
    // To prevent crashes on Mac
    if(!NeutrinoInitialized())
        return;

    if (NEBASE.guiMode())
    {
        if (m_Texture)
        {
            delete m_Texture;
        }

        if (m_NormalTexture)
        {
            delete m_NormalTexture;
        }

        if (m_SpecTexture)
        {
            delete m_SpecTexture;
        }

        shaderProgram()->removeAllShaders();
    }
}

int NEPBRMaterial::mentalTexRepeatX() const { return m_MentalTexRepeatX; }

void NEPBRMaterial::setMentalTexRepeatX(int val) { m_MentalTexRepeatX = val; }

int NEPBRMaterial::mentalTexRepeatY() const { return m_MentalTexRepeatY; }

void NEPBRMaterial::setMentalTexRepeatY(int val) { m_MentalTexRepeatY = val; }


void NEPBRMaterial::updateUniforms()
{
    if( NEBASE.guiMode() == false )
        return;

    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("metallic", metallic());
    shaderProgram()->setUniformValue("roughness", roughness());
    shaderProgram()->setUniformValue("environmentScale", environmentScale());
    shaderProgram()->setUniformValue("specularIntensity", specularIntensity());

    shaderProgram()->setUniformValue( "flipU", flipU() );
    shaderProgram()->setUniformValue( "flipV", flipV() );
    shaderProgram()->setUniformValue( "swapUV", swapUV() );
    shaderProgram()->setUniformValue( "repeatU", mentalTexRepeatX() );
    shaderProgram()->setUniformValue( "repeatV", mentalTexRepeatY() );

    shaderProgram()->setUniformValue( "specularWorkflow", specularWorkflow() );

    if( textured() )
    {
        if (m_Texture)
        {
            shaderProgram()->setUniformValue( "colorMap", 0 );
            GL->glActiveTexture( GL_TEXTURE0 );
            m_Texture->bind();
            GL->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }
    shaderProgram()->setUniformValue( "textured", textured() );

    if( normalTextured() )
    {
        if (m_NormalTexture)
        {
            shaderProgram()->setUniformValue( "normalMap", 1 );
            GL->glActiveTexture( GL_TEXTURE1 );
            m_NormalTexture->bind();
        }
    }
    shaderProgram()->setUniformValue( "normalTextured", normalTextured() );

    if( specTextured() )
    {
        if (m_SpecTexture)
        {
            shaderProgram()->setUniformValue( "specMap", 2 );
            GL->glActiveTexture( GL_TEXTURE2 );
            m_SpecTexture->bind();
        }
    }
    shaderProgram()->setUniformValue( "specTextured", specTextured() );
}

float NEPBRMaterial::metallic() const { return m_Metallic; }
void NEPBRMaterial::setMetallic(float val)
{
    m_Metallic = val;
    //updateUniforms();
}

float NEPBRMaterial::roughness() const { return m_Roughness; }
void NEPBRMaterial::setRoughness(float val)
{
    m_Roughness = val;
    //updateUniforms();
}

float NEPBRMaterial::environmentScale() const { return m_EnvironmentScale; }
void NEPBRMaterial::setEnvironmentScale(float val)
{
    m_EnvironmentScale = val;
}

float NEPBRMaterial::specularIntensity() const { return m_SpecularIntensity; }
void NEPBRMaterial::setSpecularIntensity(float val)
{
    m_SpecularIntensity = val;
    //updateUniforms();
}


bool NEPBRMaterial::textured() const { return m_Textured; }

void NEPBRMaterial::setTextured(bool val)
{
    m_Textured = val;
}

FilePath NEPBRMaterial::imgName() const { return m_ImgName; }

void NEPBRMaterial::setImgName(const FilePath& val)
{
    m_ImgName.setPath(val.path());

    if (m_Texture) delete m_Texture;

    m_Texture = new QOpenGLTexture( QImage(m_ImgName) );
    // Set nearest filtering mode for texture minification
    m_Texture->setMinificationFilter( QOpenGLTexture::Nearest );
    // Set bilinear filtering mode for texture magnification
    m_Texture->setMagnificationFilter( QOpenGLTexture::Linear );
    // Wrap textureENV coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_Texture->setWrapMode( QOpenGLTexture::Repeat );
}



bool NEPBRMaterial::normalTextured() const { return m_NormalTextured; }

void NEPBRMaterial::setNormalTextured(bool val) { m_NormalTextured = val; }

FilePath NEPBRMaterial::normalImgName() const { return m_NormalImgName; }

void NEPBRMaterial::setNormalImgName(const FilePath& val)
{
    m_NormalImgName.setPath(val.path());

    if (m_NormalTexture) delete m_NormalTexture;

    m_NormalTexture = new QOpenGLTexture( QImage(m_NormalImgName) );
    // Set nearest filtering mode for texture minification
    m_NormalTexture->setMinificationFilter( QOpenGLTexture::Nearest );
    // Set bilinear filtering mode for texture magnification
    m_NormalTexture->setMagnificationFilter( QOpenGLTexture::Linear );
    // Wrap textureENV coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_NormalTexture->setWrapMode( QOpenGLTexture::Repeat );
}



bool NEPBRMaterial::specTextured() const { return m_SpecTextured; }

void NEPBRMaterial::setSpecTextured(bool val) { m_SpecTextured = val; }

FilePath NEPBRMaterial::specImgName() const { return m_SpecImgName; }

void NEPBRMaterial::setSpecImgName(const FilePath& val)
{
    m_SpecImgName.setPath(val.path());

    m_SpecTexture = new QOpenGLTexture( QImage(m_SpecImgName) );
    // Set nearest filtering mode for texture minification
    m_SpecTexture->setMinificationFilter( QOpenGLTexture::Nearest );
    // Set bilinear filtering mode for texture magnification
    m_SpecTexture->setMagnificationFilter( QOpenGLTexture::Linear );
    // Wrap textureENV coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    m_SpecTexture->setWrapMode( QOpenGLTexture::Repeat );
}


void NEPBRMaterial::setFlipU(bool flipU)
{
    m_flipU = flipU;
    updateUniforms();
}

void NEPBRMaterial::setSwapUV(bool swapUV)
{
    m_swapUV = swapUV;
    updateUniforms();
}

void NEPBRMaterial::setFlipV(bool flipV)
{
    m_flipV = flipV;
    updateUniforms();
}

NEPBRMaterial::SpecularWorkflow NEPBRMaterial::specularWorkflow() const
{
    return m_SpecularWorkflow;
}

void NEPBRMaterial::setSpecularWorkflow(SpecularWorkflow specWorkflow)
{
    m_SpecularWorkflow = specWorkflow;
    updateUniforms();
}


void NEPBRMaterial::propertyTweak(QtPropertyBase* p)
{
    hideProperty("Color", true);
    hideProperty("KAmbient", false);
    hideProperty("KDiffuse", true);
    hideProperty("KSpecular", true);
    hideProperty("Ambient", true);
    hideProperty("Specular", true);
    hideProperty("Ambiance", true);
    NEMaterial::propertyTweak(p);
}


NEPBRMaterialFactory::NEPBRMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEPBRMaterialFactory::~NEPBRMaterialFactory() {}

QString NEPBRMaterialFactory::nodeName() { return "PBR Material"; }

uint NEPBRMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEPBRMaterialFactory::nodeInherits() { return "Materials";}

QString NEPBRMaterialFactory::objectName() { return "PBR"; }

QString NEPBRMaterialFactory::nodeVersion() { return "0.1"; }

QString NEPBRMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEPBRMaterialFactory::nodeHelp() { return "PBR material"; }

NEObject*NEPBRMaterialFactory::createInstance() { return new NEPBRMaterial(this); }
