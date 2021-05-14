#pragma once

#include "Physics/Spatial/Physical/NERigid.h"
#include "Physics/Spatial/Physical/NEParticle.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidParticle.h"
#include "Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidPciSphParticle.h"
#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEParticleRigid))

//!  Interface for implementations of Particle Rigids. Rigids are instantiated and handled by NERigidManager.
/*!
  This class provides an interface for the framework to communicate with any instance of a particle rigid.
*/
class NECEXP_PHYSICS NEParticleRigid : public NERigid
{
    Q_OBJECT
    NE_CLASS("Particle Sampled Rigid Visualization")
public:
    enum NEParticleColoring	{ None = 0,	/*VelocityBased,*/	PressureBased, TemperatureBased, ConcentrationChemicalSpeciesBased, TotalForceBased, PressureForceBased, FrictionForceBased, AdhesionForceBased/*, DensityBased, MemoryLocationBased, ParticleTypeBased, RigidNeighborhoodBased*/ };
    enum ChemSpeciesChoiceType { _1_, _2_, _3_, _4_, _5_, _6_, _7_ }; // Number of choices is 7, because maxNumOfChemSpecies = 7
    enum NEColoringOptions {Fixed = 0, Ramp = 1};

    NE_PROP_RW(bool, ShowParticles, "Show Particles", "Should the boundary particles be visible", true,)
    NE_PROP_RW(bool, ShowColorFromParticles, "Show Color from Particles", "Show Rigid base on Particle Color", true,)
    NE_PROP_RW(bool, DrawSurfaceNormals, "Draw Surface Normals", "Should the surface normals be visible?", true,)
    NE_PROP_RW(bool, TwoSidedLighting, "Two-Side Lighting", "Gives the dark sides of the objects a diffuse color in case of a single positional light source", true,)
    NE_PROP_GROUP_START(ColoringGroup, "Particle Coloring", "Particle colorization related options", true)
    NE_PROP_RW(NEParticleColoring, Coloring, "Coloring Scheme", "The scheme to use when coloring particles.", true,)
    NE_PROP_RW(ChemSpeciesChoiceType, ChemSpeciesChoice, "Chemical Species Choice", "Choice of chemical species for particle coloring", true, )
    NE_PROP_RW(bool, AutomaticRange, "Automatic Color Range", "Should the color range be automatic based on the minimum and maximum values?", true,)
    NE_PROP_RW(NEColoringOptions, ColoringOptions, "Coloring Options", "Coloring Options", true,)
    NE_SUB_PROP_START(SetColoringOptions, ColoringOptions)
//    NE_PROP_GROUP_START(Range, "Ranges and Colors", "Ranges and Colors", true)
    NE_PROP_RW(double, MinColoringRange, "Minimum Value", "The lower limit where the color interpolation starts.", true,)
    NE_PROP_RW(double, MaxColoringRange, "Maximum Value", "The upper limit where the color interpolation starts.", true,)
    NE_PROP_RW(double, ColoringExponent, "Coloring Exponent", "The exponent used to scale the input data before drawing particles.", true,)
    NE_PROP_RW(QColor, MinRangeColor, "Color at Minimum", "Color of the minimum value", true,)
    NE_PROP_RW(QColor, MaxRangeColor, "Color at Maximum", "Color of the maximum value", true,)
    NE_PROP_RW(NEColorRamp, ColorRamp, "Color Ramp", "Color Ramp", true,)
    NE_PROP_GROUP_END(SetColoringOptions)
    NE_PROP_GROUP_END(ColoringGroup)

    Q_ENUMS(NEParticleColoring ChemSpeciesChoiceType NEColoringOptions)

public:

    virtual void setRigidParticleType(NEParticleFluidSolver::NERigidParticleType val);

    NEParticleRigid(NENodeFactory* factory);
    virtual ~NEParticleRigid();

    virtual uint init(uint initMode = NE::kInitOnCreate);

    virtual void colorizeParticles();

    const NEVectorMetaData* particlesVector() const;
    //! Returns the memory consumption of the particles
    virtual uint memoryConsumption();

    QVector<QPair<qreal, QColor>> getColorList() const;

    AABB aabb() const;
public slots:
    uint SetShowStatistics(const bool& val) override;
    uint SetShowTimings(const bool& val) override;
    virtual const double& particleSize() const = 0;
    virtual void particleSizeChangedCallback(const double& val) = 0;
    uint saveAsObj(const QString& filename);
    void resetProperties();

signals:
    void initObject();

protected:
    NE_DERIVED(NESpatial, Protected)
    // This is an intelligent reference to the underlying particles
    NEVectorMetaData m_RigidParticles;
    NEParticleFluidSolver::NERigidParticleType m_RigidParticleType;

    //! Transforms the particles according to WorldTransform, should be called after particles are generated
    virtual void transformParticles() = 0;
    virtual uint createMesh() = 0;

private:
    friend class NESphFoam;
    NE_DECL_PRIVATE(NEParticleRigid)
    void colorizeNone();
   };

Q_DECLARE_INTERFACE(NEParticleRigid, "NEParticleRigid/0.1")
