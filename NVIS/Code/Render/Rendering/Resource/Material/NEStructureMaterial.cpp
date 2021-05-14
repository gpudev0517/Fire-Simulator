#include "Rendering/Resource/Material/NEStructureMaterial.h"

class NEStructureMaterialPrivate
{
public:
    NEStructureMaterialPrivate(NEStructureMaterial* s) : S(s)
    {
    }
    /* Property variables */
    NEStructureMaterial::Materials m_MaterialType;
    double m_Roughness;
    QColor m_SpecularColor;
private:
    NEStructureMaterial* S;
};

NE_PROP_DEF(NEStructureMaterial::Materials,NEStructureMaterial,MaterialType,NEStructureMaterial::Matte)
NE_PROP_DEF(double,NEStructureMaterial,Roughness,0.1)
NE_PROP_DEF(QColor,NEStructureMaterial,SpecularColor,QColor(255,255,255,255))

static void initProperties()
{
    NE_INIT_PROP(NEStructureMaterial,MaterialType);
    NE_INIT_PROP(NEStructureMaterial,Roughness);
    NE_INIT_PROP(NEStructureMaterial,SpecularColor);
}

NEStructureMaterial::NEStructureMaterial(NENodeFactory *factory) : NEMaterial(factory)
{
    P = new NEStructureMaterialPrivate(this);
    CALL_ONCE(initProperties);
    P->m_MaterialType = NE_DEFAULT(MaterialType);
    P->m_Roughness = NE_DEFAULT(Roughness);
    P->m_SpecularColor = NE_DEFAULT(SpecularColor);
    setShaderProgram(new QOpenGLShaderProgram());
    //shaderProgram()->addShaderFromSourceFile(QOpenGLShader::Fragment,"System/Shaders/Material/PerPixel/phong.frag");


    setDefaultFS(new QOpenGLShader(QOpenGLShader::Fragment));
    if( defaultFS()->compileSourceFile("System/Shaders/Material/PerPixel/phong.frag") == false )
    {
        qWarning() << " phong.frag : " << defaultFS()->log();
        std::cerr << " phong.frag : "  << defaultFS()->log().toStdString() << std::endl;
    }
    shaderProgram()->addShader(defaultFS());

    shaderProgram()->shaders()[shaderProgram()->shaders().size() - 1]->setObjectName("Phong FS");
}

NEStructureMaterial::~NEStructureMaterial()
{
    delete P;
    shaderProgram()->removeAllShaders();
}

bool NEStructureMaterial::isItNotMatte()
{
    return MaterialType() != Matte;
}

bool NEStructureMaterial::isItPlastic()
{

    return MaterialType() == Plastic;
}


NEStructureMaterialFactory::NEStructureMaterialFactory(NEManager *m) : NENodeFactory(m)
{
}

NEStructureMaterialFactory::~NEStructureMaterialFactory()
{
}

QString NEStructureMaterialFactory::nodeName()
{
    return "Structure Material";
}

uint NEStructureMaterialFactory::nodeType()
{
    return NE::kObjectMaterial;
}

QString NEStructureMaterialFactory::nodeInherits()
{
    return "Materials";
}

QString NEStructureMaterialFactory::objectName()
{
    return "StructureMaterial";
}

QString NEStructureMaterialFactory::nodeVersion()
{
    return "0.1";
}

QString NEStructureMaterialFactory::nodeVendor()
{
    return "Neutrino Dynamics";
}

QString NEStructureMaterialFactory::nodeHelp()
{
    return "Structure material";
}

NEObject* NEStructureMaterialFactory::createInstance()
{
    return new NEStructureMaterial(this);
}

/* Return material type for exporting to 3Delight */
const NEStructureMaterial::Materials& NEStructureMaterial::MaterialType() const
{
    return P->m_MaterialType;
}

/* Return surface roughness for exporting to 3Delight */
const double& NEStructureMaterial::Roughness() const
{
    return P->m_Roughness;
}

/* Return surface specular color for exporting to 3Delight */
const QColor& NEStructureMaterial::SpecularColor() const
{
    return P->m_SpecularColor;
}

/* Set material type for exporting to 3Delight */
uint NEStructureMaterial::SetMaterialType(const Materials &val)
{
    NE_PROP_SET_AND_EVAL(MaterialType,P,val);
    return NE::kReturnSuccess;
}

/* Set surface roughness for exporting to 3Delight */
uint NEStructureMaterial::SetRoughness(const double &val)
{
    if (val < 0.0)
        return NE::kReturnNotChanged;
    NE_PROP_SET_AND_EVAL(Roughness,P,val);
    return NE::kReturnSuccess;
}

/* Set surface specular color for exporting to 3Delight */
uint NEStructureMaterial::SetSpecularColor(const QColor &val)
{
    NE_PROP_SET_AND_EVAL(SpecularColor,P,val);
    return NE::kReturnSuccess;
}
