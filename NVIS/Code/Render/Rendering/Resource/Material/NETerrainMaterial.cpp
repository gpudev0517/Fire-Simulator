#include "Rendering/Resource/Material/NETerrainMaterial.h"

NETerrainMaterial::NETerrainMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setShininess(2.0f);
    setUsePhongShading(false);

    setShaderProgram(new QOpenGLShaderProgram());
//    QString shaderPath = "System/Shaders/Material/PerPixel/terrain.frag";
//    if(!shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, shaderPath))
//    {
//        qWarning() << shaderPath <<  " " << shaderProgram()->log();
//        std::cerr << shaderPath.toStdString() << " " << shaderProgram()->log().toStdString() << std::endl;
//    }

    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/terrain.frag") == false )
    {
        qWarning() << " terrain.frag : " << defaultFS()->log();
        std::cerr << " terrain.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());
}

NETerrainMaterial::~NETerrainMaterial()
{
    // To prevent crashes on Mac
    if( !NeutrinoInitialized() )
        return;

    shaderProgram()->removeAllShaders();

}

void NETerrainMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("Shininess", shininess());
    shaderProgram()->setUniformValue("usePhongShading", usePhongShading());
}

float NETerrainMaterial::shininess() const { return m_Shininess; }

void NETerrainMaterial::setShininess(float val) { m_Shininess = val; }

bool NETerrainMaterial::usePhongShading() const { return m_usePhongShading; }

void NETerrainMaterial::setUsePhongShading(bool val) { m_usePhongShading = val; }

NETerrainMaterialFactory::NETerrainMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NETerrainMaterialFactory::~NETerrainMaterialFactory() {}

QString NETerrainMaterialFactory::nodeName() { return "Terrain Material"; }

uint NETerrainMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NETerrainMaterialFactory::nodeInherits() { return "Materials";}

QString NETerrainMaterialFactory::objectName() { return "Terrain"; }

QString NETerrainMaterialFactory::nodeVersion() { return "0.1"; }

QString NETerrainMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NETerrainMaterialFactory::nodeHelp() { return "Terrain material"; }

NEObject*NETerrainMaterialFactory::createInstance() { return new NETerrainMaterial(this); }
