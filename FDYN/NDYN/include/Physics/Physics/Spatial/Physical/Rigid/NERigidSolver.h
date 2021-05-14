#pragma once

#include "NENode.h"

NE_FWD_DECL_3(class, NE_PRIVATE(NERigidSolver), NERigid, NELink)

//!  Base class solver for all rigid bodies

class NECEXP_PHYSICS NERigidSolver: public NENode
{
    Q_OBJECT
    NE_CLASS("Rigid-Body Solver")

public:
    NE_PROP_RW(double, InternalTimeStep, "Internal Time-Step", "Constant time-step internally used by the rigid-body solver", true, )

public:
    NERigidSolver(NENodeFactory* factory);
    virtual ~NERigidSolver();

public slots:

    virtual uint deInit();
    virtual uint reset();
    void resetProperties() override;

    //! Prepares the rigids for simulation by the solver
    virtual uint prepareRigidsForSimulation();

    //! Synchronizes the rigid solver and the solids
    virtual void syncRigids();

    //! Slot which is called during rigid body update for each rigid body
    virtual void syncRigid( NERigid *rigid);

    //! Add the rigid body into the simulation domain for the rigid body solver
    virtual void addRigid( const NERigid* rigid );

    //! Removes the rigid body from the solver engine.
    virtual void removeRigid( const NERigid* rigid );

    //! Resets the rigid body for the solver to apply to its initual state
    virtual void resetRigid( const NERigid* rigid );

    //! Sets the coefficient of friction for that rigid body
    virtual void setFriction( const NERigid* rigid, double val ) ;

    //! Sets the linear velocity of that rigid body
    virtual void setLinearVelocity( const NERigid* rigid, const vec3f& val );

    //! Sets the angular velocity of that rigid body
    virtual void setAngularVelocity( const NERigid* rigid, const vec3f& val);

    //! Sends a message to the solver to set the behavior of the rigid
    virtual void setBehavior(const NERigid* rigid, const NENode::NENodeState& val);

    //! Select that rigid body to perform operations on
    virtual void selectRigid( const NERigid *rigid );

    //! Sends a message to the solver to clear all forces for the rigid
    virtual void clearForces(const NERigid *rigid);

    //! Adds the fluid force to that selected
    virtual void addFluidForce( const NERigid *, vec3f pd, vec3f force );

    //! Adds the force to that selected
    virtual void addForce( const NERigid *, vec3f pd, vec3f force );

    //! Adds the torque to that selected
    virtual void addTorque( const NERigid *, vec3f torque );

    //! Gets the force for that selected rigid
    virtual vec3f force();

    //! Gets the Torque for that selected rigid
    virtual vec3f torque();

    //! Adds/removes the specific rigid body to the solver.
    virtual void setBelonging( const NERigid *rigid, bool belong );

    //! Add a constraint/link to the  rigid body chain
    virtual void addLink( const NELink* link );
    //! Remove the constraint/link/chain to the rigid body chain.
    virtual void removeLink( const NELink* link );

    //! Create the collision shape for the geometry for the rigid
    virtual void createCollisionShape( const NERigid *rigid );
    //! Remove the collision shape for the geometry
    virtual void removeCollisionShape( const NERigid *rigid );

    //! Get the angular accel for the rigid from the specific solver
    virtual vec3f angAcc( const NERigid* rigid ) const;
    //! Get the linear accel for the rigid from the specific solver
    virtual vec3f linAcc( const NERigid* rigid ) const;

protected:
    virtual bool addSource(NENode* node, NEConn& conn) override;
    virtual void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

private:
    NE_DECL_PRIVATE(NERigidSolver)
};
