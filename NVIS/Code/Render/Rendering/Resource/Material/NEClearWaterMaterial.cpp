#include "Rendering/Resource/Material/NEClearWaterMaterial.h"

NEClearWaterMaterial::NEClearWaterMaterial(NENodeFactory* factory) : NETransparentMaterial(factory)
{
    setDiffuse(QColor(255,255,255));
    setDefaultDiffuse(true);
    m_RefractionFalloffColor.setRgbF(0.0470588, 0.490196, 0.552941);
    m_RefrFalloffDist = 1.0f;


    setShaderProgram(new QOpenGLShaderProgram());
    //shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/clearwater.frag");


    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/clearwater.frag") == false )
    {
        qWarning() << " clearwater.frag : " << defaultFS()->log();
        std::cerr << " clearwater.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());


    setPresetValues();
}

NEClearWaterMaterial::~NEClearWaterMaterial()
{
    // To prevent crashes on Mac
    if(!NeutrinoInitialized())
        return;

    shaderProgram()->removeAllShaders();
}

NEClearWaterMaterial::NEClearWaterPreset NEClearWaterMaterial::clearWaterType() const {return m_Preset;}

void NEClearWaterMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("Shininess", (float) Shininess());
    /*
    if( textured() )
    {
        QOpenGLTexture* texture = new QOpenGLTexture( QImage(imgName().path()) );
        // Set nearest filtering mode for texture minification
        texture->setMinificationFilter( QOpenGLTexture::Nearest );
        // Set bilinear filtering mode for texture magnification
        texture->setMagnificationFilter( QOpenGLTexture::Linear );
        // Wrap textureENV coordinates by repeating
        // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
        texture->setWrapMode( QOpenGLTexture::Repeat );

        glActiveTexture( GL_TEXTURE0 );
        texture->bind();
        shaderProgram()->setUniformValue( "colorMap", 0 );
    }
    */
}

void NEClearWaterMaterial::setClearWaterType(NEClearWaterMaterial::NEClearWaterPreset preset) {m_Preset = preset; setPresetValues();}

float NEClearWaterMaterial::reflectivity() const { return m_Reflectivity; }

void NEClearWaterMaterial::setReflectivity(float val) { m_Reflectivity = val; }

float NEClearWaterMaterial::transparency() const { return m_Transparency; }

void NEClearWaterMaterial::setTransparency(float val) { m_Transparency = val; }

bool NEClearWaterMaterial::refractionFalloff() const { return m_RefractionFalloff; }

void NEClearWaterMaterial::setRefractionFalloff(bool val) { m_RefractionFalloff = val; }

float NEClearWaterMaterial::refrFalloffDist() const { return m_RefrFalloffDist; }

void NEClearWaterMaterial::setRefrFalloffDist(float val) { m_RefrFalloffDist = val; }

QColor NEClearWaterMaterial::refractionFalloffColor() const { return m_RefractionFalloffColor; }

void NEClearWaterMaterial::setRefractionFalloffColor(QColor val) { m_RefractionFalloffColor = val; }

bool NEClearWaterMaterial::useRefractionFalloffColor() const { return m_UseRefractionFalloffColor; }

void NEClearWaterMaterial::setUseRefractionFalloffColor(bool val) { m_UseRefractionFalloffColor = val; }

bool NEClearWaterMaterial::fresnel() const { return m_Fresnel; }

void NEClearWaterMaterial::setFresnel(bool val) { m_Fresnel = val; }

bool NEClearWaterMaterial::thinWalled() const { return m_ThinWalled; }

void NEClearWaterMaterial::setThinWalled(bool val) { m_ThinWalled = val; }

QColor NEClearWaterMaterial::diffuse() const { return m_Diffuse; }

void NEClearWaterMaterial::setDiffuse(QColor val) { m_Diffuse = val; }

float NEClearWaterMaterial::diffWeight() const { return m_DiffWeight; }

