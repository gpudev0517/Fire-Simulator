#pragma once

#include "Physics/Spatial/NESource.h"
#include "Physics/Spatial/Physical/NEParticle.h"
class NEParticleFluidSolver;

//Debug flag
#define DEBUG_EMITTER 0

//!  Emitter class for particle fluids.
/*!
        Must be binded to a particle fluid solver.
*/
class NECEXP_PHYSICS NEParticleSource : public NESource
{
    Q_OBJECT
    Q_CLASSINFO("ClassName", "General Particle Source")

    Q_PROPERTY(bool finiteVolume READ finiteVolume WRITE setFiniteVolume DESIGNABLE isFiniteVolumeVisible)
    Q_CLASSINFO("name-finiteVolume", "Limited Volume")
    Q_CLASSINFO("help-finiteVolume", "Controling whether the source produces a finite fluid volume.")
    Q_PROPERTY(SubPropertyStart VolumeParams READ subStart)
    Q_PROPERTY(double volume READ volume WRITE setVolume DESIGNABLE finiteVolume)
    Q_CLASSINFO("name-volume", "Volume")
    Q_CLASSINFO("help-volume", "The maximum amount of fluid volume that will be seeded from the source.")
    Q_PROPERTY(SubPropertyEnd VolumeParams_End READ subEnd)

    Q_PROPERTY(double startFrame READ startFrame WRITE setStartFrame)
    Q_CLASSINFO("name-startFrame", "Start Time")
    Q_CLASSINFO("help-startFrame", "The time where the source starts seeding particles.")

    Q_PROPERTY(double stopFrame READ stopFrame WRITE setStopFrame)
    Q_CLASSINFO("name-stopFrame", "Stop Time")
    Q_CLASSINFO("help-stopFrame", "The time where the source stops seeding particles.")

    Q_PROPERTY(double velocityMag READ velocityMagnitude WRITE setVelocityMagnitude DESIGNABLE velocityMagnitudeReadable)
    Q_CLASSINFO("name-velocityMag", "Velocity Magnitude")
    Q_CLASSINFO("help-velocityMag", "The velocity of the seeded fluid layer.")

    Q_PROPERTY(vec3f velocity READ velocity DESIGNABLE isVelocityVisible)
    Q_CLASSINFO("name-velocity", "Initial Velocity")
    Q_CLASSINFO("help-velocity", "The initial velocity of the particles (read-only)")

    Q_PROPERTY(bool spherePacking READ spherePacking WRITE setSpherePacking DESIGNABLE isSpherePackingVisible)
    Q_CLASSINFO("name-spherePacking", "Sphere Packing")
    Q_CLASSINFO("help-spherePacking", "If true the emitted particles are sampled using optimal, hexa-lattice arrangement. Otherwise it is sampled using the shape.")

signals:
    void volumeChanged();

public:
    NEParticleSource(NENodeFactory* factory);
    //NEParticleSource(NEParticleFluidSolver* fluid);
    virtual ~NEParticleSource();

    virtual void reset();

    uint init(uint initMode = NE::kInitOnCreate) override;
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    uint connectToFluidSolver(NEParticleFluidSolver* fluid);
    uint disconnectFromFluidSolver(NEParticleFluidSolver* fluid);

    virtual void update();

    virtual void generateParticles();

    const NEParticleFluidSolver* solver() const;

public slots:
    double velocityMagnitude() const;

    bool finiteVolume() const;
    void setFiniteVolume(bool val);

    const double& volume() const;
    void setVolume(const double volume);

    double startFrame() const;
    void setStartFrame(const double start);

    double stopFrame() const;
    void setStopFrame(const double stop);

    const vec3f& velocity();
    void setVelocity(const vec3f& vel);
    virtual void setVelocity();

    void setVelocityMagnitude(const double mag);

    bool spherePacking() const;
    virtual void setSpherePacking(bool val);

//    bool connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order = 0);
//    virtual bool disconnectFrom(NENode* target, uint type, uint sIndex, uint tIndex);

    //! Saves the data for this simulation frame.
    virtual uint saveFrameData(QString path, const int frameNum);

    //! Loads the data for this frame.
    virtual uint loadFrameData(QString path, const int frameNum);

    //! Overwrite set orientation of NENode
    virtual uint SetOrientation(const eulerf& rotation);
    //! Overwrite set position of NENode
    virtual uint SetPosition(const vec3f& Position);

    virtual void particleSizeChangedCallback();
    virtual void interRadiusChangedCallback();

    //! Deletes the solid backplane
    virtual void deleteSolidPlane();

    virtual void setGravityMagnitude(const vec3f& gravity);

    virtual void connectedFluidsChangedCallback();
    virtual void connectedGrainsChangedCallback();
    virtual void connectedChemSpeciesChangedCallback();

protected:
    //!
    virtual bool Active();
    //!
    void setDelta(vec3f offset);

    virtual void transformParticles();
    //!Returns true if the inflow is permanent, otherwise false, e.g. a blocksource is not a permanent inflow
    virtual bool permanentInflow();

    virtual bool isFiniteVolumeVisible() {return true;}
    virtual bool isVelocityVisible() {return true;}
    virtual bool isSpherePackingVisible() {return true;}
    virtual bool velocityMagnitudeReadable() {return true;}
    virtual bool timestepVisible() {return false;}
    virtual bool showStatisticsVisible() {return false;}
    virtual bool showTimingsVisible() {return false;}
    virtual bool pivotVisible() override {return false;}
    virtual bool renderModeVisible() override {return false;}
    virtual bool renderPriorityVisible() override {return false;}
    virtual bool externalRenderVisible() override {return false;}

protected:
    //! Each source can emit particles for exactly one solver.
    NEParticleFluidSolver* m_pFluidSolver;
    //!
    std::vector<NEParticle> m_tmpParticles;
    //! The volume (of fluid) that is hold by the source.
    double m_Volume;
    //! The nr of particles that will be generated.
    //int m_NrOfGeneratedParticles;
    //! The nr of particles that are generated per step.
    int m_NrGeneratedPerStep;
    //! Time the source starts to emit particles.
    double m_StartFrame;
    //! Time the source stops to emit particles.
    double m_StopFrame;

    //!
    double m_VelocityMagnitude;
    //! Initial velocity of the emitted particles.
    vec3f m_Velocity;

    //! The estimated velocity of the last set of emitted particles.
    vec3f m_VelocityEstimated;
    //! The estimated distance of the last set of emitted particles to the source.
    vec3f m_DeltaX;

    //! Outputs debug info if Debug FLAG is true
    bool m_Verbose;
    //!Finite volume, source emits only the volume that is given by volume
    bool m_FiniteVolume;
    //! If true the emitted particles are sampled using optimal, hexa-lattice arrangement.
    bool m_SpherePacking;
    //! Layer count for variational generation
    uint m_LayerCount;

    // NEObject interface
protected:
    virtual void initializeGUIActions() override;


    // NENode interface
public:
    virtual uint eval() override;
    vec3f DeltaX() const;
    void setDeltaX(const vec3f &DeltaX);
};

Q_DECLARE_INTERFACE(NEParticleSource, "NEParticleSource")
