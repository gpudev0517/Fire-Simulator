#pragma once

#include "Utility/FilePath.h"
#include "Rendering/Resource/NEMaterial.h"


class NEToonMaterial : public NEMaterial
{
    Q_OBJECT

    Q_PROPERTY(double shininess READ shininess WRITE setShininess)
    Q_CLASSINFO("name-shininess", "Shininess")
    Q_CLASSINFO("help-shininess", "Specular Coefficient")


    Q_PROPERTY(PropertyGroupStart Texturing READ groupStart)
    Q_PROPERTY(bool textured READ textured WRITE setTextured)
    Q_CLASSINFO("name-textured", "Textured")
    Q_CLASSINFO("help-textured", "Texture applied or not")

    Q_PROPERTY(FilePath imgName READ imgName WRITE setImgName)
    Q_CLASSINFO("name-imgName", "Texture")
    Q_CLASSINFO("help-imgName", "Texture File")


    Q_PROPERTY(PropertyGroupEnd Texturing_End READ groupEnd)


    Q_ENUMS(NEPhongPreset)
    Q_ENUMS(NEMentalTextureExportOption)

public:
    NEFEXP_NERENDERING NEToonMaterial(NENodeFactory* factory);
    virtual NEFEXP_NERENDERING ~NEToonMaterial();
    virtual NEFEXP_NERENDERING void updateUniforms();



public slots:
    float shininess() const;
    void setShininess(float val);


    bool textured() const;
    void setTextured(bool val);
    FilePath imgName() const;
    void setImgName(const FilePath& val);


protected:


private:
    float m_Shininess;



    // Rendering related
    FilePath m_ImgName;
    bool m_Textured;

};


#include "Base/NENodeFactory.h"

class NEToonMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEToonMaterialFactory(NEManager* m);
    ~NEToonMaterialFactory();

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
