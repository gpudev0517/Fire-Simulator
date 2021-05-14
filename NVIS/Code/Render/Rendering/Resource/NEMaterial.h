#pragma once

#include "NEResource.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEMaterial))

class NECEXP_NERENDERING NEMaterial : public NEResource
{
    Q_OBJECT
    NE_CLASS("Basic Material Properties")

public:
    enum MaterialType { Default, Water };

    NE_PROP_RW(bool, PerPixel, "Per Pixel", "When enabled, lighting of the material is computed per-pixel", true,)
    NE_PROP_RW(QColor, Color, "Color", "Material Color", true,)
    NE_PROP_RW(double, KAmbient, "Ambient Coefficient", "Ambient Coefficient of the material", true,)
    NE_PROP_RW(double, KDiffuse, "Diffuse Coefficient", "Diffuse Coefficient of the material", true,)
    NE_PROP_RW(double, KSpecular, "Specular Coefficient", "Specular Coefficient of the material", true,)
    NE_PROP_RW(QColor, Ambient, "Ambient Color", "Ambient color of the material", true,)
    NE_PROP_RW(QColor, Specular, "Specular Color", "Specular color of the material", true,)
    NE_PROP_RW(QColor, Ambiance, "Ambiance Color", "Ambiance color of the material", true,)


    Q_ENUMS(CausticType)

public:
    NEMaterial(NENodeFactory* factory);
    virtual ~NEMaterial();
    virtual void updateUniforms();
    virtual MaterialType materialType();
    QOpenGLShaderProgram* shaderProgram() const;
    void setShaderProgram(QOpenGLShaderProgram* pShaderProgram);
    void setDefaultFS(QOpenGLShader *shader);
    QOpenGLShader *defaultFS();

protected:

    virtual bool addSource(NENode* node, NEConn& conn);
    virtual void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);

private:
    NE_DECL_PRIVATE(NEMaterial)
};

Q_DECLARE_INTERFACE(NEMaterial, "NEMaterial")
