#pragma once

#include "Utility/FilePath.h"
#include "Rendering/Resource/NEMaterial.h"
#include "NEResource.h"

class FilePath;

class NECEXP_NERENDERING NEPhongMaterial : public NEMaterial
{
    Q_OBJECT
public:

    Q_PROPERTY(NEPhongPreset preset READ preset WRITE setPreset)
    Q_CLASSINFO("name-preset", "Preset")
    Q_CLASSINFO("help-preset", "Different Presets changes with the Renderer")

    Q_PROPERTY(double shininess READ shininess WRITE setShininess)
    Q_CLASSINFO("name-shininess", "Shininess")
    Q_CLASSINFO("help-shininess", "Shiniess Coefficient")

    Q_PROPERTY(bool flatShading READ flatShading WRITE setFlatShading)
    Q_CLASSINFO("name-flatShading", "Flat Shading")
    Q_CLASSINFO("help-flatShading", "On/Off Flat Shading")


    Q_PROPERTY(PropertyGroupStart Texturing READ groupStart)
    Q_PROPERTY(NETextureType textured READ textured WRITE setTextured)
    Q_CLASSINFO("name-textured", "Textured")
    Q_CLASSINFO("help-textured", "Texture applied or not")

    Q_PROPERTY(FilePath imgName READ imgName WRITE setImgName DESIGNABLE isOneImage)
    Q_CLASSINFO("name-imgName", "Texture Main")
    Q_CLASSINFO("help-imgName", "Main Texture File")

    Q_PROPERTY(FilePath imgNamePositiveX READ imgNamePositiveX WRITE setImgNamePositiveX DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNamePositiveX", "Texture Positive X")
    Q_CLASSINFO("help-imgNamePositiveX", "Texture Positive X File")

    Q_PROPERTY(FilePath imgNameNegativeX READ imgNameNegativeX WRITE setImgNameNegativeX DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNameNegativeX", "Texture Negative X")
    Q_CLASSINFO("help-imgNameNegativeX", "Texture Negative X File")


    Q_PROPERTY(FilePath imgNamePositiveY READ imgNamePositiveY WRITE setImgNamePositiveY DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNamePositiveY", "Texture Positive Y")
    Q_CLASSINFO("help-imgNamePositiveY", "Texture Positive Y File")


    Q_PROPERTY(FilePath imgNameNegativeY READ imgNameNegativeY WRITE setImgNameNegativeY DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNameNegativeY", "Texture Negative Y")
    Q_CLASSINFO("help-imgNameNegativeY", "Texture Negative Y File")


    Q_PROPERTY(FilePath imgNamePositiveZ READ imgNamePositiveZ WRITE setImgNamePositiveZ DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNamePositiveZ", "Texture Positive Z")
    Q_CLASSINFO("help-imgNamePositiveZ", "Texture Positive Z File")

    Q_PROPERTY(FilePath imgNameNegativeZ READ imgNameNegativeZ WRITE setImgNameNegativeZ DESIGNABLE is6Images)
    Q_CLASSINFO("name-imgNameNegativeZ", "Texture Negative Z")
    Q_CLASSINFO("help-imgNameNegativeZ", "Texture Negative Z File")

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

    Q_ENUMS(NEPhongPreset NETextureType)


    NEPhongMaterial(NENodeFactory* factory);
    virtual  ~NEPhongMaterial();
    virtual  void updateUniforms();

    enum NEPhongPreset
    {
        PovrayRadiosity,
        MRMIA
    };

    enum NETextureType
    {
        None = 0,
        Texture2D,
        TextureCubeMapFrom6Images,
        TextureCubeMapFromOneImage
    };

    bool flipU() const
    {
        return m_flipU;
    }


    bool swapUV() const
    {
        return m_swapUV;
    }

    bool flatShading() const
    {
        return m_flatShading;
    }

    bool flipV() const
    {
        return m_flipV;
    }

    bool isOneImage()
    {
        return m_Textured == Texture2D || m_Textured == TextureCubeMapFromOneImage;
    }

    bool is6Images()
    {
        return m_Textured == TextureCubeMapFrom6Images;
    }

public slots:
    float shininess() const;
    void setShininess(float val);

    NETextureType textured() const;
    void setTextured(NETextureType val);

    FilePath imgName() const;
    void setImgName(const FilePath& val);

    FilePath imgNamePositiveX() const;
    void setImgNamePositiveX(const FilePath& val);

    FilePath imgNameNegativeX() const;
    void setImgNameNegativeX(const FilePath& val);

    FilePath imgNamePositiveY() const;
    void setImgNamePositiveY(const FilePath& val);

    FilePath imgNameNegativeY() const;
    void setImgNameNegativeY(const FilePath& val);

    FilePath imgNamePositiveZ() const;
    void setImgNamePositiveZ(const FilePath& val);

    FilePath imgNameNegativeZ() const;
    void setImgNameNegativeZ(const FilePath& val);

    NEPhongPreset preset() const;
    void setPreset(NEPhongPreset val);

    int mentalTexRepeatX() const;
    void setMentalTexRepeatX(int val);
    int mentalTexRepeatY() const;
    void setMentalTexRepeatY(int val);


    void setFlipU(bool flipU);

    void setSwapUV(bool swapUV);

    void setFlatShading(bool flatShading);

    void setFlipV(bool flipV);

protected:

    //!
    void setPresetValues();
    //!
    void setPovPreset();
    //!
    void setMIAPreset();

private:
    float m_Shininess;
    NEPhongPreset m_Preset;

    // Rendering related
    FilePath m_ImgName;
    FilePath m_ImgNamePositiveX;
    FilePath m_ImgNamePositiveY;
    FilePath m_ImgNamePositiveZ;
    FilePath m_ImgNameNegativeX;
    FilePath m_ImgNameNegativeY;
    FilePath m_ImgNameNegativeZ;
    NETextureType m_Textured;

    void buildCubeMapTexture();

    int m_MentalTexRepeatX;
    int m_MentalTexRepeatY;

    QOpenGLTexture* m_Texture2D;
    QOpenGLTexture* m_TextureCube;
    bool m_flipU;
    bool m_flipV;
    bool m_swapUV;
    bool m_flatShading;
};


#include "Base/NENodeFactory.h"

class NEPhongMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEPhongMaterialFactory(NEManager* m);
    ~NEPhongMaterialFactory();

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
