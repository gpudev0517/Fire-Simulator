#pragma once

#include "Physics/Spatial/NEPhysical.h"

class btRigidBody;
class btCollisionShape;
class NEArticulatedRigidPrivate;

class NEArticulatedRigid : public NEPhysical
{
	Q_OBJECT
    Q_CLASSINFO("ClassName", "General Articulated Rigid Object Settings")

	Q_PROPERTY(double rigidFriction READ rigidFriction WRITE setRigidFriction)
    Q_CLASSINFO("name-rigidFriction", "Rigid Friction")
    Q_CLASSINFO("help-rigidFriction", "Changes the friction of the rigid object.")

	Q_PROPERTY(double rigidToFluidFriction READ rigidToFluidFriction WRITE setRigidToFluidFriction)
    Q_CLASSINFO("name-rigidToFluidFriction", "Rigid to Fluid Friction")
    Q_CLASSINFO("help-rigidToFluidFriction", "Changes the friction from the rigid object to fluid.")

	Q_PROPERTY(double fluidToRigidFriction READ fluidToRigidFriction WRITE setFluidToRigidFriction)
    Q_CLASSINFO("name-fluidToRigidFriction", "Fluid to Rigid Friction")
    Q_CLASSINFO("help-fluidToRigidFriction", "Changes the friction from fluid to the rigid object.")

	Q_PROPERTY(NERigidType rigidType READ rigidType WRITE setRigidType)
    Q_CLASSINFO("name-rigidType", "Rigid Type")
    Q_CLASSINFO("help-rigidType", "Changes the type of the rigid object as dynamic, static or kinematic.")

	Q_PROPERTY(bool twoWayCoupling READ twoWayCoupling WRITE setTwoWayCoupling)
    Q_CLASSINFO("name-twoWayCoupling", "Two Way Coupling")
    Q_CLASSINFO("help-twoWayCoupling", "Sets the interaction between fluid and rigid object")

	Q_PROPERTY(double mass READ mass WRITE setMass)
    Q_CLASSINFO("name-mass", "Mass")
    Q_CLASSINFO("help-mass", "Changes the mass of the rigid object.")

    Q_PROPERTY(vec3f linearVelocity READ linearVelocity WRITE setLinearVelocity)
    Q_CLASSINFO("name-linearVelocity", "Linear Velocity")
    Q_CLASSINFO("help-linearVelocity", "Changes the linear velocity of the rigid object.")

	Q_PROPERTY(vec3f angularVelocity READ angularVelocity WRITE setAngularVelocity)
    Q_CLASSINFO("name-angularVelocity", "Angular Velocity")
    Q_CLASSINFO("help-angularVelocity", "Changes the angular velocity of the rigid object.")

	Q_ENUMS(NERigidType)

public:

	enum NERigidType
	{
		dynamicRigid,
		staticRigid,
		kinematicRigid
	};


    NEFEXP_CORE NEArticulatedRigid(NENodeFactory* factory);
	virtual ~NEArticulatedRigid();
	
	virtual uint init(uint initMode = NE::kInitOnCreate);

	virtual void updateForces();

	virtual bool updateAABB();

	//! This syncs the bullet rigid with the rigid in our framework
	virtual void syncRigid();
	//! Returns true if the rigid is already added to bullet, otherwise false
	bool belongsToBullet() const {return m_InBullet;}
	//! Adds and removes the rigid to and from bullet.
	uint setBulletBelonging(bool val);
public slots:

	NERigidType rigidType() const { return m_RigidType; }
	void setRigidType(NERigidType val);

	bool twoWayCoupling() const { return m_TwoWayCoupling; }
	void setTwoWayCoupling(bool val) { m_TwoWayCoupling = val; }

	//! Returns the mass of the whole rigid
	float mass() const { return m_Mass; }
	virtual void setMass(float val);

	//! Returns the linear velocity of the rigid from the bullet representation
	vec3f linearVelocity() const;
	void setLinearVelocity(const vec3f& val);

	//! Returns the angular velocity of the rigid from the bullet representation
	vec3f angularVelocity() const;
	void setAngularVelocity(const vec3f& val);

	float rigidFriction() const { return m_RigidFriction; }
	void setRigidFriction(float val);

	//! Returns the friction amount applied from the rigid to the fluid
	float rigidToFluidFriction() const { return m_RigidToFluidFriction; }
	void setRigidToFluidFriction(float val);
	
	//! Returns the friction amount applied from the fluid to the rigid
	float fluidToRigidFriction() const { return m_FluidToRigidFriction; }
	void setFluidToRigidFriction(float val) { m_FluidToRigidFriction = val; }

	//! Saves the loading of data for the current simulation/animation frame.
	virtual uint saveFrameData(QString path, const int frameNumber);
	//! Calls the loading of data for the current simulation/animation frame.
	virtual uint loadFrameData(QString path, const int frameNumber);

	//! Sets a behavior to the node
	virtual uint SetBehavior(NENodeState val);

private:
	NERigidType m_RigidType;
	//bool m_Dynamics;
	bool m_TwoWayCoupling;
	float m_Mass;
	float m_RigidFriction;
	float m_RigidToFluidFriction;
	float m_FluidToRigidFriction;
	//temporarily put trace and shadow options here for mental ray export
	bool m_Trace;
	bool m_Shadow;
	//! True if the rigid is added to bullet and false if it is set back.
	bool m_InBullet;
protected:

	//! Creates and (re-)inserts the rigid into the bullet solver
	void createBulletRigid(btCollisionShape* shape);
	btRigidBody* m_pBulletRigidBody;

private:
    NEArticulatedRigidPrivate* P;
};

Q_DECLARE_INTERFACE(NEArticulatedRigid, "NEArticulatedRigid")
