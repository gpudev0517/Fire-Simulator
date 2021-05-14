#pragma once

#include <array>

#include "globals_Physics.h"
#include "Utility/NEColorRamp.h"
#include "Physics/Spatial/Physical/NEFluidSolver.h"
#include "Physics/Spatial/Physical/NEParticle.h"

NE_FWD_DECL_4(class, NE_PRIVATE(NEParticleFluidSolver), NEFluidParticle, NEPhysicalParticle, NECollider)
NE_FWD_DECL_3(class, NEParticleSource, NEParticleKiller, NEDualBoundary)

/*!
    The base class for all types of fluids which compute the physics based on particles.
*/

class NECEXP_PHYSICS NEParticleFluidSolver : public NEFluidSolver
{
    Q_OBJECT
    NE_CLASS("General Particle-Fluid Settings")

public:

    enum NEParticleDataType	{IdPosition = 1, Color = 2, Velocity = 4, Density = 8, Pressure = 16, Temperature = 32, Concns = 64, Normal = 128, Neighbors = 256, Type = 512, GhostType = 1024, GhostDistToTravel = 2048, DaemonName = 4096, DaemonSourceId = 8192};
    enum NEColoringOptions {Fixed = 0, Ramp = 1};
    enum NEParticleColoring	{None = 0, VelocityBased, PressureBased, DensityBased, TemperatureBased, ConcentrationChemicalSpeciesBased, ParticleTypeBased, ParticleIdBased /*,MemoryLocationBased, RigidNeighborhoodBased, DivergenceBased, PressureChange, 
        KappaBased, CurvatureBased, BlendingBased,FrozenBased, VelocityFieldBased, VorticityBased, AccelerationBased*/};
    enum ChemSpeciesChoiceType { _1_, _2_, _3_, _4_, _5_, _6_, _7_ }; // Number of choices is 7, because maxNumOfChemSpecies = 7
    enum NEParticleSectionType {kSectionSurfaceParticles = 2, kSectionIsolatedParticles = 3, kSectionInternalParticles = 4, kSectionNeighborOfSurfaceParticles = 5};
    enum NEParticleExport {Isolated, All};
    enum NEColliderTask {Ignore_Phase = 1, No_Query_But_Insert = 3, Query_Only_Own = 2, Query_Only_Other = 4, Query_All = 6};
    enum NEExportMaterial {UniqueMaterial, TypeBasedMaterial, VolumeMaterial};
    enum NERigidParticleType {NoFluidSolver = 0, SphParticle, PciSphParticle};
    enum NEPartVisType {ParticleColor = 0, Streamlines = 1, Vectors = 2};
    enum NEParticleRenderingType {Points = 0, Disks = 1, Spheres = 2, Blobbies = 3};

