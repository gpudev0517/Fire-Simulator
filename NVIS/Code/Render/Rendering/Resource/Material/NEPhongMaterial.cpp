#include "Rendering/Resource/Material/NEPhongMaterial.h"
#include "Utility/FilePath.h"
#include "Base/NEBase.h"
#include "Base/NESceneManager.h"
#include "Rendering/Manager/NERenderManager.h"
#include "Rendering/Manager/NEGLManager.h"
#include "Spatial/NESpatial.h"


NEPhongMaterial::NEPhongMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setShininess(2.0f);

    setPreset(MRMIA);

    m_Texture2D = 0;
    m_TextureCube = 0;
    m_Textured = NETextureType::None;
    m_flipU = false;
    m_flipV = false;
    m_swapUV = false;
    m_MentalTexRepeatX = 1;
    m_MentalTexRepeatY = 1;

    m_flatShading = false;



    setPresetValues();
    if (NEBASE.guiMode())
    {
        setShaderProgram(new QOpenGLShaderProgram());
//        QString shaderPath = "System/Shaders/Material/PerPixel/phong.frag";
//        if (!shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, shaderPath))
//        {
//            qWarning() << shaderPath <<  " " << shaderProgram()->log();
//            std::cerr << shaderPath.toStdString() << " " << shaderProgram()->log().toStdString() << std::endl;
//        }

        setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
        if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/phong.frag") == false )
        {
            qWarning() << " phong.frag : " << defaultFS()->log();
            std::cerr << " phong.frag : "  << defaultFS()->log().toStdString() << std::endl;
        }
        shaderProgram()->addShader(defaultFS());

        shaderProgram()->shaders()[shaderProgram()->shaders().size() - 1]->setObjectName("Phong FS");

    }
    m_ImgNamePositiveX.setDialogType(FilePath::kOpenFileType);
    //m_ImgName.setFilter("Image files (*.jpg;*.png;*.bmp)");
    m_ImgNamePositiveX.setPath("");

    m_ImgNameNegativeX.setDialogType(FilePath::kOpenFileType);
    m_ImgNameNegativeX.setPath("");

    m_ImgNamePositiveY.setDialogType(FilePath::kOpenFileType);
    m_ImgNamePositiveY.setPath("");

    m_ImgNameNegativeY.setDialogType(FilePath::kOpenFileType);
    m_ImgNameNegativeY.setPath("");

    m_ImgNamePositiveZ.setDialogType(FilePath::kOpenFileType);
    m_ImgNamePositiveZ.setPath("");

    m_ImgNameNegativeZ.setDialogType(FilePath::kOpenFileType);
    m_ImgNameNegativeZ.setPath("");
}

NEPhongMaterial::~NEPhongMaterial()
{
    // To prevent crashes on Mac
    if(!NeutrinoInitialized())
        return;

    if (NEBASE.guiMode())
    {
        if (m_Texture2D)
        {
            delete m_Texture2D;
        }

        if (m_TextureCube)
        {
            delete m_TextureCube;
        }

        shaderProgram()->removeAllShaders();
    }
}

