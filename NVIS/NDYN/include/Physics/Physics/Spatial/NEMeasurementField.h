#pragma once

#include "Physics/Spatial/NEDataCollector.h"
#include "Base/NENodeFactory.h"
#include "Utility/FilePath.h"
#include "Utility/NEColorRamp.h"
//#include <boost/graph/adjacency_list.hpp>
#include <boost/multi_array.hpp>
//#include <boost/graph/depth_first_search.hpp>
//#include <boost/graph/lookup_edge.hpp>


class NEPhysicalParticle;
class NEParticleFluidSolver;
class NESphLiquidParticle;
class NESphLiquidSolver;
class NESphRigid;
class NEMeasurementFieldPrivate;

//!  Data measurement class for particle fluids.
/*!
    Must be binded to a particle fluid solver.
*/
class NECEXP_PHYSICS NEMeasurementField : public NEDataCollector
{
    Q_OBJECT
public:
    enum class MeasureType
    {
        Velocity,
        AvgPressure,
        MaxPressure,
        Density,
        FlowRate,
        AvgHeight,
        MaxHeight,
        UAvgVelocity,
        VAvgVelocity,
        WAvgVelocity,
        NParticles,
        FlowCoupler,
        MinVelocity,
        MaxVelocity,
        MinFrontPos,
        MaxFrontPos,
        AvgTemperature,
        MinTemperature,
        MaxTemperature,
        AvgConcnsChemSpecies,
        MinConcnsChemSpecies,
        MaxConcnsChemSpecies,
        FluidToSolidForce,
        All
    };
    NE_PROP_RW(MeasureType, OutputData, "Measurement Output", "The type of data to output to scalar or vector", true,)

    Q_PROPERTY(vec3f minPos READ minPos)
    Q_CLASSINFO("name-minPos", "Minimum Position")
    Q_CLASSINFO("help-minPos", "Minimum position of the measurement field in the world.")

    Q_PROPERTY(vec3f maxPos READ maxPos)
    Q_CLASSINFO("name-maxPos", "Maximum Position")
    Q_CLASSINFO("help-maxPos", "Maximum position of the measurement field in the world.")

    Q_PROPERTY(vec3f avgVel READ avgVel)
    Q_CLASSINFO("name-avgVel", "Average Velocity")
    Q_CLASSINFO("help-avgVel", "Average velocity of the particles in the field.")

    Q_PROPERTY(double avgPressure READ avgPressure)
    Q_CLASSINFO("name-avgPressure", "Average Pressure")
    Q_CLASSINFO("help-avgPressure", "Average pressure of the particles in the field.")

    Q_PROPERTY(double maxPressure READ maxPressure)
    Q_CLASSINFO("name-maxPressure", "Maximum Pressure")
    Q_CLASSINFO("help-maxPressure", "Maximum pressure of the particles in the field.")

    Q_PROPERTY(double avgDensity READ avgDensity)
    Q_CLASSINFO("name-avgDensity", "Average Density")
    Q_CLASSINFO("help-avgDensity", "Average density of the particles in the field.")

    Q_PROPERTY(double avgFluidHeight READ avgFluidHeight RESET resetAvgFluidHeight)
    Q_CLASSINFO("name-avgFluidHeight", "Accumulation Fluid Height")
    Q_CLASSINFO("help-avgFluidHeight", "Accumulation Height of the fluid according hydrostatic pressure.")

    Q_PROPERTY(double maxFluidHeight READ maxFluidHeight RESET resetMaxFluidHeight)
    Q_CLASSINFO("name-maxFluidHeight", "Max Fluid Height")
    Q_CLASSINFO("help-maxFluidHeight", "Max Height of the fluid according to sea level parameter.")

    Q_PROPERTY(int nParticles READ nParticles)
    Q_CLASSINFO("name-nParticles", "Particles Inside")
    Q_CLASSINFO("help-nParticles", "Number of particles inside of the field.")

    Q_PROPERTY(double totalVolume READ totalVolume)
    Q_CLASSINFO("name-totalVolume", "Fluid Volume")
    Q_CLASSINFO("help-totalVolume", "Fluid volume (in m^3) inside of the field.")