    NE_PROP_RW(double, ParticleSize, "Particle Size", "Size of the fluid particles; corresponds also to the average distance between adjacent particles", true, virtual)
    NE_PROP_R(double, ParticleVolume, "Particle Volume", "Volume of the fluid particles", true)
    NE_PROP_R(double, ParticleMass, "Particle Mass", "Mass of the fluid particles", true)
    NE_PROP_RW(double, InterRadiusToPartSizeRatio, "Interaction-Radius to Particle-Size Ratio", "Ratio between the interaction radius and the particle size", true, virtual)
    NE_PROP_R(double, InterRadius, "Interaction Radius", "Interaction radius of the particles", true)
    NE_PROP_R(double, AvgNumNeighborParticles, "Avg. Num. of Neighbor Particles", "Average number of neighboring particles within the interaction radius", true)
    NE_PROP_GROUP_START(ParticleTracking, "Particle Tracking [Debug]", "Particle Tracking settings [Debug]", true)
    NE_PROP_RW(bool, EnableParticleTracking, "Enable Particle Tracking", "Shows calculation details for a particular particle", true, )
    NE_PROP_RW(int, TrackedPartNb, "Tracked Particle Number", "Particle chosen for particle tracking - This particle will appear in black", true, )
    NE_PROP_GROUP_END(ParticleTracking)
    NE_PROP_RW(int, ClearNeighborsFrameInterval, "Neighborhood Clearing Interval", "How often (in terms of frames) should the particles neighborhoods vectors should be wiped.", isClearNeighborsFrameIntervalVisible,)
    NE_PROP_GROUP_START(ParticleVisualization, "Particle Visualization Settings", "Setting about particle visualization", true)
    NE_PROP_GROUP_START(ParticleColoring, "Particle Coloring", "Particle colorization related options", true)
    NE_PROP_RW(double, MinColoringRange, "Minimum Value", "The lower limit where the color interpolation starts.", true,)
    NE_PROP_RW(double, MaxColoringRange, "Maximum Value", "The upper limit where the color interpolation starts.", true,)
    NE_PROP_RW(bool, AutomaticRange, "Automatic Color Range", "Should the color range be automatic based on the minimum and maximum values?", true,)
    NE_PROP_RW(NEParticleColoring, Coloring, "Coloring Scheme", "The scheme to use when coloring particles.", true,)
    NE_PROP_RW(ChemSpeciesChoiceType, ChemSpeciesChoice, "Chemical Species Choice", "Choice of chemical species for particle coloring", true, )
    NE_PROP_RW(NEColoringOptions, ColoringOptions, "Coloring Options", "Coloring Options", true,)
    NE_SUB_PROP_START(SetColoringOptions, ColoringOptions)
//    NE_PROP_GROUP_START(Range, "Ranges and Colors", "Ranges and Colors", true)
    NE_PROP_RW(QColor, MinRangeColor, "Color at Minimum", "Color of the minimum value", true,)
    NE_PROP_RW(QColor, MaxRangeColor, "Color at Maximum", "Color of the maximum value", true,)
//    NE_PROP_GROUP_END(Range)
    NE_PROP_RW(double, ColoringExponent, "Coloring Exponent", "The exponent used to scale the input data before drawing particles.", true,)
    NE_PROP_RW(bool, InterpolateHues, "Interpolate Hues", "Interpolation between Max and Min Colors based on Hue", true,)
    NE_PROP_RW(NEColorRamp, ColorRamp, "Color Ramp", "Color Ramp", true,)
    NE_SUB_PROP_END(SetColoringOptions)
    NE_PROP_GROUP_END(ParticleColoring)
    NE_PROP_RW(NEPartVisType, VisType, "Visualization Type", "The settings for Visualizing the particles the visualize window", true,)
    NE_PROP_GROUP_START(Debugging, "Debugging", "Debugging Settings", true)
    NE_PROP_RW(bool, DrawParticleNormals, "Draw Particle Normals", "Draws per-particle normals", true,)
    NE_PROP_RW(bool, DrawFluidNeighbors, "Draw Fluid Neighbors", "Draws lines between neighboring fluid particles.", true,)
    NE_PROP_RW(bool, DrawRigidNeighbors, "DrawRigidNeighbors", "Draws lines between neighboring fluid and boundary particles.", true,)
    NE_PROP_RW(bool, DrawVelocity, "Draw Velocity Vectors", "Renders particles as a velocity field", true,)
    NE_PROP_GROUP_END(Debugging)
    NE_PROP_RW(double, RenderRadiusScaling, "Render Radius Scaling", "For changing the rendered radius of particles.", true,)
    // Hide Ghosts property is hidden, as not yet functional
    NE_PROP_RW(bool, HideGhosts, "Hide Ghosts", "Hide the ghost particles", false, )
    NE_PROP_RW(QColor, InflowGhostColor, "Inflow Ghost Color", "Color of the inflow ghost particles", true,)
    NE_PROP_RW(QColor, InflowGhostExtColor, "Ext Inflow Ghost Color", "Color of the exterior inflow ghost particles", true,)
    NE_PROP_RW(QColor, InflowGhostFillColor, "Fill Inflow Ghost Color", "Color of the fill inflow ghost particles", true,)
    NE_PROP_RW(QColor, OutflowGhostColor, "Outflow Ghost Color", "Color of the outflow ghost particles", true,)
    NE_PROP_RW(QColor, PeriodicGhostColor, "Periodic Ghost Color", "Color of the periodic ghost particles", true,)
    NE_PROP_RW(QColor, FreeSurfaceGhostColor, "Free Surface Ghost Color", "Color of the free surface ghost particles", true,)
    NE_PROP_GROUP_END(ParticleVisualization)
    NE_PROP_GROUP_START(Export, "Particle Export", "Particle Export Settings", true)
    NE_PROP_RW(NEParticleExport, ExportMethod, "Export Method", "Which type of particles should be exported", true,)
    NE_PROP_RW(int, MinNeighbors, "Neighbor Count", "For exporting particles which have the number of neighbors larger than this threshold.", true,)
    NE_PROP_RW(NEExportMaterial, ExportMaterial, "Export Material", "What type of material should be assigned to the particles when exporting.", true,)
    NE_PROP_RW(bool, BinaryExport, "Binary Export", "Should the particles be exported to a binary file.", true,)
    NE_PROP_GROUP_END(Export)
    NE_PROP_RW(NEColliderTask, QueryNeighbors, "Neighborhood Query", "For changing how the collision detection system handles this solver.", true,)

