#pragma once

#include "Utility/FilePath.h"
#include "Rendering/Resource/NEMaterial.h"

class NEPBRMaterial : public NEMaterial
{
    Q_OBJECT

    Q_PROPERTY(double metallic READ metallic WRITE setMetallic)
    Q_CLASSINFO("name-metallic", "Metallic")
    Q_CLASSINFO("help-metallic", "Metallic Coefficient [0..1]")

    Q_PROPERTY(double roughness READ roughness WRITE setRoughness)
    Q_CLASSINFO("name-roughness", "Roughness")
    Q_CLASSINFO("help-roughness", "Roughness Coefficient [0..1]")

    Q_PROPERTY(double environmentScale READ environmentScale WRITE setEnvironmentScale)
    Q_CLASSINFO("name-environmentScale", "Environment Scale")
    Q_CLASSINFO("help-environmentScale", "Environment Scale")

    Q_PROPERTY(double specularIntensity READ specularIntensity WRITE setSpecularIntensity)
    Q_CLASSINFO("name-specularIntensity", "Spec Intensity")
    Q_CLASSINFO("help-specularIntensity", "Specular Intensity from lights except image-based lighting")

    Q_PROPERTY(PropertyGroupStart Texturing READ groupStart)
    Q_PROPERTY(bool textured READ textured WRITE setTextured)
    Q_CLASSINFO("name-textured", "Textured")
    Q_CLASSINFO("help-textured", "Texture applied or not")

    Q_PROPERTY(FilePath imgName READ imgName WRITE setImgName)
    Q_CLASSINFO("name-imgName", "Texture")
    Q_CLASSINFO("help-imgName", "Texture File")

    Q_PROPERTY(bool normalTextured READ normalTextured WRITE setNormalTextured)
    Q_CLASSINFO("name-normalTextured", "Normal Textured")
    Q_CLASSINFO("help-normalTextured", "Normal Texture applied or not")

    Q_PROPERTY(FilePath normalImgName READ normalImgName WRITE setNormalImgName)
    Q_CLASSINFO("name-normalImgName", "Normal Texture")
    Q_CLASSINFO("help-normalImgName", "Normal Texture File")

    Q_PROPERTY(bool specTextured READ specTextured WRITE setSpecTextured)
    Q_CLASSINFO("name-specTextured", "Roughness/Metalness/AO")
    Q_CLASSINFO("help-specTextured", "Roughness/Metalness/AO Texture applied or not")

    Q_PROPERTY(FilePath specImgName READ specImgName WRITE setSpecImgName)
    Q_CLASSINFO("name-specImgName", "Roughness/Metalness/AO Texture")
    Q_CLASSINFO("help-specImgName", "Roughness/Metalness/AO Texture File")

    Q_PROPERTY(SpecularWorkflow specularWorkflow READ specularWorkflow WRITE setSpecularWorkflow)
    Q_CLASSINFO("name-specularWorkflow", "Specular Workflow")
    Q_CLASSINFO("help-specularWorkflow", "Specular Workflow")

    Q_PROPERTY(bool flipU READ flipU WRITE setFlipU)
    Q_CLASSINFO("name-flipU", "Invert U")
    Q_CLASSINFO("help-flipU", "Invert U Coordinate")


    Q_PROPERTY(bool flipV READ flipV WRITE setFlipV)
    Q_CLASSINFO("name-flipV", "Invert V")
    Q_CLASSINFO("help-flipV", "Invert V Coordinate")

    Q_PROPERTY(bool swapUV READ swapUV WRITE setSwapUV)
    Q_CLASSINFO("name-swapUV", "Swap UV")
    Q_CLASSINFO("help-swapUV", "Swap UV Coordinates")

    Q_PROPERTY(int RepeatX READ  mentalTexRepeatX WRITE setMentalTexRepeatX)
    Q_PROPERTY(int RepeatY READ  mentalTexRepeatY WRITE setMentalTexRepeatY)

    Q_PROPERTY(PropertyGroupEnd Texturing_End READ groupEnd)

    Q_ENUMS(NEPBRPreset)
    Q_ENUMS(SpecularWorkflow)

public:
    NEFEXP_NERENDERING NEPBRMaterial(NENodeFactory* factory);
    virtual NEFEXP_NERENDERING ~NEPBRMaterial();
    virtual void propertyTweak(QtPropertyBase* p);
    virtual NEFEXP_NERENDERING void updateUniforms();

    enum SpecularWorkflow
    {
        RoughnessMetal,
        GlossMetal,
        RoughnessInverseMetal,
        GlossInverseMetal
    };

public slots:
    float metallic() const;
    void setMetallic(float val);

    float roughness() const;
    void setRoughness(float val);

    float environmentScale() const;
    void setEnvironmentScale(float val);

    float specularIntensity() const;
    void setSpecularIntensity(float val);

    bool flipU() const
    {
        return m_flipU;
    }

    bool swapUV() const
    {
        return m_swapUV;
    }

    bool flipV() const
    {
        return m_flipV;
    }

    void setFlipU(bool flipU);
    void setSwapUV(bool swapUV);
    void setFlipV(bool flipV);

    int mentalTexRepeatX() const;
    void setMentalTexRepeatX(int val);
    int mentalTexRepeatY() const;
    void setMentalTexRepeatY(int val);


    SpecularWorkflow specularWorkflow() const;
    void setSpecularWorkflow(SpecularWorkflow specWorkflow);

    bool textured() const;
    void setTextured(bool val);
    FilePath imgName() const;
    void setImgName(const FilePath& val);

    bool normalTextured() const;
    void setNormalTextured(bool val);
    FilePath normalImgName() const;
    void setNormalImgName(const FilePath& val);

    bool specTextured() const;
    void setSpecTextured(bool val);
    FilePath specImgName() const;
    void setSpecImgName(const FilePath& val);

protected:

private:
    float m_Metallic;
    float m_Roughness;
    float m_EnvironmentScale;
    float m_SpecularIntensity;

    // Rendering related
    FilePath m_ImgName;
    bool m_Textured;

    FilePath m_NormalImgName;
    bool m_NormalTextured;

    FilePath m_SpecImgName;
    bool m_SpecTextured;

    QOpenGLTexture* m_Texture;
    QOpenGLTexture* m_NormalTexture;
    QOpenGLTexture* m_SpecTexture;

    SpecularWorkflow m_SpecularWorkflow;
    bool m_flipU;
    bool m_flipV;
    bool m_swapUV;

    int m_MentalTexRepeatX;
    int m_MentalTexRepeatY;
};


#include "Base/NENodeFactory.h"

class NEPBRMaterialFactory : public NENodeFactory
{
	Q_OBJECT
	Q_INTERFACES(NENodeFactory)

public:
    NEPBRMaterialFactory(NEManager* m);
    ~NEPBRMaterialFactory();

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
