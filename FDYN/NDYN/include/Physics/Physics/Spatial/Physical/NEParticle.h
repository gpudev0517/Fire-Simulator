#pragma once

#include "Physics/Spatial/NEPhysical.h"

//struct Neighbor;
class NEParticle;

struct Neighbor
{
public:
    Neighbor(){m_pNeighbor = NULL;}
    Neighbor(const NEParticle* pParticle, float d):m_pNeighbor(pParticle), m_Dist(d){}

    const NEParticle* neighbor() {return m_pNeighbor;}
    float distance() const {return m_Dist;}
    float& distance() {return m_Dist;}
    void setDistance(float val) {m_Dist = val;}
    vec3f wGrad() const {return m_WGrad;}
    vec3f& wGrad() {return m_WGrad;}
    void setWGrad(vec3f val) {m_WGrad = val;}

private:
    //! Pointer to the neighbor. This one might belong to another fluid/solver.
    const NEParticle* m_pNeighbor;
    //! Distance to neighbor.
    float m_Dist;
    vec3f m_WGrad;
};


class NECEXP_PHYSICS NEParticle
{
public:
    enum NEParticleType
    {
        kNormalParticle = 1,
        kInflowGhost = 2,
        kExtInflowGhost = 3,
        kFillInflowGhost = 4,
        kPressureOutflowGhost = 5,
        kPeriodicGhost = 6,
        kVelocityOutflowGhost = 7,
        kFreeSurfaceGhost = 8
    };

    NEParticle() :  m_Id(0), m_CellId(NE::kUnregisteredCellId), m_pSolver(NULL), m_Color(200,200,255,255), m_newParticle(true), m_IsConstrained(false), m_Type(kNormalParticle), m_GhostDistToTravel(0.0), m_GhostExtDistToTravel(0.0), m_OutflowGhostNormal(0.0,0.0,0.0) {}

    inline const vec3f& position() const { return m_Position; }
    inline vec3f& position() { return m_Position; }
    inline void setPosition(const vec3f& position) { m_Position = position; }
    inline void setPositionX(const double x) { m_Position.setX(x); }
    inline void setPositionY(const double y) { m_Position.setY(y); }
    inline void setPositionZ(const double z) { m_Position.setZ(z); }

    inline uint id() const { return m_Id; }
    inline uint& id() { return m_Id; }
    inline void setId(const uint id) { m_Id = id; }

    inline uint cellId() const { return m_CellId; }
    inline uint& cellId() { return m_CellId; }
    inline void setCellId(const uint id) {m_CellId = id; }

    inline const Color32& color() const { return m_Color; }
    inline Color32& color() { return m_Color; }
    inline void setColor(Color32 color) { m_Color = color; }

    inline NEPhysical* solver() const { return m_pSolver; }
    inline NEPhysical* solver() { return m_pSolver; }
    inline void setSolver(NEPhysical* solver) { m_pSolver = solver; }

    inline bool active() const { return false; }
    inline virtual void setActive(bool b) {}

    //adds a fluid neighbor for this particle
    inline void addNeighbor(Neighbor& n) { m_pSolver->addNeighbor(n); }

    std::vector<Neighbor>* neighborVector(uint index) const { return m_pSolver->neighborVector(index); }
    std::vector<Neighbor>* rigidNeighborVector(uint index) { return  m_pSolver->rigidNeighborVector(index); }

    inline const NEVectorMetaData* vectorMetaData() const { return m_pSolver->particlesVector(); }
    inline uint index() { return vectorMetaData()->dataIndex(this); }
    //uint getIndex() const { return index(); }

    inline void setNew(bool newParticle) { m_newParticle = newParticle; }
    inline bool isNew() {return m_newParticle; }

    inline bool isConstrained() { return m_IsConstrained; }
    inline size_t constrainerId() { return m_ConstrainerId; }
    inline void constrain(size_t id) { m_IsConstrained = true; m_ConstrainerId = id; }
    inline void unconstrain() { m_IsConstrained = false; }

    inline uint type() const { return m_Type; }
    inline uint& type() { return m_Type; }
    inline void setType(uint type) { m_Type = type; }

    inline std::string daemonName() const { return m_DaemonName; }
    inline std::string& daemonName() { return m_DaemonName; }
    inline void setDaemonName(const std::string& daemonName) { m_DaemonName = daemonName; }

    inline uint daemonSourceId() const { return m_DaemonSourceId; }
    inline uint& daemonSourceId() { return m_DaemonSourceId; }
    inline void setDaemonSourceId(uint daemonSourceId) { m_DaemonSourceId= daemonSourceId; }

    inline float ghostDistToTravel() const { return m_GhostDistToTravel; }
    inline float& ghostDistToTravel() { return m_GhostDistToTravel; }
    void setGhostDistToTravel(float dist) { m_GhostDistToTravel = dist; }

    inline float ghostExtDistToTravel() const { return m_GhostExtDistToTravel; }
    inline float& ghostExtDistToTravel() { return m_GhostExtDistToTravel; }
    void setGhostExtDistToTravel(float dist) { m_GhostExtDistToTravel = dist; }

    inline vec3f outflowGhostNormal() const { return m_OutflowGhostNormal; }
    inline vec3f& outflowGhostNormal() { return m_OutflowGhostNormal; }
    void setOutflowGhostNormal(const vec3f& normal) { m_OutflowGhostNormal = normal; }

    inline uint periodicOriginId() const { return m_PeriodicOriginId; }
    inline uint& periodicOriginId() { return m_PeriodicOriginId; }
    void setPeriodicOriginId(const uint& id) { m_PeriodicOriginId = id; }

protected:

    uint m_Id;
    uint m_CellId;
    NEPhysical* m_pSolver;
    bool m_newParticle;
    bool m_IsConstrained;
    size_t m_ConstrainerId;
    uint m_Type;
    std::string m_DaemonName; // Source, killer or dual boundary; empty string if not a fluid normal particle
    uint m_DaemonSourceId; // source id of the inflow coupler; used only when the daemon is an inflow coupler
    float m_GhostDistToTravel; // Distance left to travel as ghost particle
    float m_GhostExtDistToTravel; // Distance left to travel as ghost external particle
    vec3f m_OutflowGhostNormal; // Normal of the outflow boundary for outflow ghost particles
    uint m_PeriodicOriginId; // id of the origin particle for periodic boundary particles
    //Modifying the relative order of color and position will break the renderer
    //NEW: keep these variables at the end of the member variables list
    Color32 m_Color;
    vec3f m_Position;
};



// The particle struct for visualization
struct NEVisParticle
{
    Color32 col;
    vec3f pos;
    //matrix33f aniso;
};
