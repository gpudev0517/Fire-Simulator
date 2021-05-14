#pragma once

#include "Physics/Spatial/Physical/Rigid/NEParticleRigid.h"
#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NEPciSphRigid.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NESimpleSphRigid.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NESphRigid), NEParticleKiller, NENodeFactory)

//!  Implements a simple particle based rigid body object as we had in Neutrino2 (old framework).
/*!
  Currently used to test the design of the framework to handle collision and physic computation among different entities.
*/
class NECEXP_PHYSICS NESphRigid : public NEParticleRigid, protected NEPciSphRigid, NESimpleSphRigid
{
    Q_OBJECT
    NE_CLASS("Particle Sampled Rigid Options")

public:
    enum SurfaceOffsetType
    {
        NoOffset = 0,
        InteriorOffset = 1,
        ExteriorOffset = 2
    };
    struct ParticleForceDecomposition
    {
        vec3f totalForce;
        vec3f pressureForce;
        vec3f frictionForce;
        vec3f adhesionForce;
    };

    Q_INTERFACES(NEParticleRigid)
    NE_PROP_RW(SurfaceOffsetType, SurfaceOffset, "Particle Surface-Offset", "Tells how the boundary particles shoud be offset from the rigid surface", true, virtual)
    NE_PROP_RW(bool, VolumeSampling, "Volume Sampling", "Decides whether using a volume-based particle sampling, ideal for closed mesh", true, )
    NE_PROP_RW(bool, SuperSampling, "Supersampling", "Decides whether using supersampling, to reduce the aliasing", true, )
    NE_PROP_RW(bool, OptimalNumParticleLayers, "Optimal Num. of Particle Layers", "Optimal number of layers of boundary particles", true, )
    NE_PROP_RW(int, NumParticleLayers, "Num. of Particle Layers", "Number of layers of boundary particles", true, )
    NE_PROP_GROUP_START(ExportParticleData, "Export Particle Data", "Exports particle data to a csv file", isExportParticleDataVisible)
    NE_PROP_RW(bool, ExportParticleForce, "Export Particle Force", "Exports the force applied on all particles", true, )
    NE_PROP_RW(bool, ExportParticleForceDecomposition, "Export Particle Force Decomposition", "Exports the decomposition of the force (fluid pressure & fluid friction) applied on all particles", true, )
    NE_PROP_RW(FilePath, ExportParticlePath, "Export Particle File Path", "Sets the path to export the particle data", areParticleDataToBeExported, )
    NE_PROP_GROUP_END(ExportParticleData)

    Q_ENUMS(SurfaceOffsetType)

public:
    NESphRigid(NENodeFactory* factory);
    virtual ~NESphRigid();
    virtual uint init(uint initMode = NE::kInitOnCreate) override;
    void reset() override;
    void resetProperties() override;

    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    void updateForces() override;
    void syncRigid() override;
    void colorizeParticles() override;
    virtual void setRigidParticleType(NEParticleFluidSolver::NERigidParticleType val) override;

    //! Returns the address of the particle neighborhood std::vector. This should be used by the collider for writing out neighbors.
    virtual std::vector<std::vector<Neighbor>>& liquidParticleNeighbors();

    //! Cubic Spline Kernel
    virtual void addNeighbor(Neighbor& n, uint index);
    virtual std::vector<Neighbor>* neighborVector(uint index) override;

    //! Remove neighbor with particleToRemove from liquid neighbor std::vector at index
    virtual void removeNeighbor(NEParticle* particleToRemove, uint index);

    //! DEBUG method for initial density coloring
    void colorizeDensities();
    void colorizeActiveParticles();
    void computeStatistics() override;
    void clearBoundaryParticles();

    // Reinitialize the values set to boundary particles
    // It does not reset the positions
    void resetBoundaryParticles();

    float Kernel(float r);
    const double& surfaceOffsetValue() const;
    void resetParticleForces();
    void addParticleForces(int pId, const vec3f& pTotalForce, const vec3f& pPressureForce, const vec3f& pFrictionForce, const vec3f& pAdhesionForce);
    const std::map<uint, ParticleForceDecomposition>& particleForces() const;
    void computeKernelSums();

    void prepareForSimulation() override;

    void getFluidSolverInfo(double partSize, double interRadius);

    //! Returns the list of connected killers
    std::forward_list<NEParticleKiller*> connectedKiller() const;
    //! Connects to a killer
    uint connectToKiller(NEParticleKiller* killer);
    //! Disconnects from a killer
    uint disconnectFromKiller(NEParticleKiller* killer);

    virtual uint SetSwapUpAxes(const bool& val);

    void updateTransform() override;

public slots:
	//!
    virtual void regenerateParticles();
    uint SetBehavior(const NENodeState& val) override;
    uint SetPosition(const vec3f& Position) override;
    uint SetOrientation(const eulerf& rotation) override;
    uint SetScale(const vec3f& val) override;
    virtual uint SetColor(const QColor& Color);
    bool densityInitialized();
    void setDensityInitialized(bool b);
    bool particlesInCollider();
    void setParticlesInCollider(bool b);
    void saveParticlesAsObj(const QString& filename);
    void sphKernelChangedCallback(const NESphLiquidSolver::NESPHKernel& val);
    const double& particleSize() const override;

    void particleSizeChangedCallback(const double& val) override;
    void interRadiusChangedCallback(const double& val);

    uint saveFrameData(QString path, const int frameNumber) override;
    uint loadFrameData(QString path, const int frameNumber) override;

signals:
    void deletingParticles(NESphRigid* rigid);

protected:
    virtual void generateBoundaryParticles();
    void generateVolumeParticles();
    void generateMeshParticles();
    virtual uint createMesh() override = 0;

    //! Transforms the particles according to WorldTransform, should be called after particles are generated
    virtual void transformParticles() override;

    //! Reserves memory for the given number of particles according to the current fluid solver
    void reserveParticlesMemory(uint total);

    //! Returns ture if particle sampling of the mesh is required.
    bool requireParticleSampling();

    // Has to be called after the mesh has been created
    void meshCreated();

    //! Initializes NEVectorMetaData and m_LiquidParticleNeighbors
    void postProcessParticleGeneration();

    //! Generates a boundary particle at the given position and if needed with the given normal according to the current fluid solver
    void generateSphParticle(vec3f Position, vec3f normal);
    void generateSphParticle(vec3f Position);

    uint SetThermalSetting(const ThermalSettingType& val) override;
    uint SetTemperature(const double& val) override;

    virtual bool isExportParticleDataVisible();

private:

    NE_DECL_PRIVATE(NESphRigid)
    bool areParticleDataToBeExported();
};

Q_DECLARE_INTERFACE(NESphRigid, "NESphRigid/0.1")
