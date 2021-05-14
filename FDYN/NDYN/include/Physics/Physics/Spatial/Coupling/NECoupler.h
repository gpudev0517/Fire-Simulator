#pragma once

#include "Utility/FilePath.h"
#include "Physics/Spatial/NEDualBoundary.h"

#define RIGIDTOPO 1

//!  General coupling class.

NE_FWD_DECL_3(class, NE_PRIVATE(NECoupler), NEParticleFluidSolver, NESphLiquidSolver)
#if RIGIDTOPO
NE_FWD_DECL_4(class, NE_PRIVATE(NECoupler), NEParticleFluidSolver, NERigidTopography, NESphLiquidSolver)
#endif

//! Public interface of NECoupler
#define INTERFACE_NECoupler_Public(_Terminal_)\
    virtual void resetProperties() _Terminal_\
    virtual void reset() _Terminal_\
    virtual bool connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order = 0) _Terminal_\
    virtual bool disconnectFrom(NENode* target,uint type, uint sIndex, uint tIndex) _Terminal_\

//! Pure virtual public interface of NECoupler
#define INTERFACE_NECoupler_Public_Pure(_Terminal_)\
    virtual uint saveFrameData(QString path,const int frameNumber) _Terminal_\
    virtual uint loadFrameData(QString path,const int frameNumber) _Terminal_

//! Public slot interface of NECoupler
#define INTERFACE_NECoupler_Public_Slots(_Terminal_)\
    virtual uint SetBoundaryShape(const BoundaryShapeType& val) _Terminal_\
    virtual uint SetGaugeInput(const FilePath& val) _Terminal_\
    virtual uint SetExtremPointInput(const FilePath& val) _Terminal_\
    virtual uint SetConnexionInput(const FilePath& val) _Terminal_\
    virtual uint SetPosition(const vec3f& val) _Terminal_\
    virtual uint SetOrientation(const eulerf& val) _Terminal_\
    virtual uint SetScale(const vec3f& val) _Terminal_

//! Protected interface of NECoupler
#define INTERFACE_NECoupler_Protected(_Terminal_)\
    virtual void particleSizeChangedCallback() _Terminal_\
    virtual void initializeGUIActions() _Terminal_\
    virtual void reloadGaugeInput() _Terminal_\
    virtual void clearGaugeInput() _Terminal_\
    virtual void reloadExtremPointInput() _Terminal_\
    virtual void clearExtremPointInput() _Terminal_\
    virtual void reloadConnexionInput() _Terminal_\
    virtual void clearConnexionInput() _Terminal_

class NECEXP_PHYSICS NECoupler : public NEDualBoundary
{
    Q_OBJECT
    NE_CLASS("General Coupler")

public:
    enum BoundaryShapeType
    {
        Linear ,
        Rectangular = 1,
        Arbitrary = 2
    };
    enum class InputGeoCoordSystemType
    {
        LocalEstNorthUp = 0,
        LongitudeLatitudeHeightWgs84 = 1,
        LongitudeLatitudeHeightNad83 = 2
    };
    enum class OutputGeoCoordSystemType
    {
        LocalEstNorthUp = 0
    };

    NE_PROP_RW(BoundaryShapeType, BoundaryShape, "Boundary Shape", "Shape of boundary", true, virtual)
    NE_PROP_RW(FilePath, GaugeInput, "Gauge File Path","CSV file containing gauge x- and z-position", true, virtual)
    NE_PROP_RW(FilePath, ExtremPointInput, "Extremity Point File Path", "CSV file containing extremity point x- and z-position", true, virtual)
    NE_PROP_RW(FilePath, ConnexionInput, "Connexion File Path", "CSV file containing id pairs of connected gauge(s)/extremity point", isBoundaryArbitrary, virtual)
    NE_PROP_RW(InputGeoCoordSystemType, InputGeoCoordSystem, "Input Geo. Coord. System", "Geographic coordinate system of the positions of the gauges and extremity points", true, )
    NE_PROP_RW(OutputGeoCoordSystemType, OutputGeoCoordSystem, "Output Geo. Coord. System", "Geographic coordinate system to convert the positions of the gauges and extremity points to", true, )

    Q_ENUMS(BoundaryShapeType InputGeoCoordSystemType OutputGeoCoordSystemType)

public:
    NECoupler(NENodeFactory* factory);
    virtual ~NECoupler();
    virtual uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;
    bool connectTo(NENode* target, uint type, uint sIndex, uint tIndex, int order = 0);
    bool disconnectFrom(NENode* target,uint type, uint sIndex, uint tIndex);

    NE_BASE_PURE(NECoupler, Public_Pure)
    bool addSource(NENode* node, NEConn& conn);
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);
    // Returns the gauge array in the converted coordinate system
    const std::vector<vec2f>& vecConvertedGauges() const;
    // Return the extremity point array in the converted coordinate system
    const std::vector<vec2f>& vecConvertedExtremPoints() const;
    // Returns the connected component array of the graph
    // Returns the connected component array of the graph
    const std::vector<std::list<size_t>>& vecListConnectedComponents() const;
    // Returns the connected fluid solver
    NESphLiquidSolver* fluidSolver() const;
    // Returns the resolution-dependant epsilon
    float epsilon() const;
    // Returns the rest distance to scale ratio
    float partSizeOverScaleX() const;
    float partSizeOverScaleZ() const;
    // For each fluid particle and solid particle, decides whether to "kill" it or not;
    // if to kill, either marks it for deletion or transforms it into an outflow ghost (fluid only)
    // Called at each time step
    uint checkAndMarkToKill() override;
    void generateParticles() override;

public slots:
    NE_DERIVED(NESpatial, Public_Slots)
    // Synchronize the input geographic coordinate system
    void syncInputGeoCoordSystem();
    // Synchronize the output geographic coordinate system
    void syncOutputGeoCoordSystem();
    // Synchronize the geographic reference location
    void syncGeoRefLocation();

signals:
    void geoRefLocationChanged();

protected:
    virtual void particleSizeChangedCallback();
    void initializeGUIActions() override;
    virtual void reloadGaugeInput();
    virtual void clearGaugeInput();
    virtual void reloadExtremPointInput();
    virtual void clearExtremPointInput();
    virtual void reloadConnexionInput();
    virtual void clearConnexionInput();
#if RIGIDTOPO

    // Returns the connected rigid topography node
    // for the geographic coordinate conversion
    NERigidTopography* connectedRigidTopoGeoConversion() const;
    // Returns the connected rigid topography node for the triangle mesh
    NERigidTopography* connectedRigidTopoTriMesh() const;
#endif
    // Fits the elevation of the coupling boundary
    // to the connected topography triangle mesh
    virtual void fitBoundaryToTopoTriMesh();


private:
    // Returns true if the boundary shape is arbitrary, false otherwise
    bool isBoundaryArbitrary();
    NE_DECL_PRIVATE(NECoupler)
};

Q_DECLARE_INTERFACE(NECoupler, "NECoupler")