void NEPhongMaterial::updateUniforms()
{
    if( NEBASE.guiMode() == false )
        return;

    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("Shininess", shininess());

    shaderProgram()->setUniformValue( "flipU", flipU() );
    shaderProgram()->setUniformValue( "flipV", flipV() );
    shaderProgram()->setUniformValue( "swapUV", swapUV() );
    shaderProgram()->setUniformValue( "repeatU", mentalTexRepeatX() );
    shaderProgram()->setUniformValue( "repeatV", mentalTexRepeatY() );

    if( textured() == NEPhongMaterial::Texture2D )
    {
        if (m_Texture2D)
        {
            shaderProgram()->setUniformValue( "cubeMap", 0 );
            GL->glActiveTexture( GL_TEXTURE0 );
            m_Texture2D->bind();
            GL->glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        }
    }
    else if( textured() == NEPhongMaterial::TextureCubeMapFrom6Images || textured() == NEPhongMaterial::TextureCubeMapFromOneImage){
        if (m_TextureCube)
        {
            shaderProgram()->setUniformValue( "cubeMap", 0 );

            GL->glActiveTexture( GL_TEXTURE0 );
            m_TextureCube->bind(0);
            GL->glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    shaderProgram()->setUniformValue( "textured", (int)textured() );
    shaderProgram()->setUniformValue( "uDepthScale", 0.5f );
    //shaderProgram()->setUniformValue( "uAlpha", 0.5f );
}

float NEPhongMaterial::shininess() const { return m_Shininess; }

void NEPhongMaterial::setShininess(float val) { m_Shininess = val; }


NEPhongMaterial::NETextureType NEPhongMaterial::textured() const
{
    return m_Textured;
}

void NEPhongMaterial::setTextured(NEPhongMaterial::NETextureType val)
{
    m_Textured = val;
    if( NEBASE.guiMode() == false )
        return;
    if(m_Textured == NEPhongMaterial::Texture2D){
        if (m_Texture2D) {
            delete m_Texture2D;
            m_Texture2D = NULL;
        }

        if(m_ImgName.path().isEmpty())
            return;

        m_Texture2D = new QOpenGLTexture( QImage(m_ImgName) );
        // Set nearest filtering mode for texture minification
        m_Texture2D->setMinificationFilter( QOpenGLTexture::Nearest );
        // Set bilinear filtering mode for texture magnification
        m_Texture2D->setMagnificationFilter( QOpenGLTexture::Linear );
        // Wrap textureENV coordinates by repeating
        // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
        m_Texture2D->setWrapMode( QOpenGLTexture::Repeat );

    }
    else if(m_Textured == NEPhongMaterial::TextureCubeMapFrom6Images || m_Textured == NEPhongMaterial::TextureCubeMapFromOneImage){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }

}

void NEPhongMaterial::buildCubeMapTexture()
{

    if(textured() ==  NEPhongMaterial::TextureCubeMapFrom6Images){
        if(m_ImgNamePositiveX.path().isEmpty())
            return;

        m_TextureCube = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
        m_TextureCube->create();

        QImage img(QImage(m_ImgNamePositiveX).convertToFormat(QImage::Format_RGB888));
        int w = img.width();
        int h = img.height();
        int finalWidth =  fmin(512, fmin(w, h));


        img = img.scaled(finalWidth, finalWidth);

        m_TextureCube->setSize(finalWidth, finalWidth);
        m_TextureCube->setFormat(QOpenGLTexture::RGB8_UNorm);
        m_TextureCube->allocateStorage();

        m_TextureCube->bind();
        if(!m_ImgNamePositiveX.path().isEmpty()){
            //QImage img(QImage(m_ImgNamePositiveX).convertToFormat(QImage::Format_RGB888));
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }
        if(!m_ImgNameNegativeX.path().isEmpty()){
            QImage img(QImage(m_ImgNameNegativeX).convertToFormat(QImage::Format_RGB888));
            img = img.scaled(finalWidth, finalWidth);
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }
        if(!m_ImgNamePositiveY.path().isEmpty()){
            QImage img(QImage(m_ImgNamePositiveY).convertToFormat(QImage::Format_RGB888));
            img = img.scaled(finalWidth,finalWidth);
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }
        if(!m_ImgNameNegativeY.path().isEmpty()){
            QImage img(QImage(m_ImgNameNegativeY).convertToFormat(QImage::Format_RGB888));
            img = img.scaled(finalWidth,finalWidth);
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }
        if(!m_ImgNamePositiveZ.path().isEmpty()){
            QImage img(QImage(m_ImgNamePositiveZ).convertToFormat(QImage::Format_RGB888));
            img = img.scaled(finalWidth,finalWidth);
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }
        if(!m_ImgNameNegativeZ.path().isEmpty()){
            QImage img(QImage(m_ImgNameNegativeZ).convertToFormat(QImage::Format_RGB888));
            img = img.scaled(finalWidth,finalWidth);
            m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, img.bits());
        }

        m_TextureCube->setWrapMode(QOpenGLTexture::Repeat);
        m_TextureCube->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    //    // Set nearest filtering mode for texture minification
    //    m_TextureCube->setMinificationFilter( QOpenGLTexture::Nearest );
    //    // Set bilinear filtering mode for texture magnification
    //    m_TextureCube->setMagnificationFilter( QOpenGLTexture::Linear );
    //    // Wrap textureENV coordinates by repeating
    //    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    //    m_TextureCube->setWrapMode( QOpenGLTexture::Repeat );
    }
    else if(textured() == NEPhongMaterial::TextureCubeMapFromOneImage){

        if(m_ImgName.path().isEmpty())
            return;

        m_TextureCube = new QOpenGLTexture(QOpenGLTexture::TargetCubeMap);
        m_TextureCube->create();

        QImage img(QImage(m_ImgName).convertToFormat(QImage::Format_RGB888));
        int w = img.width();
        int h = img.height();

        int finalWidth = fmin(512, fmin(w/4, h/3));

        QRect positiveXRect(w/4 * 2, h/3, w/4, h/3);
        QImage posXImg = img.copy(positiveXRect);
        posXImg = posXImg.scaled(finalWidth, finalWidth);

        QRect negativeXRect(0, h/3, w/4, h/3);
        QImage negXImg = img.copy(negativeXRect);
        negXImg = negXImg.scaled(finalWidth, finalWidth);

        QRect positiveYRect(w/4, 0, w/4, h/3);
        QImage posYImg = img.copy(positiveYRect);
        posYImg = posYImg.scaled(finalWidth, finalWidth);

        QRect negativeYRect(w/4, h/3*2, w/4, h/3);
        QImage negYImg = img.copy(negativeYRect);
        negYImg = negYImg.scaled(finalWidth, finalWidth);

        QRect positiveZRect(w/4, h/3, w/4, h/3);
        QImage posZImg = img.copy(positiveZRect);
        posZImg = posZImg.scaled(finalWidth, finalWidth);

        QRect negativeZRect(w/4*3, h/3, w/4, h/3);
        QImage negZImg = img.copy(negativeZRect);
        negZImg = negZImg.scaled(finalWidth, finalWidth);

        m_TextureCube->setSize(finalWidth, finalWidth);
        m_TextureCube->setFormat(QOpenGLTexture::RGB8_UNorm);
        m_TextureCube->allocateStorage();

        m_TextureCube->bind();
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, posXImg.bits());
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeX, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, negXImg.bits());
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, posYImg.bits());
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeY, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, negYImg.bits());
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, posZImg.bits());
        m_TextureCube->setData(0, 0, QOpenGLTexture::CubeMapNegativeZ, QOpenGLTexture::RGB, QOpenGLTexture::UInt8, negZImg.bits());

        m_TextureCube->setWrapMode(QOpenGLTexture::Repeat);
        m_TextureCube->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);

    }


}

