#pragma once

#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NENSphFluidSolver))

/*!
    Base class of Neutrino SPH solvers
*/

class NECEXP_PHYSICS NENSphFluidSolver : public NESphLiquidSolver
{
    Q_OBJECT
    NE_CLASS("Neutrino SPH Fluid Solver Settings")

public:

    enum BuoyancyModelType
    {
        Isothermal = 0,
        BoussinesqApproximation = 1
    };
    enum DensityComputationType
    {
      Interpolation = 0,
      MassEquation = 1
    };
    enum SphLaplacienForVisType
    {
        AngularMomentumPreserving = 0,
        VelocityDifferenceCollinear = 1,
        VariableViscosityConsistent = 2
    };
    enum class WallExtrapolType
    {
        Shepard = 0,
        MovingLinearLeastSquares = 1
    };
    enum class LeastSquareResolutionType
    {
        QR = 0,
        SVD = 1,
        NormalEquation = 2,
        Band2018 = 3
    };
    enum class OpenBoundaryExtrapolType
    {
        Shepard = 0,
        MovingLinearLeastSquares = 1
    };

    NE_PROP_RW(BuoyancyModelType, BuoyancyModel, "Buoyancy Model", "Buoyancy modeling", true, virtual)
    NE_PROP_RW(bool, SurfaceTensionModel, "Surface Tension Model", "Surface Tension modeling", true, )
    NE_PROP_GROUP_START(DensityHandling, "Density Handling", "Density handling settings", true)
    NE_PROP_RW(DensityComputationType, DensityComputation, "Density Computation", "Method for computing the density", true, )
    NE_PROP_GROUP_END(DensityHandling)
    NE_PROP_GROUP_START(PressureHandling, "Pressure Handling", "Pressure handling settings", true)
    NE_PROP_RW(double, BackgroundPressure, "Background Pressure", "Background positive pressure, to avoid negative pressures", true, )
    NE_PROP_RW(bool, EnableNegativePressures, "Enable Negative Pressures", "Decides whether to enable negative pressures", true, )
    NE_PROP_RW(double, MinAllowedNegativePressure, "Min. Allowed Negative Pressure", "Minimum allowed pressure", true, )
    NE_PROP_RW(bool, TensileInstabilityControl, "Tensile Instability Control", "Decides whether to use a non-conservative pressure gradient to prevent occurrence of tensile instability", true, )
    NE_PROP_RW(bool, SolidNegativePressureControl, "Solid Negative Pressure Control", "Decides whether to allow negative pressure on solid particle close to free surface", true, )
    NE_PROP_GROUP_END(PressureHandling)
    NE_PROP_GROUP_START(VisHandling, "Viscosity Handling","Viscosity handling settings", true)
    NE_PROP_RW(SphLaplacienForVisType, SphLaplacienForVis, "SPH Laplacien for Viscosity", "SPH Laplacien operator used for discretizing the viscous term", true, )
    NE_PROP_GROUP_END(VisHandling)
    NE_PROP_GROUP_START(WallHandlingGroup, "Wall Boundary Handling", "Wall boundary handling settings", true )
    NE_PROP_RW(WallExtrapolType, WallExtrapol, "Wall Boundary Extrapolation", "Extrapolation method for the wall boundary handling", true, )
    NE_PROP_RW(LeastSquareResolutionType, LeastSquareResolution, "Least square resolution method", "Least square resolution method for MLS extrapolation", true, )
    NE_PROP_RW(bool, WallHydrostaticCorrection, "Wall Hydrostatic Pressure Correction", "Decides whether to apply the hydrostatic correction when enforcing wall pressure boundary conditions", true, )
    NE_PROP_RW(bool, WallViscousCorrection, "Wall Viscous Correction", "Decides whether to apply the viscous correction when enforcing the wall pressure boundary conditions", true, )
    NE_PROP_GROUP_END(WallHandlingGroup)
    NE_PROP_GROUP_START(FreeSurfaceHandlingGroup, "Free-Surface Handling", "Free-surface handling settings", true)
    NE_PROP_RW(bool, FreeSurfaceDensityCorrection, "Free-Surface Density Correction", "Decides whether to correct the densities of fluid particles near the free-surface", true, )
    NE_PROP_RW(bool, FreeSurfacePressureCorrection, "Free-Surface Pressure Correction", "Decides whether to correct the pressures of fluid particles near the free-surface", true, )
    NE_PROP_RW(double, NearFreeSurfaceIdentificationCoeff, "Near-Free-Surface Identification Coeff.", "Affects the identification of fluid particles near the free-surface", true, )
    NE_PROP_GROUP_END(FreeSurfaceHandlingGroup)
    NE_PROP_GROUP_START(OpenBoundaryHandlingGroup, "Open Boundary Handling", "Open boundary handling settings", true )
    NE_PROP_RW(OpenBoundaryExtrapolType, OpenBoundaryExtrapol, "Open Boundary Extrapolation", "Extrapolation method for the open boundary handling", true, )
    NE_PROP_GROUP_END(OpenBoundaryHandlingGroup)
    NE_PROP_GROUP_START(AdaptiveTimeStepping, "Adaptive Time-Stepping", "Adaptive time-stepping settings", true)
    NE_PROP_RW(double, LambdaCFL, "CFL Number", "Affects the adaptive time-stepping scheme", true, virtual)
    NE_PROP_RW(double, LambdaVis, "Time-Step Diffusion Coeff.", "Affects the adaptive time-stepping scheme", true, virtual)
    NE_PROP_RW(bool, ClampToMultipleSolidSolverTimeStep, "Clamp to Multiple of Solid-Solver Time-Step", "Decides whether to clamp the fluid-solver time-step to make it a multiple of the solid-solver time-step (only one rigid-body solver considered)", true, )
    NE_PROP_GROUP_END(AdaptiveTimeStepping)
    NE_PROP_RW(bool, ClampSpeed, "Clamp Speed", "Decides whether to clamp the fluid particle speed", true, )
    NE_PROP_RW(double, MaxAllowedSpeed, "Max. Allowed Speed", "Maximum allowed speed", true, )
    NE_PROP_RW(QString, DaemonName, "Daemon Name", "Name of the daemon that handles open boundary particles", true, )

