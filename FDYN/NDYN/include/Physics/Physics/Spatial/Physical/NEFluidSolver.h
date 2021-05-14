#pragma once

#include "Physics/Spatial/NEPhysical.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NEFluidSolver), NEFluid, NEMixture)

class NECEXP_PHYSICS NEFluidSolver : public NEPhysical
{
    Q_OBJECT
    NE_CLASS("Fluid Rendering Settings")

public:
    NEFluidSolver(NENodeFactory* factory);
    virtual ~NEFluidSolver();

    virtual uint init(uint initMode) override;
    virtual void reset() override;
    virtual void resetProperties() override;

    virtual void propertyTweak(QtPropertyBase* p) override;
    //! For accessing the particles by other instances/plugins
    virtual const NEVectorMetaData* particlesVector() const = 0;
    //! Return the radius of a marker particle
    virtual double particleRadius() const = 0;
    virtual double particleRenderScale() const = 0;
    virtual NESpatialType spatialType();

    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Return the list of fluids
    std::vector<NEFluid*> connectedFluids() const;
    //! Connect to a fluid
    virtual uint connectToFluid(NEFluid* fluid);
    //! Disconnect from a fluid
    virtual uint disconnectFromFluid(NEFluid* fluid);

    //! Return the list of mixtures
    std::vector<NEMixture*> connectedMixtures() const;
    //! Connect to a mixture
    virtual uint connectToMixture(NEMixture* mixture);
    //! Disconnect from a mixture
    virtual uint disconnectFromMixture(NEMixture* mixture);

    //! Return the reference temperature used for the single fluid case
    double refTemperatureSingle() const;
    //! Return the rest density used for the single fluid case
    double restDensitySingle() const;
    //! Return the sound speed used for the single fluid case
    double soundSpeedSingle() const;
    //! Returns the kinematic viscosity used for the single fluid case
    double kinematicViscositySingle() const;
    //! Return the dynamic viscosity used for the single fluid case
    double dynamicViscositySingle() const;
    //! Return the isobaric heat capacity used for the single fluid case
    double isobaricHeatCapacitySingle() const;
    //! Return the isochoric heat capacity used for the single fluid case
    double isochoricHeatCapacitySingle() const;
    //! Return the adiabatic index used for the single fluid case
    double adiabaticIndexSingle() const;
    //! Return the thermal diffusivity used for the single fluid case
    double thermalDiffusivitySingle() const;
    //! Return the thermal conductivity used for the single fluid case
    double thermalConductivitySingle() const;
    //! Return the Prandlt number used for the single fluid case
    double prandtlNumberSingle() const;
    //! Return the thermal expansion coefficient for the single fluid case
    double thermalExpansionCoeffSingle() const;
    //! Return the diffusivities of chemical species for the single fluid case
    std::array<double, maxNumOfChemSpecies> diffusivitiesChemSpeciesSingle() const;

public slots:
    virtual void refTemperatureChangedCallback();
    virtual void restDensityChangedCallback();
    virtual void soundSpeedChangedCallback();
    virtual void kinematicViscosityChangedCallback();
    virtual void dynamicViscosityChangedCallback();
    virtual void isobaricHeatCapacityChangedCallback();
    virtual void isochoricHeatCapacityChangedCallback();
    virtual void adiabaticIndexChangedCallback();
    virtual void thermalDiffusivityChangedCallback();
    virtual void thermalConductivityChangedCallback();
    virtual void prandtlNumberChangedCallback();
    virtual void thermalExpansionCoeffChangedCallback();
    virtual void diffusivitiesChemSpeciesChangedCallback();

    virtual void connectedFluidsChangedCallback();
    virtual void connectedGrainsChangedCallback();
    virtual void connectedChemSpeciesChangedCallback();

    virtual void editCachingMode() override;

signals:
    void connectedFluidsChanged();
    void connectedGrainsChanged();
    void connectedChemSpeciesChanged();
    void updateDrawingBuffers();

protected:
    NE_DERIVED(NESpatial, Protected)

    virtual void initializeGUIActions() override;
    virtual bool positionVisible() override;
    virtual bool orientationVisible() override;
    virtual bool scaleVisible() override;
    virtual bool pivotVisible() override;

private:
    NE_DECL_PRIVATE(NEFluidSolver)
};

Q_DECLARE_INTERFACE(NEFluidSolver, "NEFluidSolver")