FilePath NEPhongMaterial::imgName() const
{
    return m_ImgName;

}
void NEPhongMaterial::setImgName(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgName.setPath(val.path());

    if(textured() == NEPhongMaterial::Texture2D){
        if (m_Texture2D) {
            delete m_Texture2D;
            m_Texture2D = NULL;
        }

        if(m_ImgName.path().isEmpty())
            return;

        m_Texture2D = new QOpenGLTexture( QImage(m_ImgName) );
        // Set nearest filtering mode for texture minification
        m_Texture2D->setMinificationFilter( QOpenGLTexture::Nearest );
        // Set bilinear filtering mode for texture magnification
        m_Texture2D->setMagnificationFilter( QOpenGLTexture::Linear );
        // Wrap textureENV coordinates by repeating
        // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
        m_Texture2D->setWrapMode( QOpenGLTexture::Repeat );
    }
    else if (textured() == NEPhongMaterial::TextureCubeMapFromOneImage){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }

}

FilePath NEPhongMaterial::imgNamePositiveX() const { return m_ImgNamePositiveX; }

void NEPhongMaterial::setImgNamePositiveX(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNamePositiveX.setPath(val.path());

    if( textured() == NEPhongMaterial::TextureCubeMapFrom6Images){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }

}

FilePath NEPhongMaterial::imgNameNegativeX() const { return m_ImgNameNegativeX; }

