#pragma once
#include "Utility/FilePath.h"
#include "Physics/Spatial/NEPhysical.h"
#include "Resource/Mesh/NEIndexedTriMesh.h"


NE_FWD_DECL_2(class, FilePath, NE_PRIVATE(NERigid))


class NECEXP_PHYSICS NERigid : public NEPhysical
{
    Q_OBJECT
    NE_CLASS("General Rigid Body Options")

public:
    enum NERigidType { Dynamic, Static, Kinematic };
    enum ThermalSettingType { Adiabatic = 0, Isothermal = 1 };
    enum WallBoundarySettingType { NoSlip = 0, FreeSlip = 1 };
    enum NEVisType { MeshColor = 0, Elevation = 1, Force  = 2 };
    enum NECollisionShapeType { ConvexHull, ConvexHullReduced, TriangleMesh };
    enum NEMeshRenderingType { Polygons = 0, SubdivisionSurface = 1 };

    NE_PROP_RW(NERigidType, RigidType, "Rigid Body Type", "Type of the rigid body object", isRigidTypeVisible, )
    NE_PROP_RW(double, Density, "Density", "Density of the rigid body", isRigidTypeDynamic, )
    NE_PROP_R(double, Mass, "Mass", "Mass of the rigid body object", isRigidTypeDynamic)
    NE_PROP_RW(ThermalSettingType, ThermalSetting, "Thermal Setting", "Type of thermal setting", true, virtual)
    NE_PROP_RW(double, Temperature, "Temperature", "Temperature of the fluid at the generation time", true, virtual)
    NE_PROP_RW(WallBoundarySettingType, WallBoundarySetting, "Wall Boundary Setting", "Type of wall boundary setting", true, )
    NE_PROP_RW(double, SolidFluidSlipMultiCoeff, "Solid-Fluid Slip Multi. Coeff.", "Multiplicative coefficient used when imposing slip conditions", true, )
    NE_PROP_RW(double, LinVelSolidFluidSlipMultiCoeff, "Linear Multi. Coeff. to Slip Velocity", "Multiplicative coefficient used when imposing slip conditions. (y=a*v+b) this is a", true, )
    NE_PROP_RW(double, OffsetSolidFluidSlipMultiCoeff, "Offset Multi. Coeff. to Slip Velocity", "Multiplicative coefficient used when imposing slip conditions. (y=a*v+b) this is b", true, )
    NE_PROP_RW(double, PressureForceScaling, "Multi. Coeff. to Pressure force", "Multiplicative coefficient used for pressure force applied on rigids", true, )
    NE_PROP_RW(double, RigidFriction, "Rigid-Rigid Friction Coeff.", "Controls the friction force between the solid and the other solids.", true, )
    NE_PROP_RW(double, Viscosity, "Rigid-Fluid Friction Coeff.", "Controls the friction force between the solid and the fluids.", true, )
    NE_PROP_RW(double, Adhesion, "Rigid-Fluid Adhesion Coeff.", "Controls the adhesion force between the solid and the fluids.", true, )
    NE_PROP_GROUP_START(DynamicProperties, "Dynamic properties","Properties of the dynamics of the rigid", true)
    NE_PROP_RW(bool, UseCustomCenterOfGravity, "Modify gravity center ?",  "Allows to define a custom gravity center that would not be the center of the geometry of the rigid", (not isRigidTypeNonStatic), )
    NE_PROP_3D_RW(vec3f, float, CustomCenterOfGravity, "Center of gravity position",  "Position of the center of gravity in the rigid body located according to its coordinate system.", showCustomCenterOfGravityParam, )
    NE_PROP_33D_RW(matrix33d, double, CustomInertiaTensor, "Custom inertia tensor", "Inertia tensor that has to be provided when the center of gravity is custom.", showCustomCenterOfGravityParam, )
    NE_PROP_33D_R(matrix33d, double, InertiaTensor, "Inertia tensor", "Inertia tensor taken into account in dynamic computations", true )
    NE_PROP_3D_RW(vec3f, float, LinearVelocity, "Linear Velocity",  "Linear velocity of the rigid body", isRigidTypeNonStatic, )
    NE_PROP_3D_RW(vec3f, float, AngularVelocity, "Angular Velocity",  "Angular velocity of the rigid body", isRigidTypeNonStatic, )
    NE_PROP_3D_R(vec3f, float, LinearAcceleration, "Linear Acceleration",  "Linear acceleration of the rigid body", isRigidTypeNonStatic)
    NE_PROP_3D_R(vec3f, float, AngularAcceleration, "Angular Acceleration",  "Angular acceleration of the rigid body", isRigidTypeNonStatic)
    NE_PROP_3D_RW(vec3f, float, InputForce, "Input Force", "Force applied on center of gravity added to the rigid body in dynamic update", isRigidTypeDynamic, )
    NE_PROP_3D_RW(vec3f, float, InputTorque, "Input Torque", "Torque added to the rigid body in dynamic update", isRigidTypeDynamic, )
    NE_PROP_3D_R(vec3f, float, TotalForce, "Total Force", "Total force applied to the rigid object", true)
    NE_PROP_3D_R(vec3f, float, FluidForce, "Fluid->Solid Force", "Force applied by the fluid to the rigid object", true)
    NE_PROP_3D_R(vec3f, float, SolidForce, "Solid->Solid Force", "Force applied by the solids to the rigid object", true)
    NE_PROP_3D_R(vec3f, float, TotalTorque, "Total Torque", "Total torque applied to the rigid object", true)
    NE_PROP_3D_R(vec3f, float, FluidTorque, "Fluid->Solid Torque", "Torque applied by the fluid to the rigid object", true)
    NE_PROP_3D_R(vec3f, float, SolidTorque, "Solid->Solid Torque", "Torque applied by the solids to the rigid object", true)
    NE_PROP_GROUP_END(DynamicProperties)
    NE_PROP_GROUP_START(ExportData, "Export Data","Export data to a csv file", isExportDataVisible)
    NE_PROP_RW(bool, ExportPosition, "Export Position","Export the position", true, )
    NE_PROP_RW(bool, ExportLinVel, "Export Linear Velocity","Export the linear velocity", true, )
    NE_PROP_RW(bool, ExportAngVel, "Export Angular Velocity","Export the angular velocity", true, )
    NE_PROP_RW(bool, ExportLinAcc, "Export Linear Acceleration","Export the linear acceleration", true, )
    NE_PROP_RW(bool, ExportAngAcc, "Export Angular Acceleration","Export the angular acceleration", true, )
    NE_PROP_RW(bool, ExportForce, "Export Force","Export the force", true, )
    NE_PROP_RW(bool, ExportTorque, "Export Torque","Export the torque", true, )
    NE_PROP_RW(bool, ExportForceDecomposition, "Export Force Decomposition","Export the decomposition of the force (fluid pressure & fluid friction)", true, )
    NE_PROP_RW(bool, ExportTorqueDecomposition, "Export Torque Decomposition","Export the decomposition of the torque (fluid pressure & fluid friction)", true, )
    NE_PROP_RW(FilePath, ExportPath, "Export File Path", "Set the path to export the data", areDataToBeExported, )
    NE_PROP_GROUP_END(ExportData)
    NE_PROP_GROUP_START(Advanced, "Advanced Properties", "Advanced properties", true)
    NE_PROP_RW(bool, FlipMesh, "Invert Normals", "Inverts the mesh normals and vertex order", true, )
    NE_PROP_RW(NEVisType, VisType, "Visualization Type", "The settings for Visualizing the rigid in the visualize window", true, )
    NE_PROP_RW(NECollisionShapeType, CollisionShapeType, "Collision Shape Type", "Sets the collision shape type as convex hull, reduced convex hull or impact mesh.", true, )
    NE_PROP_RW(bool, ComputePerVertexVelocity, "Per-Vertex Velocities", "Computes per-vertex velocities", true, )
    NE_PROP_GROUP_END(Advanced)