    Q_FLAGS(ParticleDataTypes)
    Q_ENUMS(NEParticleDataType NEColoringOptions NEParticleColoring ChemSpeciesChoiceType NEParticleSectionType NEParticleExport NEExportMaterial NEColliderTask NEPartVisType NEParticleRenderingType)

    friend class NERenderer;
    friend class NECollider;

public:
    virtual NERigidParticleType rigidParticleType();
    NEParticleFluidSolver(NENodeFactory* factory);
    virtual ~NEParticleFluidSolver();
    virtual uint init(uint initMode = NE::kInitOnCreate);
    //! Updates the physics of the particle fluid. Is called by the coresponding manager.
    virtual void update() = 0;    
    virtual uint eval() override;

    uint connectToFluid(NEFluid* fluid) override;
    uint disconnectFromFluid(NEFluid* fluid) override;

    virtual void propertyTweak(QtPropertyBase* p) override;
    //! Integrates the position and velocity of the fluid.
    virtual void integrate(bool computeCollision=false);
    const NEVectorMetaData* particlesVector() const;
    NEVectorMetaData& particlesMetaVector();
    virtual double particleRadius() const;
    virtual double particleRenderScale() const;

    //! For adding particles to the fluid, for example by a source that is connected to the fluid.
    virtual void addParticles(std::vector<NEParticle>& particles, std::vector<vec3f>& vels);
    virtual void addParticles(std::vector<NEParticle>& particles, vec3f& vel);
    virtual void addParticles(std::vector<NEParticle>& particles, std::vector<vec3f>& vels, double temperature);
    virtual void addParticles(std::vector<NEParticle>& particles, vec3f& vel, double temperature);
    virtual void addParticles(std::vector<NEParticle>& particles, std::vector<vec3f>& vels, std::array<double, maxNumOfChemSpecies>& concns);
    virtual void addParticles(std::vector<NEParticle>& particles, vec3f& vel, std::array<double, maxNumOfChemSpecies>& concns);
    virtual void addParticles(std::vector<NEParticle>& particles, std::vector<vec3f>& vels, double temperature, std::array<double, maxNumOfChemSpecies>& concns);
    virtual void addParticles(std::vector<NEParticle>& particles, vec3f& vel, double temperature, std::array<double, maxNumOfChemSpecies>& concns);

    virtual void addPeriodicGhosts(std::vector<NEPhysicalParticle>& particles);

    //! Delets all marked particles.
    virtual void deleteParticles()=0;
    //! deletes the particle
    virtual void deleteParticle(NEParticle* particle)= 0;
    //! A fast reordering method that only keeps the values that are required, called by collider.
    virtual void reorderParticles(const std::vector<IdPointerPair>& idPairs);
    virtual void colorizeParticles();
    virtual bool updateAABB();

    //NEIGHBORHOOD related
    //! Returns the adress of the particle neighborhood std::vector. This should be used by the collider for writing out neighbors.
    virtual std::vector< std::vector<Neighbor>>& liquidParticleNeighbors()
    {
        return m_LiquidParticleNeighbors;
    }
    //! Returns the adress of the rigid particle neighborhood std::vector. This should be used by the collider for writing out neighbors.
    virtual std::vector< std::vector<Neighbor>>& rigidParticleNeighbors()
    {
        return m_RigidParticleNeighbors;
    }
    virtual void addNeighbor(Neighbor& n, uint index);
    virtual void removeNeighbor(NEParticle* particleToRemove, uint index);
    virtual std::vector<Neighbor>* neighborVector(uint index)
    {
        return &m_LiquidParticleNeighbors[index];
    }

    virtual std::vector<Neighbor>* rigidNeighborVector(uint index)
    {
        return  &m_RigidParticleNeighbors[index];
    }