    Q_ENUMS(BuoyancyModelType DensityComputationType SphLaplacienForVisType WallExtrapolType OpenBoundaryExtrapolType LeastSquareResolutionType)

public:
    NENSphFluidSolver(NENodeFactory* factory);
    ~NENSphFluidSolver();

    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

    void preProcess(const std::vector<NERigidSolver*>& rigidSolvers) override;

    void computeStatistics(bool multiphase=false) override;

    uint connectToFluid(NEFluid* fluid) override;
    uint disconnectFromFluid(NEFluid* fluid) override;

public slots:
    uint SetParticleSize(const double& val) override;
    uint SetInterRadiusToPartSizeRatio(const double& val) override;
    uint SetSphKernel(const NESPHKernel& val) override;

    void restDensityChangedCallback() override;
    void kinematicViscosityChangedCallback() override;
    void thermalDiffusivityChangedCallback() override;
    void diffusivitiesChemSpeciesChangedCallback() override;

protected:
    uint computeParticleVolumeMass() override;

    //! Return half the particle mass
    const double& halfParticleMass() const;
    //! Return five times the particle mass
    const double& fiveTimesParticleMass() const;
    //! Return the square of the particle mass
    const double& squaredParticleMass() const;
    //! Return the sum of product of mass and kernel gradient of prototype particle
    const vec3d& sumMassKernelGradPrototype() const;

    //! Return the ratio between background pressure and squared rest-density
    const double& backgroundPressureOverSquaredRestDensity() const;
    //! Return the squared maximum allowed speed
    const double& squaredMaxAllowedSpeed() const;

    //! Compute the non-pressure accelerations and temperature rate-of-change over all fluid particles
    void computeNonPressureAccelAndTemperatureRateOfChangeAll(std::vector<NESphLiquidParticle>& fParticles);

    //! Compute the fluid-to-solid forces onto all solid particles
    void computeFluidToSolidForcesAll();

    //! Compute the fluid-to-solid forces onto the solid particle
    void computeFluidToSolidForces(NERigidSphParticle& p, size_t index);

    //! Compute the kernel gradients of the fluid particle
    void computeKernelGrads(NESphLiquidParticle& p, size_t index);
    //! Identify whether the fluid particle is near a boundary
    void classifyIfNearBoundary(NESphLiquidParticle& p, size_t index);
    //! Compute the density (using the interpolation) of the fluid particle
    void computeDensityInterp(NESphLiquidParticle& p, size_t index);
    //! Compute the density (using the mass equation) of the fluid particle
    void computeDensityMassEq(NESphLiquidParticle& p, size_t index);

    //! Compute the normal vector of the fluid particle
    void computeNormal(NESphLiquidParticle& p, size_t index);
    //! Compute the acceleration induced by the surface tension forces on the fluid particle
    vec3f getSurfaceTensionAcc(NESphLiquidParticle& p, size_t index);

    //! Compute the acceleration induced by the viscosity forces on the fluid particle,
    //! assuming constant viscosity and using the angular-momentum preserving formulation
    vec3f getAngMomentumPreservingVisAccConstVis(NESphLiquidParticle& p, size_t index);
    //! Compute the acceleration induced by the viscosity forces on the fluid particle,
    //! assuming constant viscosity and using the velocity-difference collinear formulation
    vec3f getVelDiffCollinearConstVis(NESphLiquidParticle& p, size_t index);
    //! Compute the acceleration induced by the viscosity forces on the fluid particle,
    //! assuming constant viscosity and using the variable-viscosity consistent formulation
    vec3f getVarVisConsistentVisAccConstVis(NESphLiquidParticle& p, size_t index);

    //! Compute the rate-of-changes of chemical species concentrations of the fluid particle
    void computeConcnsChemSpeciesRateOfChanges(NESphLiquidParticle& p, size_t index);
    //! Compute the rate-of-change of temperature of the fluid particle
    void computeTempRateOfChange(NESphLiquidParticle& p, size_t index);
    //! Compute the rate-of-changes of chemical species concentrations and rate-of-change of temperature of the fluid particle with respect to chemical species
    void computeConcnsChemSpeciesAndTempRateOfChanges(NESphLiquidParticle& p, size_t index);

