#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_4(class, NE_PRIVATE(NEFluid), NEChemSpecies, NEMixture, NEFluidSolver)

/*!
  Class defining a fluid and its physical properties.
*/

class NECEXP_PHYSICS NEFluid : public NENode
{
    Q_OBJECT
    NE_CLASS("Fluid Physical Properties")

public:
    enum FluidPresetType {NoPreset = 0,
                          Water_293K = 1,
                          Air_293K = 2};

    NE_PROP_RW(FluidPresetType, FluidPreset, "Fluid Preset", "Preset of the physical properties of the fluid", true, )
    NE_PROP_RW(double, RefTemperature, "Ref. Temperature", "Temperature at which the physical properties of the fluid are defined", true, )
    NE_PROP_RW(double, RestDensity, "Rest Density", "Rest density of the fluid", true, )
    NE_PROP_RW(double, SoundSpeed, "Sound Speed", "Sound speed of the fluid", true, )
    NE_PROP_RW(double, KinematicViscosity, "Kinematic Viscosity", "Kinematic viscosity of the fluid", true, )
    NE_PROP_R(double, DynamicViscosity, "Dynamic Viscosity", "Dynamic viscosity of the fluid", true)
    NE_PROP_RW(double, IsobaricHeatCapacity, "Isobaric Specific Heat Capacity", "Mass heat capacity at constant pressure of the fluid", true, )
    NE_PROP_RW(double, IsochoricHeatCapacity, "Isochoric Specific Heat Capacity", "Mass heat capacity at constant volume of the fluid", true, )
    NE_PROP_R(double, AdiabaticIndex, "Adiabatic Index", "Ratio of isobaric heat capacity to isochoric heat capacity of the fluid", true)
    NE_PROP_RW(double, ThermalDiffusivity, "Thermal Diffusivity", "Thermal diffusivity of the fluid", true, )
    NE_PROP_R(double, ThermalConductivity, "Thermal Conductivity", "Thermal conductivity of the fluid", true)
    NE_PROP_R(double, PrandtlNumber, "Prandtl Number", "Ratio of kinematic viscosity to thermal diffusivity of the fluid", true)
    NE_PROP_RW(double, ThermalExpansionCoeff, "Thermal Expansion Coeff.", "Coefficient of thermal expansion of the fluid", true, )
    NE_PROP_GROUP_START(DiffusivitiesChemSpecies, "Diffusivities Chemical Species", "Diffusivities of chemical species diluted in the fluid", true)
    NE_PROP_RW(double, DiffusivityChemSpecies1, "Diffusivity Chemical Species 1", "Diffusivity of chemical species 1 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies2, "Diffusivity Chemical Species 2", "Diffusivity of chemical species 2 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies3, "Diffusivity Chemical Species 3", "Diffusivity of chemical species 3 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies4, "Diffusivity Chemical Species 4", "Diffusivity of chemical species 4 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies5, "Diffusivity Chemical Species 5", "Diffusivity of chemical species 5 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies6, "Diffusivity Chemical Species 6", "Diffusivity of chemical species 6 diluted in the fluid", true, )
    NE_PROP_RW(double, DiffusivityChemSpecies7, "Diffusivity Chemical Species 7", "Diffusivity of chemical species 7 diluted in the fluid", true, )
    NE_PROP_GROUP_END(DiffusivitiesChemSpecies)

    Q_ENUMS(FluidPresetType)

public:
    NEFluid(NENodeFactory* factory);
    ~NEFluid();

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

    //! Returns the list of connected fluid solvers
    std::vector<NEFluidSolver*> connectedFluidSolvers() const;
    //! Connects to a fluid solver
    uint connectToFluidSolver(NEFluidSolver* fluidSolver);
    //! Disconnects from a fluid solver
    uint disconnectFromFluidSolver(NEFluidSolver* fluidSolver);

signals:
    void connectedChemSpeciesChanged();

private:
    NE_DECL_PRIVATE(NEFluid)
};

class NEFluidFactory : public NENodeFactory
{
public:
    NEFluidFactory(NEManager* m);
    virtual ~NEFluidFactory();
    NE_DERIVED(NENodeFactory, Public)
};