void NEPhongMaterial::setImgNameNegativeX(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNameNegativeX.setPath(val.path());

    if(textured() == NEPhongMaterial::TextureCubeMapFrom6Images){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }
}

FilePath NEPhongMaterial::imgNamePositiveY() const { return m_ImgNamePositiveY; }

void NEPhongMaterial::setImgNamePositiveY(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNamePositiveY.setPath(val.path());

    if(textured() == NEPhongMaterial::TextureCubeMapFrom6Images){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }

}

FilePath NEPhongMaterial::imgNameNegativeY() const { return m_ImgNameNegativeY; }

void NEPhongMaterial::setImgNameNegativeY(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNameNegativeY.setPath(val.path());

    if(textured() == NEPhongMaterial::TextureCubeMapFrom6Images){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }

        buildCubeMapTexture();
    }

}

FilePath NEPhongMaterial::imgNamePositiveZ() const { return m_ImgNamePositiveZ; }

void NEPhongMaterial::setImgNamePositiveZ(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNamePositiveZ.setPath(val.path());

    if(textured() == NEPhongMaterial::TextureCubeMapFrom6Images){
        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }

}

FilePath NEPhongMaterial::imgNameNegativeZ() const { return m_ImgNameNegativeZ; }

void NEPhongMaterial::setImgNameNegativeZ(const FilePath& val)
{
    if( NEBASE.guiMode() == false )
        return;

    m_ImgNameNegativeZ.setPath(val.path());

    if(textured() == NEPhongMaterial::TextureCubeMapFrom6Images){

        if( m_TextureCube){
            m_TextureCube->destroy();
            delete m_TextureCube;
            m_TextureCube = NULL;
        }
        buildCubeMapTexture();
    }
}

NEPhongMaterial::NEPhongPreset NEPhongMaterial::preset() const { return m_Preset; }

void NEPhongMaterial::setPreset(NEPhongMaterial::NEPhongPreset val) { m_Preset = val; setPresetValues();}


int NEPhongMaterial::mentalTexRepeatX() const { return m_MentalTexRepeatX; }

void NEPhongMaterial::setMentalTexRepeatX(int val) { m_MentalTexRepeatX = val; }

int NEPhongMaterial::mentalTexRepeatY() const { return m_MentalTexRepeatY; }

void NEPhongMaterial::setMentalTexRepeatY(int val) { m_MentalTexRepeatY = val; }

void NEPhongMaterial::setFlipU(bool flipU)
{
    m_flipU = flipU;
    updateUniforms();
}

void NEPhongMaterial::setSwapUV(bool swapUV)
{
    m_swapUV = swapUV;
    updateUniforms();
}

void NEPhongMaterial::setFlatShading(bool flatShading)
{
    m_flatShading = flatShading;
    for( NESpatial* object : NESCENE.spatials() )
    {
        const NEIndexedTriMesh& mesh = object->triangleMesh();
        if( NERENDERMAN.GLManager()->material(  const_cast< NEIndexedTriMesh*>( &mesh )) == this)
        {
            if( flatShading)
                object->SetRenderMode(NESpatial::FlatShaded);
            else
                object->SetRenderMode(NESpatial::SmoothShaded);
        }
    }
    updateUniforms();
}


void NEPhongMaterial::setFlipV(bool flipV)
{
    m_flipV = flipV;
    updateUniforms();
}

void NEPhongMaterial::setPresetValues()
{
}


NEPhongMaterialFactory::NEPhongMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEPhongMaterialFactory::~NEPhongMaterialFactory() {}

QString NEPhongMaterialFactory::nodeName() { return "Phong Material"; }

uint NEPhongMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEPhongMaterialFactory::nodeInherits() { return "Materials";}

QString NEPhongMaterialFactory::objectName() { return "Phong"; }

QString NEPhongMaterialFactory::nodeVersion() { return "0.1"; }

QString NEPhongMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEPhongMaterialFactory::nodeHelp() { return "Phong material"; }

NEObject*NEPhongMaterialFactory::createInstance() { return new NEPhongMaterial(this); }