    //! Assign the rigid body velocity and rigid body acceleration to the velocity and acceleration fields of the solid particles
    void assignRigidBodyVelocityAndAccelToSolidParticles();
    //! Extrapolate the density and velocity fields to the solid particles from the neighboring fluid particles
    //! Treatment for a solid particle differs depending on the wall boundary setting (no-slip or free-slip) of the solid object
    void extrapolateDensityAndSlipVelocityToSolidParticles();

    //! Extrapolate the no-slip velocity field to the solid particle from neighboring fluid particles using Shepard method
    void extrapolateNoSlipVelocityShepard(NERigidSphParticle& p);
    //! Extrapolate the no-slip velocity field to the solid particle from neighboring fluid particles using moving linear least-squares
    void extrapolateNoSlipVelocityMLS(NERigidSphParticle& p);
    //! Extrapolate the free-slip velocity field to the solid particle from neighboring fluid particles using Shepard method
    void extrapolateFreeSlipVelocityShepard(NERigidSphParticle& p);
    //! Extrapolate the free-slip velocity field to the solid particle from neighboring fluid particles using moving linear least-squares
    void extrapolateFreeSlipVelocityMLS(NERigidSphParticle& p);
    //! Extrapolate the density field to the solid particle from neighboring fluid particles using Shepard method
    void extrapolateDensityShepard(NERigidSphParticle& p);
    //! Extrapolate the density field to the solid particle from neighboring fluid particles using moving linear least-squares
    void extrapolateDensityMLS(NERigidSphParticle& p);
    //! Extrapolate the pressure field to the solid particle from neighboring fluid particles using Shepard method
    void extrapolatePressureShepard(NERigidSphParticle& p);
    //! Extrapolate the pressure field to the solid particle from neighboring fluid particles using moving linear least-squares
    void extrapolatePressureMLS(NERigidSphParticle& p);
    //! Extrapolate the temperature field to the adiabatic solid particles from the neighboring fluid particles
    void extrapolateTemperatureToAdiabaticSolidParticles();
    //! Extrapolate the temperature field to the solid particle from neighboring fluid particles using Shepard method
    void extrapolateTemperatureShepard(NERigidSphParticle& p);
    //! Extrapolate the temperature field to the solid particle from neighboring fluid particles using moving linear least-squares
    void extrapolateTemperatureMLS(NERigidSphParticle& p);

    //! Extrapolate the density field to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolateDensityShepard(NESphLiquidParticle& p, size_t index);
    //! Extrapolate the pressure field to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolatePressureShepard(NESphLiquidParticle& p, size_t index);
    //! Extrapolate the velocity field to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolateVelocityShepard(NESphLiquidParticle& p, size_t index);
    //! Extrapolate the temperature field to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolateTemperatureShepard(NESphLiquidParticle& p, size_t index);
    //! Extrapolate the chemical species concentration fields to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolateConcnsChemSpeciesShepard(NESphLiquidParticle& p, size_t index);
    //! Extrapolate the normal field to the fluid particle from neighboring fluid particles using Shepard method
    void extrapolateNormalShepard(NESphLiquidParticle& p, size_t index);

    //! Return the array of rate-of-changes of chemical species concentrations of all fluid particles
    std::vector<std::array<double, maxNumOfChemSpecies>>& concnsChemSpeciesRateOfChanges();
    //! Return the array of rate-of-changes of temperature of all fluid particles
    std::vector<double>& temperatureRateOfChanges();

    //! Return the squared maximum speed over all particles
    double squaredMaxSpeed() const;
    double& squaredMaxSpeed();
    //! Return the squared maximum acceleration magnitude over all particles
    double squaredMaxAccelMag() const;
    double& squaredMaxAccelMag();
    //! Return the minimum temperature over all particles
    double minTemperature() const;
    double& minTemperature();
    //! Return the maximum temperature over all particles
    double maxTemperature() const;
    double& maxTemperature();

    //! Return the maximum time-step satisfying the CFL condition in the case of a non-accelerated motion
    double getCFLSpeedTimeStep(double speed);
    //! Return the maximum time-step satisfying the CFL condition in the case of a pure acceleration-induced motion
    double getCFLAccelMagTimeStep(double accelMag);
    //! Return the maximum time-step satisfying the diffusivity condition
    double getDiffTimeStep(double diffusivity);
    //! Return the time step corresponding to the frame rate
    double getFrameRateTimeStep();
    //! Return the optimal time-step selected by the adaptive time-stepping scheme
    virtual double getOptimalTimeStep();

    //! Update the time step
    virtual void updateTimeStep();
    //! Return the time step clamped to the largest multiple of the solid-solver time-step not greater than timeStep
    double getClampedTimeStepToMultipleSolidSolverTimeStep(double timeStep);

    //! Compute the coefficients for the SPH formulations of the diffusion terms
    void computeConstViscCoeffs(double kinematicViscosity);

private:
    NE_DECL_PRIVATE(NENSphFluidSolver)
};
