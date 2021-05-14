#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

class NEPhysicalMaterialPrivate;
/*!
  Class defining a solid and its physical properties.
*/

class  NEPhysicalMaterial : public NENode
{
    Q_OBJECT
    NE_CLASS("Physical Material")

public:

    enum PhysicalPresetType
    {
        NoPreset = 0,
        Gypsum = 1,
        Concrete,
        Wood
    };

    NE_PROP_RW(PhysicalPresetType, PhysicalPreset, "Material Preset", "Physical Property Preset", true, )
    NE_PROP_RW(int, ID, "Material ID", "Material ID", true, )
    NE_PROP_RW(double, RefTemperature, "Ref. Temperature", "Temperature at which the physical properties of the solid are defined", true, )
    NE_PROP_RW(double, Density, "Density", "Density of the solid", true, )
    NE_PROP_RW(double, Thickness, "Thickness", "Material Thickness", true, )
    NE_PROP_RW(double, SpecificHeat, "Specific Heat Capacity", "Mass heat capacity", true, )
    NE_PROP_RW(double, ThermalConductivity, "Thermal Conductivity", "Thermal conductivity", true, )
    NE_PROP_RW(double, Emissivity, "Emissivity", "Emission Control", true, )

    Q_ENUMS(PhysicalPresetType)

public:
    NEPhysicalMaterial(NENodeFactory* factory);
    ~NEPhysicalMaterial();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

signals:

private:
    NE_DECL_PRIVATE(NEPhysicalMaterial)
};

class NEPhysicalMaterialFactory : public NENodeFactory
{
public:
    NEPhysicalMaterialFactory(NEManager* m);
    virtual ~NEPhysicalMaterialFactory();
    NE_DERIVED(NENodeFactory, Public)
};
