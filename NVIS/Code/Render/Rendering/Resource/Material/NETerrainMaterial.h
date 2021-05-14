#pragma once

#include "Rendering/Resource/NEMaterial.h"

class FilePath;

class NETerrainMaterial : public NEMaterial
{
    Q_OBJECT

    Q_PROPERTY(double shininess READ shininess WRITE setShininess)
    Q_CLASSINFO("name-shininess", "Shininess")
    Q_CLASSINFO("help-shininess", "Specular Coefficient")

    Q_PROPERTY(bool usePhongShading READ usePhongShading WRITE setUsePhongShading)
    Q_CLASSINFO("name-usePhongShading", "Use Phong Shading")
    Q_CLASSINFO("help-usePhongShading", "Shade the geometry")

public:
    NEFEXP_NERENDERING NETerrainMaterial(NENodeFactory* factory);
    virtual NEFEXP_NERENDERING ~NETerrainMaterial();
    virtual NEFEXP_NERENDERING void updateUniforms();

public slots:
    float shininess() const;
    void setShininess(float val);
    bool usePhongShading() const;
    void setUsePhongShading(bool val);

private:
    float m_Shininess;
    bool m_usePhongShading;
};


#include "Base/NENodeFactory.h"

class NETerrainMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NETerrainMaterialFactory(NEManager* m);
    ~NETerrainMaterialFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();

private:
    NEObject* createInstance();
};