    Q_PROPERTY(bool computeFlowRate READ computeFlowRate WRITE setComputeFlowRate)
    Q_CLASSINFO("name-computeFlowRate", "Compute Flow Rate")
    Q_CLASSINFO("help-computeFlowRate", "Enable to compute the amount of fluid (m^3/s) passing through a plane at the center.")

    Q_PROPERTY(SubPropertyStart FlowRateParams READ subStart)

    Q_PROPERTY(vec3f flowPlaneNormal READ flowPlaneNormal WRITE setFlowPlaneNormal DESIGNABLE computeFlowRate)
    Q_CLASSINFO("name-flowPlaneNormal", "Plane Normal")
    Q_CLASSINFO("help-flowPlaneNormal", "The normal of the plane where the flow rate is computed.")

    Q_PROPERTY(double flowRate READ flowRate DESIGNABLE computeFlowRate)
    Q_CLASSINFO("name-flowRate", "Flow Rate")
    Q_CLASSINFO("help-flowRate", "The amount of fluid (m^3/s) passing through the plane.")


    Q_PROPERTY(SubPropertyEnd FlowRateParams_End READ subEnd)

    NE_PROP_R(vec3f, MinVel, "Min. Velocity", "Minimum x-, y- and z-velocities over all particles", true)
    NE_PROP_R(vec3f, MaxVel, "Max. Velocity", "Maximum x-, y- and z-velocities over all particles", true)
    NE_PROP_R(vec3f, MinFrontPos, "Min. Front Position", "Minimum x-, y- and z-positions over all particles", true)
    NE_PROP_R(vec3f, MaxFrontPos, "Max. Front Position", "Maximum x-, y- and z-positions over all particles", true)
    NE_PROP_R(double, AvgTemperature, "Avg. Temperature", "Average temperature over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinTemperature, "Min. Temperature", "Minimum temperature over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxTemperature, "Max. Temperature", "Maximum temperature over all particles contained within the measurement field", true)
    NE_PROP_GROUP_START(ConcnsChemSpecies, "Concentrations Chemical Species", "Concentrations of chemical species diluted in the fluid", true)
    NE_PROP_R(double, AvgConcnChemSpecies1, "Avg. Concn. Chemical Species 1", "Average concentration of chemical species 1 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies1, "Min. Concn. Chemical Species 1", "Minimum concentration of chemical species 1 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies1, "Max. Concn. Chemical Species 1", "Maximum concentration of chemical species 1 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies2, "Avg. Concn. Chemical Species 2", "Average concentration of chemical species 2 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies2, "Min. Concn. Chemical Species 2", "Minimum concentration of chemical species 2 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies2, "Max. Concn. Chemical Species 2", "Maximum concentration of chemical species 2 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies3, "Avg. Concn. Chemical Species 3", "Average concentration of chemical species 3 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies3, "Min. Concn. Chemical Species 3", "Minimum concentration of chemical species 3 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies3, "Max. Concn. Chemical Species 3", "Maximum concentration of chemical species 3 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies4, "Avg. Concn. Chemical Species 4", "Average concentration of chemical species 4 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies4, "Min. Concn. Chemical Species 4", "Minimum concentration of chemical species 4 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies4, "Max. Concn. Chemical Species 4", "Maximum concentration of chemical species 4 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies5, "Avg. Concn. Chemical Species 5", "Average concentration of chemical species 5 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies5, "Min. Concn. Chemical Species 5", "Minimum concentration of chemical species 5 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies5, "Max. Concn. Chemical Species 5", "Maximum concentration of chemical species 5 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies6, "Avg. Concn. Chemical Species 6", "Average concentration of chemical species 6 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies6, "Min. Concn. Chemical Species 6", "Minimum concentration of chemical species 6 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies6, "Max. Concn. Chemical Species 6", "Maximum concentration of chemical species 6 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, AvgConcnChemSpecies7, "Avg. Concn. Chemical Species 7", "Average concentration of chemical species 7 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MinConcnChemSpecies7, "Min. Concn. Chemical Species 7", "Minimum concentration of chemical species 7 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_R(double, MaxConcnChemSpecies7, "Max. Concn. Chemical Species 7", "Maximum concentration of chemical species 7 diluted in the fluid over all particles contained within the measurement field", true)
    NE_PROP_GROUP_END(ConcnsChemSpecies)
    NE_PROP_R(int, NumSolidParticles, "Num. of Solid Particles", "Number of solid particles inside the field.", true)
    NE_PROP_3D_R(vec3f, float, FluidToSolidTotalForce, "Fluid->Solid Total Force", "Total force applied by the fluid on the solid particles; the corresponding solid objects must have Set Show Force/Torque set on.", true)
    NE_PROP_3D_R(vec3f, float, FluidToSolidPressureForce, "Fluid->Solid Pressure Force", "Pressure force applied by the fluid on the solid particles; the corresponding solid objects must have Set Show Force/Torque set on.", true)
    NE_PROP_3D_R(vec3f, float, FluidToSolidFrictionForce, "Fluid->Solid Friction Force", "Friction force applied by the fluid on the solid particles; the corresponding solid objects must have Set Show Force/Torque set on.", true)
    NE_PROP_3D_R(vec3f, float, FluidToSolidAdhesionForce, "Fluid->Solid Adhesion Force", "Adhesion force applied by the fluid on the solid particles; the corresponding solid objects must have Set Show Force/Torque set on.", true)

    Q_PROPERTY(bool exportParticles READ exportParticles WRITE setExportParticles)
    Q_CLASSINFO("name-exportParticles", "Export Stats")
    Q_CLASSINFO("help-exportParticles", "When enabled particles in the measurement field are exported.")

    Q_PROPERTY(SubPropertyStart exportParams READ subStart)

    Q_PROPERTY(bool exportPP READ exportPP WRITE setExportPP DESIGNABLE exportParticles)
    Q_CLASSINFO("name-exportPP", "Export Per Particle")
    Q_CLASSINFO("help-exportPP", "Export particle data per frame.")

    Q_PROPERTY(double exportInt READ exportInt WRITE setExportInt DESIGNABLE exportAtIntervals)
    Q_CLASSINFO("name-exportInt", "Export Interval")
    Q_CLASSINFO("help-exportInt", "The successive interval between data writes.")

    Q_PROPERTY(FilePath exportPath READ exportPath WRITE setExportPath DESIGNABLE exportParticles)
    Q_CLASSINFO("name-exportPath", "Export Path")
    Q_CLASSINFO("help-exportPath", "The path where the data will be exported. - Data Format is time data")

    Q_PROPERTY(MeasureType exportType READ exportType WRITE setExportType DESIGNABLE exportParticles)
    Q_CLASSINFO("name-exportType", "Export Type")
    Q_CLASSINFO("help-exportType", "Type of Data to export height/vel etc")

    Q_PROPERTY(DataFormat exportFormat READ exportFormat WRITE setExportFormat DESIGNABLE exportParticles)
    Q_CLASSINFO("name-exportFormat", "Export Format")
    Q_CLASSINFO("help-exportFormat", "Data Format to Export Particle Data")

    Q_PROPERTY(PlotType plotType READ plotType WRITE setPlotType DESIGNABLE exportParticles)
    Q_CLASSINFO("name-plotType", "Plot Type")
    Q_CLASSINFO("help-plotType", "Type of plot")

    Q_PROPERTY(SubPropertyEnd exportParams_End READ subEnd)

    Q_PROPERTY(bool subdivideMeasurementField READ subdivideMeasurementField WRITE setSubdivideMeasurementField)
    Q_CLASSINFO("name-subdivideMeasurementField", "Subdivide")
    Q_CLASSINFO("help-subdivideMeasurementField", "Enable to see velocity values mapped on a grid defined by the measurement field")

    Q_PROPERTY(vec3i subdiv READ subdiv WRITE setSubdiv DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-subdiv", "Subdivisions")
    Q_CLASSINFO("help-subdiv", "The Number of Subdivision Slices in the Measurement Field to visualize values mapped on a grid")

    Q_PROPERTY(double sdArrowScale READ subDivArrowScale WRITE setSubDivArrowScale DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-sdArrowScale", "Velocity arrow scale")
    Q_CLASSINFO("help-sdArrowScale", "Scaling used in the visualization of velocity arrows in subdivision")

    Q_PROPERTY(double sdMaxArrowLength READ subDivMaxArrowLength WRITE setSubDivMaxArrowLength DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-sdMaxArrowLength", "Velocity Max Arrow Length")
    Q_CLASSINFO("help-sdMaxArrowLength", "Length used in the visualization of velocity arrows in subdivision")

    Q_PROPERTY(double sdArrowDiameter READ subDivArrowDiameter WRITE setSubDivArrowDiameter DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-sdArrowDiameter", "Velocity Arrow Diameter")
    Q_CLASSINFO("help-sdArrowDiameter", "Arrow diameter used in the visualization of velocity arrows in subdivision")

    Q_PROPERTY(double sdCylinderDiameter READ subDivCylinderDiameter WRITE setSubDivCylinderDiameter DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-sdCylinderDiameter", "Velocity Arrow Cylinder Diameter")
    Q_CLASSINFO("help-sdCylinderDiameter", "Cylinder diameter used in the visualization of velocity arrows in subdivision")

    Q_PROPERTY(bool sdNormalizeArrows READ subDivNormalizeArrows WRITE setSubDivNormalizeArrows DESIGNABLE subdivideMeasurementField)
    Q_CLASSINFO("name-sdNormalizeArrows", "Velocity Arrow normalization")
    Q_CLASSINFO("help-sdNormalizeArrows", "Scale the length of the visual velocity arrows so that all of them are the same length")

    Q_PROPERTY( NEBool reset READ reset WRITE setReset USER true )
    Q_CLASSINFO( "name-reset", "Reset" )
    Q_CLASSINFO( "help-reset", "Reset Statistics/Plots and Output Data" )

    Q_PROPERTY(NEColorRamp  ColorRamp READ ColorRamp WRITE setColorRamp USER true)
    Q_CLASSINFO("name-ColorRamp" , "Color Ramp")
    Q_CLASSINFO("help-ColorRamp" , "Color Ramp")

    Q_ENUMS(MeasureType)
    Q_ENUMS(PlotType)
    Q_ENUMS(DataFormat)
    Q_ENUMS(OutputDataType)


public:



    enum DataFormat
    {
        Text,
        CSV
    };

    enum PlotType
    {
        Curve = 0,
        Bars = 1,
        Area = 2
    };


public:
    NEMeasurementField( NENodeFactory *factory = 0 );
    ~NEMeasurementField();

    MeasureType measureType() const;

    uint init( uint initMode = NE::kInitOnCreate );
    void resetProperties() override;

    bool flowCheck(NEPhysicalParticle* p, float rd, double &dt);
    vec3f avgVel() const;
    vec3f& avgVel();
    float maxV() const;
    float& maxV();
    int nParticles() const;
    int& nParticles();
    double avgPressure() const;
    double& avgPressure();
    double avgDensity() const;
    double& avgDensity();
    double totalVolume() const;
    double& totalVolume();
    double flowRate() const;
    double& flowRate();
    vec3f flowPlaneNormal() const;
    bool computeFlowRate() const;

    const vec3i& subdiv() const;
    vec3i subdiv();


    const vec3f &subdivh() const;




    void zeroSubDivQuantities();
    void addParticleVel( const vec3f& pos, const vec3f& vel );
    void addParticleQuantities( const vec3f& pos, float pressure, float density, float mass );
    void doSubDivAveraging();
    bool subdivideMeasurementField() const;
    double subDivArrowScale() const;
    double subDivMaxArrowLength() const;
    double subDivArrowDiameter() const;
    double subDivCylinderDiameter() const;
    bool subDivNormalizeArrows() const;
    double& maxFluidHeight();
    void resetMaxFluidHeight();
    double& maxPressure();
    bool exportParticles() const;
    void setMaxV(float maxV);
    NEColorRamp ColorRamp() const;
    QVector<QPair<qreal, QColor>> getColorList() const;



    std::vector<NESphLiquidParticle*> m_Particles;

    void update();

    FilePath exportPath() const;
    MeasureType exportType() const;
    bool exportPP() const;
    bool exportAtIntervals() const;
    double exportInt() const;
    double &avgFluidHeight() const;
    void setAvgFluidHeight(double avgFluidHeight);
    void resetAvgFluidHeight();
    NEBool reset() const;
    PlotType plotType() const;

    DataFormat exportFormat() const;

    const boost::multi_array< vec3f, 3 >& avgVelField() const ;
    const boost::multi_array< float, 3 >& avgPressureField() const;
    const boost::multi_array< float, 3 >& avgDensityField() const;

    virtual void archive();
    virtual void saveProperties();
    virtual void restoreProperties();

    //! Returns the list of connected fluid solvers
    std::forward_list<NESphLiquidSolver*> connectedFluidSolvers() const;
    //! Connects to a fluid solver
    uint connectToFluidSolver(NESphLiquidSolver* fluidSolver);
    //! Disconnects from a fluid solver
    uint disconnectFromFluidSolver(NESphLiquidSolver* fluidSolver);

    //! Returns the list of connected rigids
    std::forward_list<NESphRigid*> connectedRigids() const;
    //! Connects to a rigid
    uint connectToRigid(NESphRigid* rigid);
    //! Disconnects from a rigid
    uint disconnectFromRigid(NESphRigid* rigid);

public slots:

    virtual void setObjectName(const QString& name);

    void setMeasureType( MeasureType arg );

    vec3f minPos() const;
    vec3f maxPos() const;
    uint SetPosition(const vec3f& Position);
    uint SetScale(const vec3f& Scale);
    uint SetPivot( const vec3f& Pivot );
    void setFlowPlaneNormal(vec3f arg);
    void setComputeFlowRate(bool arg);
    void setSubdiv(vec3i subdiv);
    void setSubdivideMeasurementField(bool arg);
    void setSubDivArrowScale(double arg);
    void setSubDivMaxArrowLength(double arg);
    void setSubDivArrowDiameter(double arg);
    void setSubDivCylinderDiameter(double arg);
    void setSubDivNormalizeArrows(bool arg);
    void setExportParticles(bool arg);
    void setExportPath(FilePath arg);
    void setExportType(MeasureType arg);
    void setExportPP(bool arg);
    void setExportInt( double arg );
    void setReset(NEBool arg);
    void setPlotType(PlotType plotType);
    void setExportFormat(DataFormat arg);
    void setColorRamp(const NEColorRamp& arg);

    void connectedFluidsChangedCallback();
    void connectedChemSpeciesChangedCallback();

signals:
    void setupChart( QString name );
    void clearChart( QString name );
    void updateCharts();
    void updateChart(  QString name, QVector< double > & xValues, QVector< double > yValues );
    void setChartType( QString name );

protected:
    virtual void render();
    virtual void renderPickable();
    virtual void renderPicked();
    void updateMinMaxPos();
    void updateSubDivh();
    bool addSource(NENode* node, NEConn &conn);
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex);

    void setMeasurementFile( FilePath arg );
    NEMeasurementFieldPrivate* P;

    // NENode interface


public:
    virtual uint eval() override;

};


class NEMeasurementFieldFactory : public NENodeFactory
{
public:
    NEMeasurementFieldFactory(NEManager* m);
    ~NEMeasurementFieldFactory();

    QString nodeName();
    uint nodeType();
    QString nodeInherits();
    QString objectName();
    QString nodeVersion();
    QString nodeVendor();
    QString nodeHelp();
    NEObject* createInstance();
};

Q_DECLARE_INTERFACE(NEMeasurementField, "NEMeasurementField")