void NEClearWaterMaterial::setDiffWeight(float val) { m_DiffWeight = val; }

QColor NEClearWaterMaterial::outColor() const { return m_OutColor; }

void NEClearWaterMaterial::setOutColor(QColor val) { m_OutColor = val; }

float NEClearWaterMaterial::outIOR() const { return m_outIOR; }

void NEClearWaterMaterial::setOutIOR(float val) { m_outIOR = val; }

bool NEClearWaterMaterial::ignoreNormals() const { return m_IgnoreNormals; }

void NEClearWaterMaterial::setIgnoreNormals(bool val) { m_IgnoreNormals = val; }

float NEClearWaterMaterial::phongCoef() const { return m_PhongCoef; }

void NEClearWaterMaterial::setPhongCoef(float val) { m_PhongCoef = val; }

bool NEClearWaterMaterial::defaultDiffuse() const { return m_DefaultDiffuse; }

void NEClearWaterMaterial::setDefaultDiffuse(bool val) { m_DefaultDiffuse = val; }

bool NEClearWaterMaterial::doRefractiveCaustics() const { return m_DoRefractiveCaustics; }

void NEClearWaterMaterial::setDoRefractiveCaustics(bool val) { m_DoRefractiveCaustics = val; }

/*!
*/
void NEClearWaterMaterial::setPresetValues()
{
    switch(m_Preset)
    {
    case NEClearWaterPreset::Dielectric:
        setDielectricPreset();
        break;
    }
}

/*!
*/
void NEClearWaterMaterial::setMiaWaterPreset()
{
    setDefaultDiffuse(true);
    setDiffuse(QColor(255,255,255));
    setDiffWeight(0);
    setReflectivity(1.0f);
    setTransparency(1.0f);
    SetIndexOfRefraction(1.33);
    setFresnel(true);
    setDoRefractiveCaustics(true);
}

void NEClearWaterMaterial::setMiaColoredDiffusePreset()
{
    setDefaultDiffuse( false );
    setDiffuse( QColor( 0, 255, 255 ) );
    setDiffWeight( 0.5 );
    setReflectivity( 1.0f);
    setTransparency( 0.8f);
    SetIndexOfRefraction( 1.33f );
    setFresnel(true);
    setDoRefractiveCaustics(true);
}

void NEClearWaterMaterial::setMiaColoredRefractPreset()
{
    setDefaultDiffuse(true);
    setDiffuse(QColor(255,255,255));
    setRefractionFalloffColor(QColor::fromRgbF(0.0470588, 0.490196, 0.552941));
    setRefrFalloffDist(1.0f);
    setReflectivity(1.0f);
    setTransparency(1.0f);
    SetIndexOfRefraction(1.33f);
    setFresnel(true);
    setDiffWeight(0.0);
    setUseRefractionFalloffColor(true);
    setRefractionFalloff(true);
    setThinWalled(false);
    setDoRefractiveCaustics(true);
}

void NEClearWaterMaterial::setDielectricPreset()
{
    SetIndexOfRefraction(1.33f);
    setDiffuse(QColor(255,255,255));
    setOutColor(QColor(255,255,255));
    setOutIOR(1.0f);
    setPhongCoef(60);
    setIgnoreNormals(false);
}



NEClearWaterMaterialFactory::NEClearWaterMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEClearWaterMaterialFactory::~NEClearWaterMaterialFactory() {}

QString NEClearWaterMaterialFactory::nodeName() { return "Clear Water Material"; }

uint NEClearWaterMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEClearWaterMaterialFactory::nodeInherits() { return "Materials";}

QString NEClearWaterMaterialFactory::objectName() { return "Water"; }

QString NEClearWaterMaterialFactory::nodeVersion() { return "0.1"; }

QString NEClearWaterMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEClearWaterMaterialFactory::nodeHelp() { return "Clear water material"; }

NEObject*NEClearWaterMaterialFactory::createInstance() { return new NEClearWaterMaterial(this); }