    bool particlesInCollider();
    void setParticlesInCollider(bool b);
    inline int nrOfSurfaceParticles();
    //! Extracts the surface particles
    virtual void extractSurfaceParticles();
    //! Saves the loading of data for this simulation/animation frame.
    virtual uint saveFrameData(QString path, const int frameNumber);
    //! Calls the loading of data for this simulation/animation frame.
    virtual uint loadFrameData(QString path, const int frameNumber);
    uint openFrameFileV1(QString path, const int frameNumber, QFile &fIn, QDataStream &sIn);
    uint openFrameFileV2(QString path, const int frameNumber, std::istream &sIn);
    //! Returns the volume radius of the particles
    virtual float volumeRadius() const;
    //! Returns true if explicit boundary handling is required used when integrating particles.
    virtual bool explicitBoundaryHandling() const;
    //! Returns the number of fluid particles
    virtual size_t numFluidParticles() const;

    //! Returns the connected collider
    NECollider* connectedCollider() const;
    //! Returns the list of connected sources
    std::forward_list<NEParticleSource*> connectedSources() const;
    //! Returns the list of connected killers
    std::forward_list<NEParticleKiller*> connectedKillers() const;
    //! Returns the list of connected dual boundaries
    std::forward_list<NEDualBoundary*> connectedDualBoundaries() const;

    //! Connects to a collider
    uint connectToCollider(NECollider* collider);
    //! Connects to a source
    uint connectToSource(NEParticleSource* source);
    //! Connects to a killer
    uint connectToKiller(NEParticleKiller* killer);
    //! Connects to a dual boundary
    uint connectToDualBoundary(NEDualBoundary* dualBoundary);
    //! Disconnects from a collider
    uint disconnectFromCollider(NECollider* collider);
    //! Disconnects from a source
    uint disconnectFromSource(NEParticleSource* source);
    //! Disconnects from a killer
    uint disconnectFromKiller(NEParticleKiller* killer);
    //! Disconnects from a dual boundary
    uint disconnectFromDualBoundary(NEDualBoundary* dualBoundary);

    //! Returns the inverse of the particle size
    const double& invParticleSize();
    //! Returns the inverse of the particle volume
    const double& invParticleVolume();
    //! Returns the inverse of the particle mass
    const double& invParticleMass();
    //! Returns the inverse of the interaction radius
    const double& invInterRadius();

    //SAVING AND LOADING RELATED
    struct NEBinaryDataSection;

public slots:
    NE_DERIVED(NESpatial, Public_Slots)
    void resetProperties();

    void restDensityChangedCallback() override;

signals:
    void initObject();
    //! Signal to inform that the fluid particles fParticles are getting deleted
    void deletingParticles(std::vector<NEFluidParticle*>& fParticles);

public:
    //! Cleans all cached data of this entity and clears the current data like physical particles
    virtual uint cleanData(QString path);
    //! Cleans all cached data of this entity and clears the current data like physical particles
    virtual uint cropData(QString path, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);

    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //!Opens the edit caching dialog.
    virtual void editCachingMode();
    //! For generic editing of caching modes.
    virtual int primitiveGroupIndexOfEnum();
    virtual int primitiveDataTypesIndexOfEnum();
    void ClearNeighbors();
    QVector<QPair<qreal, QColor>> getColorList() const;
    bool queryNeighbors() const;
    void setQueryNeighbors(const NEColliderTask& val);

    void resizeParticles(uint nItems);

protected:
    //! Compute the particle volume and particle mass
    virtual uint computeParticleVolumeMass();
    //! Compute the interaction radius from the rest distance and the ratio between interaction radius and rest distance
    virtual uint computeInterRadius();

