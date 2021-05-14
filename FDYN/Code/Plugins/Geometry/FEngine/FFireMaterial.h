#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

class FFireMaterialPrivate;
/*!
  Class defining a solid and its physical properties.
*/

class  FFireMaterial : public NENode
{
    Q_OBJECT
    NE_CLASS("Fire Material")

public:

    NE_PROP_RW(int, ID, "Fire Material ID", "Fire Material ID", true, )
    NE_PROP_RW(int, CarbonAtoms, "Carbon Items", "Number of Carbon Items", true, )
    NE_PROP_RW(int, HydrogenAtoms, "Hydrogen Atoms", "Number of Hydrogen Atoms", true, )
    NE_PROP_RW(int, OxygenAtoms, "Oxygen Atoms", "Number of Oxygen Atoms", true, )
    NE_PROP_RW(int, NitrogenAtoms, "Nitrogen Atoms", "Number of Nitrogen Atoms", true, )
    NE_PROP_RW(int, ChlorineAtoms, "Chlorine Atoms", "Number of Chlorine Atoms", true, )
    NE_PROP_RW(double, HeatCombustion, "Heat Combustion", "Heat Combustion", true, )
    NE_PROP_RW(double, RadiativeFraction, "Radiative Fraction", "Radiative Fraction", true, )

public:
    FFireMaterial(NENodeFactory* factory);
    ~FFireMaterial();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;



signals:

private:
    NE_DECL_PRIVATE(FFireMaterial)
};

class FFireMaterialFactory : public NENodeFactory
{
public:
    FFireMaterialFactory(NEManager* m);
    virtual ~FFireMaterialFactory();
    NE_DERIVED(NENodeFactory, Public)
};
