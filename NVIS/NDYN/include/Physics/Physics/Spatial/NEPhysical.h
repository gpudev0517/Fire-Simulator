#pragma once

#include "Spatial/NESpatial.h"

NE_FWD_DECL_3(class, NEParticle, NEForceField, NE_PRIVATE(NEPhysical))
struct Neighbor;

class NECEXP_PHYSICS NEPhysical : public NESpatial
{
    Q_OBJECT
    NE_CLASS("General Simulation Options")
    public:
        // Type of the physical entity
        enum PhysicalType
        {
            kNone,
            kSolid,
            kDeformable,
            kFluid,
            kMSS
        };

    NE_PROP_RW(double, Timestep, "Time-Step", "The time-step required by the solver", timeStepVisible,)
    NE_PROP_RW(bool, ShowStatistics, "Display Statistics", "Compute and display simulation statistics.", true, virtual)
    NE_PROP_RW(bool, ShowTimings, "Display Timings", "Compute and display simulation timings.", true, virtual)

public:

    NEPhysical(NENodeFactory *factory, PhysicalType val);
    virtual ~NEPhysical();
    //! Resets the values of the physical
    virtual void reset();

    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Updates the AABB of the physical, return false if the update fails
    virtual bool updateAABB();
    //! This function is called once when the simulation starts
    virtual void prepareForSimulation();

    //! Returns the list of connected force fields
    std::forward_list<NEForceField*> connectedForceFields() const;
    //! Connects to a force field
    uint connectToForceField(NEForceField* forceField);
    //! Disconnects from a forceField
    uint disconnectFromForceField(NEForceField* forceField);

    //! Returns the total external acceleration contributed by the connected force fields
    vec3f totalExternalAcceleration(const vec3f* pos = 0, const vec3f* vel = 0);

    virtual void addNeighbor(Neighbor& n);
    virtual std::vector<Neighbor>* neighborVector(uint index);
    virtual std::vector<Neighbor>* rigidNeighborVector(uint index);
    virtual const NEVectorMetaData* particlesVector() const;

    //! Computes statistics of the solver and prints it to the OSD
    virtual void computeStatistics();

    //!	Returns the frame number of the frame in which the physical got into a sink
    int inSinkAtFrame() const;
    //!  Returns true if the spatial is in the sink for the provided frame.
    bool inSink(int frameNr) const;
    //! Sets the frame number to the frame in which the physical got into a sink. Called by collider
    void setInSinkAtFrame(int frameNr);

    PhysicalType physicalType();

signals:
    void requireSimulationUpdate(bool);
    void gravityChanged(vec3f);

public slots:
    virtual uint SetBehavior(const NENodeState& val) override;
    virtual void resetProperties() override;

protected:
    virtual void extForceChangedCallback();

    virtual bool timeStepVisible() {return false;}

	//! Frame number that the physical got in a sink/killer field.
	int m_InSinkAtFrame;

private:
	NE_DECL_PRIVATE(NEPhysical)

	// NENode interface
public:
        virtual uint eval() override;

	// NESpatial interface
protected:
        virtual void render() override;
        virtual void renderPicked() override;
        virtual void renderPickable() override;
        };

Q_DECLARE_INTERFACE(NEPhysical, "NEPhysical")
