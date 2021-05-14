#pragma once

#include "Rendering/Resource/NEMaterial.h"
#include "Base/NENodeFactory.h"

//NE_FWD_DECL(class,NE_PRIVATE(NEStructureMaterial))
class NEStructureMaterialPrivate;

class NEStructureMaterial : public NEMaterial

{
    Q_OBJECT

public:
    enum Materials
    {
        Matte = 0,
        Metal = 1,
        Plastic = 2
    };

    NE_PROP_RW(Materials, MaterialType, "Material Type", "Type of material", true,)
    NE_PROP_RW(double, Roughness, "Roughness", "Roughness of the surface", isItNotMatte,)
    NE_PROP_RW(QColor, SpecularColor, "Specular Color", "Specular color of the surface", isItPlastic,)

    Q_ENUMS(Materials)

public:
    NEStructureMaterial(NENodeFactory* factory);
    ~NEStructureMaterial();

private:
    NE_DECL_PRIVATE(NEStructureMaterial)
    bool isItNotMatte();
    bool isItPlastic();
};

class NEStructureMaterialFactory : public NENodeFactory
{
public:
    NEStructureMaterialFactory(NEManager* m);
    ~NEStructureMaterialFactory();
    NE_DERIVED(NENodeFactory,Public)
};
