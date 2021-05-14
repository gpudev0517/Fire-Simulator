#pragma once

#include "Physics/Spatial/Coupling/NECoupler.h"
#include "Physics/Spatial/NEMultiDirectionalDaemon.h"
#include "Base/NENodeFactory.h"

//!  Coupling class for inflow.

NE_FWD_DECL_1(class, NE_PRIVATE(NEInflowCoupler))

class NECEXP_PHYSICS NEInflowCoupler : public NECoupler, public NEMultiDirectionalDaemon
{
    Q_OBJECT
    NE_CLASS("Inflow Coupler")

public:
    enum FlowDirectionType
    {
        InteriorNormal = 0,
        ExteriorNormal = 1,
        InteriorVelocity = 2,
        ExteriorVelocity = 3
    };
    enum InflowDataType
    {
        ShallowWater = 0
    };

    NE_PROP_RW(FlowDirectionType, FlowDirection, "Flow Direction", "Direction of flow", true, )
    NE_PROP_RW(bool, SubdivideSources, "Subdivide Sources", "Subdivides the sources", true, )
    NE_PROP_RW(double, SubdivisionLength, "Subdivision Length", "Length of the source subdivisions", SubdivideSources, )
    NE_PROP_RW(InflowDataType, InflowData, "Inflow Data Type", "Type of the inflow data", true, )
    NE_PROP_RW(FilePath, SwDataInput, "Shallow-Water Data File Path", "CSV file containing time, x- and z-velocity, and height", true, )
    NE_PROP_GROUP_START(Advanced, "Advanced Properties", "Advanced properties", true)
    NE_PROP_RW(bool, EnforceFlowRate, "Enforce Flow Rate", "Enforce the right flow rate; drawback is a deviation from the right flow speed", true, )
    NE_PROP_RW(bool, CompressionAllowed, "Compression Allowed", "Allows slightly compressed particle configuration, to enhance mass conservation", true, )
    NE_PROP_RW(bool, KillInGhostDomain, "Kill In Ghost Domain", "Allows killing of regular fluid particles that enter the ghost domain", true, )
    NE_PROP_RW(bool, GenerateOutflowGhosts, "Generate Outflow Ghosts", "Allows the generation of outflow ghost particles when killing", true, )
    NE_PROP_GROUP_END(Advanced)

    Q_ENUMS(FlowDirectionType InflowDataType)

public:
    NEInflowCoupler(NENodeFactory* factory);
    ~NEInflowCoupler();
    uint init(uint initMode = NE::kInitOnCreate) override;
    NE_DERIVED(NECoupler, Public)
    NE_DERIVED(NECoupler, Public_Pure)
    void generateParticles() override;
    float halfPartSizeOverScale() const;
    std::vector<vec3f> vecSourcePositions() const;
    uint checkAndMarkToKill() override;

    vec3f velocity(uint sourceId, const vec3f& pos) const override;
    double normalAdjSpeed(uint sourceId, const vec3f& pos) const override;
    vec3f normalAdjVelocity(uint sourceId, const vec3f& pos) const override;

public slots:
    NE_DERIVED(NECoupler, Public_Slots)

private:
    NE_DERIVED(NECoupler, Protected)
    void interRadiusChangedCallback();
    void fitBoundaryToTopoTriMesh() override;
    NE_DECL_PRIVATE(NEInflowCoupler)
};

class NEInflowCouplerFactory : public NENodeFactory
{
public:
    NEInflowCouplerFactory(NEManager* m);
    ~NEInflowCouplerFactory();
    NE_DERIVED(NENodeFactory, Public)
};
