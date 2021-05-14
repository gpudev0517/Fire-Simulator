#pragma once

#include "Physics/Spatial/Physical/Fluid/ParticleFluid/SphLiquid/NESphLiquidParticle.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"
#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"

NE_FWD_DECL_4(class, NEKeyValuePair, NESphLiquidParticle, NERigidSolver, NE_PRIVATE(NESphLiquidSolver))

//!  Interface for implementations of SPH Liquids. SPH Liquids are instantiated and handled by NESphLiquidManager.
/*!
  This class provides an interface for the framework to communicate with any instance of an Sph Liquid.
*/
class NECEXP_PHYSICS NESphLiquidSolver : public NEParticleFluidSolver
{
public:

    enum NESPHKernel
    {
        CubicBSplineKernel,
        QuarticBSplineKernel,
        QuinticBSplineKernel,
        QuinticWendlandKernel
    };

    enum NESphCohesionModel
    {
        None,
        ArtificialPressure,
        ArtPresSurfMin,
        //		Becker,
        //		BeckerNormalized,
        //		BeckerVariation,
        //		BeckerVarNormalized,
        NadirFinal,
        NadirFinalSymmetric,
        NadirPaper,
        NadirPolyNew2,
        NadirPolyNew3,
        //		NadirSpherical,
        SpringBased,
        TartaMeakin,
        SineKernel,
        AdhesionSpline,
        NadirPaperAdhesion,
        RepulsionOnly
    };


    //! For multiphase fluid handling.
    enum NESphUpdatePart
    {
        kFinish,
        kOne,
        kTwo,
        kThree,
        kFour,
        kFive,
        kCount
    };
private:
    Q_OBJECT
    NE_CLASS("SPH Liquid Solver Settings")

    NE_PROP_RW(NESphCohesionModel, CohesionModel, "Cohesion", "The active cohesion model", isCohesionModelVisible,)
    NE_PROP_RW(double, Cohesion, "Cohesion", "The cohesion coefficient", true,)
    NE_PROP_RW(NESphCohesionModel, AdhesionModel, "Adhesion Model", "The active adhesion model", isAdhesionModelVisible,)
    NE_PROP_RW(double, InterfaceTension, "Interface Tension", "Interface tension coefficient defines the strength of the interface tension force.", isInterfaceTensionVisible,)
    NE_PROP_RW(double, ColorValue, "Color Value", "The color value of the fluid", isColorValueVisible,)
    NE_PROP_RW(bool, AdaptiveTimestep, "Adaptive Timestep", "For controlling adaptive timestepping", true, virtual)
    NE_PROP_RW(double, FixedTimeStep, "Time-Step", "Sets the time-step", isFixedTimeStepVisible,)
    NE_PROP_RW(bool, FirstUpdateTimestep, "First Update Timestep", "When set to true, timestep is updated first.", isFirstUpdateTimestepVisible,)
    NE_PROP_RW(bool, CorrectDensity, "Correct Density", "Enables density correction", isCorrectDensityVisible,)
    NE_PROP_RW(bool, SmoothVelocities, "Smooth Velocities", "Enables velocity smoothing", isSmoothVelocitiesVisible,)
    NE_PROP_RW(double, BoundaryCoefficient, "Boundary Coefficient", "Sets the boundary coefficient", false,)
    NE_PROP_RW(NESPHKernel, SphKernel, "Smoothing Kernel", "Sets the smoothing kernel", true, virtual)
    NE_PROP_R(double, KernelStdDeviation, "Smoothing Kernel Std. Deviation", "Standard deviation of the smoothing kernel", true)
    NE_PROP_RW(bool, StuckPrevention, "Stuck Prevention", "Enables stuck prevention", isStuckPreventionVisible,)
    NE_PROP_RW(bool, ExtractSurfaceParticles, "Surface Particle Extraction", "Enables extraction of surface particles", isExtractSurfaceParticlesVisible,)
    Q_ENUMS(NESPHKernel NESphCohesionModel)

public:
    NESphLiquidSolver(NENodeFactory* factory);
    virtual ~NESphLiquidSolver();
    virtual uint init(uint initMode = NE::kInitOnCreate);