    virtual void initializeGUIActions() override;
    virtual void boundaryCollision(NEPhysicalParticle* particle, vec3f& pos, bool correctVel);
    void prepareParticleColoring(float minVal, float maxVal);
    //! Gets a float value and returns a color value
    Color32 getColorValueF(float value);
    //! Adds the particles of newParticles to the target std::vector NEVector.
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, double temperature, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, double temperature, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, std::array<double, maxNumOfChemSpecies>& concns, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, std::array<double, maxNumOfChemSpecies>& concns, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, double temperature, std::array<double, maxNumOfChemSpecies>& concns, NEVector<T>& target);
    template<class T> void addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, double temperature, std::array<double, maxNumOfChemSpecies>& concns, NEVector<T>& target);

    template<class T> void addPeriodicGhosts(std::vector<NEPhysicalParticle>& newParticles, NEVector<T>& target);

    template<class T> void deleteParticles(NEVector<T>& target);
    template<class T> void deleteParticle(NEParticle* particle, NEVector<T>& target);
    virtual void resizeParticlesVector(uint nItems) = 0;
    virtual void reserveSpaceForParticles(uint nItems) = 0;

    //! Returns the file name for the current frame number
    QString fileName(int frameNumber);
    uint readFrameFileHeaderV1(QDataStream &sIn, quint32& nrParticles, std::vector<NEBinaryDataSection>& dataSections );
    uint readFrameFileHeaderV2(std::istream &sIn, quint32& nrParticles, std::vector<NEBinaryDataSection>& dataSections );
    uint loadAllSectionsV1(QString path, QString fileName, QDataStream &sIn, std::vector<NEBinaryDataSection>& dataSections );
    virtual uint loadAllSectionsV2(QString path, QString fileName, std::istream &sIn, std::vector<NEBinaryDataSection>& dataSections );
    virtual void loadSectionDataV2(std::istream& sIn, int start, int end, uint contentFlags, uint requiredContentFlags);
    virtual void loadSectionDataV1(QDataStream& sIn, int start, int end, uint contentFlags, uint requiredContentFlags);
    int sectionOffset(const uint value, const int nrOfItems);
    virtual uint writeSectionsCStream(std::ostream& out,  const QString path, int secOffset);
    //! Returns the path were the solver stores its content
    virtual QString savePath();

    virtual bool isClearNeighborsFrameIntervalVisible();

    QMap<uint,uint> m_SectionTypeMap;
    //! True if the particles are inserted into the collider grid.
    bool m_bParticlesInCollider;
    //! Other instances might work only on the surface particles. In order to always recompute the number it should be set when loading or extracting surface particles.
    int m_NrOfSurfaceParticles;
    //! Stores the neighbors for each particle. The std::vector has the same order as the m_Particles std::vector.
    std::vector<std::vector<Neighbor>> m_LiquidParticleNeighbors;
    //! Stores the rigid neighbors for each particle. The std::vector has the same order as the m_Particles std::vector.
    std::vector<std::vector<Neighbor>> m_RigidParticleNeighbors;
    // This is an intelligent reference to the underlying particles
    NEVectorMetaData m_FluidParticles;
    //! This property sets if adaptive support radius is used. Can be accessed with updateSimulation()
    bool m_bAdaptiveRadius;
    //! Counts the non-decreasing number of particles of this solver over the whole simulation time, i.e. if a particle is deleted, the number is not decreased
    mutable uint m_OldParticleCount;
    //! Keeps the square of the maximum velocity among all particles
    double m_MaxVel2;
    //! Keeps the square of the maximum acceleration among all particles
    double m_MaxAccel2;
    //! Keeps the minimum, maximum and average densities among all particles
    double m_MaxDensity;
    double m_MinDensity;
    double m_AveDensity;
    //! Keeps the minimum and maximum pressures among all particles
    double m_MaxPressure;
    double m_MinPressure;
    //! Keeps the minimum and maximum temperatures among all particles
    double m_MaxTemperature;
    double m_MinTemperature;
    //! Keeps the minimum and maximum concentrations with respect to chemical species among all particles
    std::array<double, maxNumOfChemSpecies> m_MaxConcnsChemSpecies;
    std::array<double, maxNumOfChemSpecies> m_MinConcnsChemSpecies;
    //! Keeps the kinetic energy of the particles
    double m_EnergyKinetic;
    //! Keeps the potential energy with respect to the coordinate system origin
    double m_EnergyPotential;
    double m_MinStress;
    double m_MaxStress;
    double m_MinVMStress;
    double m_MaxVMStress;

private:
    NE_DECL_PRIVATE(NEParticleFluidSolver)
};


struct NEParticleFluidSolver::NEBinaryDataSection
{
    qint16 Id;
    qint32 contentFlag;
    qint32 nParticles;
    qint64 Offset;
};


Q_DECLARE_INTERFACE(NEParticleFluidSolver, "NEParticleFluidSolver")


#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEParticleFluidSolver.inl"
