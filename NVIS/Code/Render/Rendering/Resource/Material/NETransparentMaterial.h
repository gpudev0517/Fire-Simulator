#pragma once

#include "Rendering/Resource/NEMaterial.h"

class NECEXP_NERENDERING NETransparentMaterial : public NEMaterial
{
    Q_OBJECT

    NE_PROP_RW(double, IndexOfRefraction, "Index of Refraction", "Refractive Index", true,)
    NE_PROP_RW(double, Shininess, "Shininess", "Shininess", true,)

    public:

    NETransparentMaterial(NENodeFactory* factory);
    virtual ~NETransparentMaterial();
    virtual void updateUniforms();

protected:

private:
    double m_Shininess;

    //! The Index of Refraction.
    double m_IndexOfRefraction;

};

Q_DECLARE_INTERFACE(NETransparentMaterial, "NETransparentMaterial")