    //! For updating a single-phase fluid.
    virtual void update() = 0;
    //! For updating multiphase fluids.
    virtual NESphUpdatePart updateMultiPhase(NESphUpdatePart currentStep) = 0;
    //! Compute the particle normals
    virtual void computeParticleNormals(bool normalize = true);
    //! Extracts the surface particles
    virtual void extractSurfaceParticles();
    //Computes the boundary pressure
    virtual void computeBoundaryPressure(uint method = 1);
    //virtual void reorderParticles(const std::vector<IdPair>* idPairs);
    //! Compute statistics of the solver and prints it to the OSD
    virtual void computeStatistics(bool multiphase=false);
    //! Computes the acceleration of the rigid particle for multiphase simulations
    void computeRigidAccelMultiphase( NERigidSphParticle& bp );
    //! Compute and returns the optimal timestep for this solver
    virtual double computeOptimalTimeStep()
    {
        return 0.0;
    }
    //! Preprocesses, at the beginning of the time step
    virtual void preProcess(const std::vector<NERigidSolver*>& rigidSolvers);
    //! Postprocesses, at the end of the time step
    virtual void postProcess();
    //! Saves the loading of data for this simulation/animation frame.
    virtual uint saveFrameData(QString path, const int frameNumber);
    //! Calls the loading of data for this simulation/animation frame.
    virtual uint loadFrameData(QString path, const int frameNumber);
    //! Calls the loading of data for this simulation/animation frame.
    virtual uint loadFrameData(QString path, const int frameNumber, std::vector<NEParticle>& neParticles);

    //####################### Kernel functions ########################//
    float Kernel(float r);
    float KernelDeriv(float r);
    //! Cubic B-Spline Kernel
    float WCubicBSpline(const float r);
    //! Cubic B-Spline Kernel Derivative
    float WDerivCubicBSpline(const float r);
    //! Quartic B-Spline Kernel
    float WQuarticBSpline(const float r);
    //! Quartic B-Spline Kernel Derivative
    float WDerivQuarticBSpline(const float r);
    //! Quintic B-Spline Kernel
    float WQuinticBSpline(const float r);
    //! Quintic B-Spline Kernel Derivative
    float WDerivQuinticBSpline(const float r);
    //! Quintic Wendland Kernel
    float WQuinticWendland(const float r);
    //! Quintic Wendland Kernel Derivative
    float WDerivQuinticWendland(const float r);

    //! Returns the memory consumption of the particles
    virtual uint memoryConsumption() = 0;

signals:
    void updateProperties();

protected:

    //! Initialize spatial related Actions that are added to context menus //derived from NENode.
    virtual void initializeGUIActions();
    //! Computes the density (based on neighboring rigid and fluid particles) for a given fluid particle
    void computeFluidParticleDensity(NESphLiquidParticle* particle, uint index);
    float getFluidParticleDensity(const NESphLiquidParticle& particle, uint index);
    //! Computes the density of the particle neglecting the mass for multiphase physics
    void computeAdaptedDensity(NESphLiquidParticle* particle, const int particleVecId);
    //! Computes the density (based on neighboring rigid and fluid particles) for a given fluid particle
    bool computeFluidParticleDensityWithRemoval(NESphLiquidParticle* particle, uint index);
    //! Computes the densities of particles and deletes dangerous particles with high density
    void computeFluidDensitiesWithRemoval();
    //! Computes the smoothed color values used for computing interface tension in multiphase solver.
    void computeColorValues(NESphLiquidParticle* particle, const int particleVecId);
    //! Computes the normals at the interface of multiphase fluids using the smoothed color field.
    void computeInterfaceNormals(NESphLiquidParticle* particle, const int particleVecId);
    //! Computes the density (based on neighboring fluid particles) for a given rigid particle
    void computeRigidParticleDensity(NERigidSphParticle& particle);
    //! Computes the adapted density (based on neighboring fluid particles) for a given rigid particle, i.e. mass of fluid particles is neglected
    void computeAdaptedRigidParticleDensity(NERigidSphParticle& particle);
    Inline vec3f viscosityAccelContribution(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float* pWLapl);
    Inline vec3f viscosityAccelContributionMulti(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float* pWLapl, const NESphLiquidSolver* solver);
    Inline vec3f viscosityAccelContributionSolid(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float* pWLapl, float dragCoeff);
    Inline vec3f viscosityMonaghan(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float constant, float epsilon);
    Inline vec3f viscosityLaminar(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float constant);
    Inline vec3f viscosityManning(float r, float mass2, float d1, float d2, const vec3f& dv, const vec3f& dx, float* pWGrad, float dragCoeff, float constant, float epsilon);
    Inline vec3f surfaceTensionAccelContribution(float r, float mass2, const vec3f& dx, float density, float* pW, float* pWGrad, float* pWLapl, float constant);
    Inline vec3f adhesionAccelContribution(float r, float mass2, const vec3f& dx, float density, float* pW, float* pWGrad, float* pWLapl, float constant);
    Inline vec3f artificialPressure(float r, float mass2, const vec3f& dx, float* pW, float* pWGrad, float constant);
    //	Inline vec3f cohesionBecker(float r, float mass2, const vec3f& dx, float* pW, float constant);
    //	Inline vec3f cohesionBeckerNormalized(float r, float mass2, const vec3f& dx, float* pW, float constant);
    //	Inline vec3f cohesionBeckerVariation(float r, float mass2, const vec3f& dx, float* pW, float constant);
    //	Inline vec3f cohesionBeckerVarNormalized(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionSpringBased(float r, float mass2, const vec3f& dx, float constant);
    //	Inline vec3f cohesionNadirSpherical(float r, float mass2, const vec3f& dx, float constant);
    Inline vec3f cohesionNadirFinal(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionNadirFinalSymmetric(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionNadirPaper(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionNadirPolyNew2(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionNadirPolyNew3(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f cohesionTartaMeakin(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f repulsionOnly(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f sineKernel(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f adhesionSpline(float r, float mass2, const vec3f& dx, float* pW, float constant);
    Inline vec3f nadirPaperAdhesion(float r, float mass2, const vec3f& dx, float* pW, float constant);
    //!Computes the adapted pressure force
    Inline vec3f adaptedPressureForce(float r, float p1PressureOverDensity2, float p2PressureOverDensity2, const vec3f& dx, float* pWGrad);
    void initializeKernelCoefficients();
    void recomputeVelocities(const std::vector<NEParticle>& fParticles, const double Timestep);

    //################## DEFINITION OF PROPERTY METHODS ####################//

public:
    bool CohesionEnabled() const;
    //! Allocates space for the given number of particles
    virtual void resizeParticlesVector(uint nItems) = 0;
    virtual void reserveSpaceForParticles(uint nItems) = 0;
    bool createParticleSection(uint sectionId, uint particleDataType);
    bool removeParticleSection(uint sectionId);
    bool useInterfaceTension() const;
    void constrainParticles(vec3f& vel,size_t id);

public slots:
    //void loadParticlesFromBGEO(QString path);
    void loadParticlesFromPDA(QString path);
    void loadParticlesFromNEParticles(QString path);
    void setParticleInitialVelocities( vec3f vel );
    void loadParticles();
    virtual void resetProperties();

protected:
    uint computeInterRadius() override;

    uint writeSectionsCStream(std::ostream& out, const QString path, int secOffset);
    //! Internally used method to store particle data in a file
    void saveSectionDataCStream(std::ostream &sOut, uint contentFlags);
    void saveSectionDataCStream(std::ostream& sOut, std::vector<NEKeyValuePair> &vectorMap, uint contentFlags, uint startId, uint size);
    //! Internally used method to load particle data from a file
    virtual void loadSectionDataV2(std::istream& sIn, int start, int end, uint savedContentFlags, uint requiredContentFlag);
    //! Internally used method to load particle data from a file
    virtual void loadSectionDataV1(QDataStream& sIn, int start, int end, uint savedContentFlags, uint requiredContentFlag);
    uint openFrameFileV1(QString path, const int frameNumber, QFile &fIn, QDataStream &sIn, std::vector<NEParticle>& neParticles);
    uint openFrameFileV2(QString path, const int frameNumber, std::ifstream &sIn, std::vector<NEParticle>& neParticles);
    uint loadAllSectionsV1(QString path, QString fileName, QDataStream &sIn, std::vector<NEBinaryDataSection>& dataSections, std::vector<NEParticle>& neparticles );
    uint loadAllSectionsV2(QString path, QString fileName, std::ifstream &sIn, std::vector<NEBinaryDataSection>& dataSections, std::vector<NEParticle>& neParticles );
    //! Internally used method to load particle data from a file to  neParticles std::vector
    void loadSectionDataV2(std::istream &sIn, int start, int end, uint contentFlags,  std::vector<NEParticle>& neParticles);
    void loadSectionDataV1(QDataStream& sIn, int start, int end, uint contentFlags,  std::vector<NEParticle>& neParticles);
    inline void computePressure(NESphLiquidParticle* particle);
    inline void computePressure(NERigidSphParticle* particle);
    inline void computeMultiPressure(NESphLiquidParticle* particle);

    //! //! Computes the acceleration of the rigid particle for singlephase simulations
    void computeRigidAccel( NERigidSphParticle& bp );
    Inline float WSpikyGradFull(const float r);
    void computeManningVisCte();
    void computeVisEps();
    virtual bool isCohesionModelVisible();
    virtual bool isAdhesionModelVisible();
    virtual bool isInterfaceTensionVisible();
    virtual bool isColorValueVisible();
    bool isFixedTimeStepVisible();
    virtual bool isFirstUpdateTimestepVisible();
    virtual bool isCorrectDensityVisible();
    virtual bool isSmoothVelocitiesVisible();
    virtual bool isStuckPreventionVisible();
    virtual bool isExtractSurfaceParticlesVisible();

    //################## General SPH Variables ####################//

    double m_WCubicBSplineCoeff;
    double m_WDerivCubicBSplineCoeff;
    double m_WQuarticBSplineCoeff;
    double m_WDerivQuarticBSplineCoeff;
    double m_WQuinticBSplineCoeff;
    double m_WDerivQuinticBSplineCoeff;
    double m_WQuinticWendlandCoeff;
    double m_WDerivQuinticWendlandCoeff;
    double m_WZero;

    double m_CohesionKernelTerm;
    double m_CohesionNadirFinalFactor;
    double m_CohesionPolyNew2Term;
    double m_AdhesionTerm;

    std::vector<NEFluidParticle*> m_StuckParticles;
    NESPHKernel m_SphKernel = CubicBSplineKernel;
    NESphCohesionModel m_CohesionModel = NadirPaper;
    NESphCohesionModel m_AdhesionModel = NadirPaperAdhesion;
    double m_SoundSpeedSquare;
    bool m_ExtractSurfaceParticles = true;
    bool m_AdaptiveTimestep = true;

    double m_ManningVisCte;
    double m_VisEps;

private:
    NE_DECL_PRIVATE(NESphLiquidSolver)
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(NESphLiquidSolver::ParticleDataTypes)


Q_DECLARE_INTERFACE(NESphLiquidSolver, "NESphLiquidSolver")

