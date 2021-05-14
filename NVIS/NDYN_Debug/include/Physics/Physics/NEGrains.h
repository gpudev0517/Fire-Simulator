#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NEGrains), NEChemSpecies, NEMixture)

/*!
  Class defining a collection of grains and their physical properties.
*/

class NECEXP_PHYSICS NEGrains : public NENode
{
    Q_OBJECT
    NE_CLASS("Grains Physical Properties")

public:
    NE_PROP_RW(double, RestDensity, "Rest Density", "Rest density of the grains", true, )
    NE_PROP_RW(double, GrainDiameter, "Grain Diameter", "Mean diameter of the grains", true, )
    NE_PROP_RW(double, RefElasticityModulus, "Ref. Elasticity Modulus", "Reference elasticity modulus of the grains", true, )
    NE_PROP_RW(double, CompactionModulus, "Compaction Modulus", "Compaction modulus of the grains", true,)
    NE_PROP_RW(double, PackedCompVolumeFrac, "Packed Complementary Volume Fraction", "Complementary volume-fraction at maximum-packed state of the grains", true, )
    NE_PROP_RW(double, HeatCapacity, "Specific Heat Capacity", "Mass heat capacity of the grains", true, )
    NE_PROP_RW(double, ThermalDiffusivity, "Thermal Diffusivity", "Thermal diffusivity of the grains", true, )
    NE_PROP_R(double, ThermalConductivity, "Thermal Conductivity", "Thermal conductivity of the grains", true)

public:
    NEGrains(NENodeFactory* factory);
    ~NEGrains();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Returns the list of connected chemical species
    std::array<NEChemSpecies*, maxNumOfChemSpecies> connectedChemSpecies() const;
    //! Connects to a chemical species
    uint connectToChemSpecies(NEChemSpecies* chemSpecies, uint connOrder);
    //! Disconnects from a chemical species
    uint disconnectFromChemSpecies(NEChemSpecies* chemSpecies);

    //! Returns the list of connected mixtures
    std::vector<NEMixture*> connectedMixtures() const;
    //! Connects to a mixture
    uint connectToMixture(NEMixture* mixture);
    //! Disconnects from a mixture
    uint disconnectFromMixture(NEMixture* mixture);

signals:
    void connectedChemSpeciesChanged();

private:
    NE_DECL_PRIVATE(NEGrains)
};

class NEGrainsFactory : public NENodeFactory
{
public:
    NEGrainsFactory(NEManager* m);
    virtual ~NEGrainsFactory();
    NE_DERIVED(NENodeFactory, Public)
};
