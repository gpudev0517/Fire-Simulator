#include "Rendering/Resource/Material/NEToonMaterial.h"
#include "Utility/FilePath.h"

NEToonMaterial::NEToonMaterial(NENodeFactory* factory) : NEMaterial(factory)
{
    setShininess(2.0f);

    setShaderProgram(new QOpenGLShaderProgram());
    //shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment, "System/Shaders/Material/PerPixel/toon.frag");

    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/toon.frag") == false )
    {
        qWarning() << " toon.frag : " << defaultFS()->log();
        std::cerr << " toon.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());

    m_ImgName.setDialogType(FilePath::kOpenFileType);
    m_ImgName.setPath("");
}

NEToonMaterial::~NEToonMaterial()
{
    // To prevent crashes on Mac
    if( !NeutrinoInitialized() )
        return;

    shaderProgram()->removeAllShaders();

}

void NEToonMaterial::updateUniforms()
{
    NEMaterial::updateUniforms();
    shaderProgram()->setUniformValue("Shininess", shininess());
}

float NEToonMaterial::shininess() const { return m_Shininess; }

void NEToonMaterial::setShininess(float val) { m_Shininess = val; }

bool NEToonMaterial::textured() const { return m_Textured; }

void NEToonMaterial::setTextured(bool val) { m_Textured = val; }

FilePath NEToonMaterial::imgName() const { return m_ImgName; }

void NEToonMaterial::setImgName(const FilePath& val) { m_ImgName.setPath(val.path()); }


NEToonMaterialFactory::NEToonMaterialFactory(NEManager* m) : NENodeFactory(m) {}

NEToonMaterialFactory::~NEToonMaterialFactory() {}

QString NEToonMaterialFactory::nodeName() { return "Toon Material"; }

uint NEToonMaterialFactory::nodeType(){ return NE::kObjectMaterial;}

QString NEToonMaterialFactory::nodeInherits() { return "Materials";}

QString NEToonMaterialFactory::objectName() { return "Toon"; }

QString NEToonMaterialFactory::nodeVersion() { return "0.1"; }

QString NEToonMaterialFactory::nodeVendor() { return "Neutrino Dynamics"; }

QString NEToonMaterialFactory::nodeHelp() { return "Toon material"; }

NEObject*NEToonMaterialFactory::createInstance() { return new NEToonMaterial(this); }
