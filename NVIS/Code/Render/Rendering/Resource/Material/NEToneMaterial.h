#pragma once

#include "Rendering/Resource/NEMaterial.h"

class NEToneMaterial : public NEMaterial
{
    Q_OBJECT

    Q_PROPERTY(double shininess READ shininess WRITE setShininess)
    Q_CLASSINFO("name-shininess", "Shininess")
    Q_CLASSINFO("help-shininess", "Shininess")

    Q_PROPERTY(double blueShift READ blueShift WRITE setBlueShift)
    Q_CLASSINFO("name-blueShift", "Blue Shift")
    Q_CLASSINFO("help-blueShift", "Blue Shift")

    Q_PROPERTY(double blueLuminance READ blueLuminance WRITE setBlueLuminance)
    Q_CLASSINFO("name-blueLuminance", "Blue Luminance")
    Q_CLASSINFO("help-blueLuminance", "Blue Luminance")

    Q_PROPERTY(double yellowShift READ yellowShift WRITE setYellowShift)
    Q_CLASSINFO("name-yellowShift", "Yellow Shift")
    Q_CLASSINFO("help-yellowShift", "Yellow Shift")

    Q_PROPERTY(double yellowLuminance READ yellowLuminance WRITE setYellowLuminance)
    Q_CLASSINFO("name-yellowLuminance", "Yellow Luminance")
    Q_CLASSINFO("help-yellowLuminance", "Yellow Luminance")

public:
    NEFEXP_NERENDERING NEToneMaterial(NENodeFactory* factory);
    virtual NEFEXP_NERENDERING ~NEToneMaterial();
    virtual NEFEXP_NERENDERING void updateUniforms();

    float blueShift() const;

    float blueLuminance() const;

    float yellowShift() const;

    float yellowLuminance() const;

public slots:
    float shininess() const;
    void setShininess(float val);



    void setBlueShift(float blueShift);

    void setBlueLuminance(float blueLuminance);

    void setYellowShift(float yellowShift);

    void setYellowLuminance(float yellowLuminance);

protected:


private:
    float m_Shininess;

    // Rendering related
    bool m_Textured;

    double m_blueShift;
    double m_blueLuminance;
    double m_yellowShift;
    double m_yellowLuminance;
};


#include "Base/NENodeFactory.h"

class NEToneMaterialFactory : public NENodeFactory
{
    Q_OBJECT
    Q_INTERFACES(NENodeFactory)

public:
    NEToneMaterialFactory( NEManager* m );
    ~NEToneMaterialFactory();

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