    Q_ENUMS(NERigidType ThermalSettingType WallBoundarySettingType NEVisType NECollisionShapeType NEMeshRenderingType)

public:
    NERigid( NENodeFactory* factory );
    virtual ~NERigid();

    uint init( uint initMode = NE::kInitOnCreate ) override;
    void reset() override;
    void resetProperties() override;

    virtual bool updateAABB();
    virtual void syncRigid();
    virtual void updateForces();

    void setLinearAcceleration(const vec3f& val);
    void setAngularAcceleration(const vec3f& val);
    void setInvInertiaDiagLocal(const vec3d& val);
    void setLocalInertia(const vec3d& val);

    const matrix33d& invInertiaTensor() const;
    vec3d getInvInertiaDiagLocal();
    vec3d getLocalInertia();
    matrix33d getInertiaTensor();

    void updateTotalForce();
    void updateTotalTorque();
    bool belongsToSystem() const;

    void resetFluidForceAndTorque();
    void accumulateFluidForceAndTorque(const vec3f& position,
                                       const vec3f& pressureForce,
                                       const vec3f& frictionForce,
                                       const vec3f& adhesionForce);

    void setIsChangedForce(bool flag ){ isChangedForced = flag; }
    bool getIsChangedForce() { return isChangedForced; }

signals:

    void triMeshChanged();
    void updateDrawingBuffers();

    void createCollisionShape( const NERigid * );
    void removeCollisionShape( const NERigid * );
    void selectRigid( const NERigid * );
    void addRigid( const NERigid * );
    void removeRigid( const NERigid * );
    void resetRigid( const NERigid * );
    void addFluidForce( const NERigid *, vec3f, vec3f );
    void addForce( const NERigid *, vec3f, vec3f );
    void addTorque( const NERigid *, vec3f );
    void clearForces( const NERigid * );
    void syncRigidBody(  NERigid * );

public slots:
    //! Returns the volume of the rigid
    virtual double volume();



    //! Returns the acutal velocity of the rigid at the given world position. It is computed combining linear and angular velocities.
    vec3f velocity(const vec3f& pos) const;

    //! Saves the loading of data for the current simulation/animation frame.
    virtual uint saveFrameData(QString path, const int frameNumber);

    //! Calls the loading of data for the current simulation/animation frame.
    virtual uint loadFrameData(QString path, const int frameNumber);

    //! Sets a behavior to the node
    virtual uint SetBehavior(const NENodeState& val) override;

    //! These functions are used to notify the collider whenever the rigid is moved
    bool hasMoved();
    void setMoved(bool b);

    virtual const vec3f& centerOfMass() const;

    uint SetScale(const vec3f& val) override;
    uint SetCenterMethod(const NESpatial::NECenterCalculation& val) override;
    uint SetLocalizedTransforms(const bool& val) override;
    uint computeInertiaTensor(const matrix33d& val);

protected:

    uint localizeTransforms() override;
    uint relocalizeTransforms() override;
    uint freezeTransforms() override;
    uint computeCenter() override;


    void computeMass();
    virtual bool isRigidTypeVisible();
    virtual bool isExportDataVisible();
    bool isChangedForced;

private:
    NE_DECL_PRIVATE(NERigid)
    bool isRigidTypeDynamic() const;
    bool isRigidTypeNonStatic() const;
    bool isCustomCenterOfGravity() const;
    bool showCustomCenterOfGravityParam() const;
    bool areDataToBeExported();
};

Q_DECLARE_INTERFACE(NERigid, "NERigid")
