#pragma once

#include "Manager/NEManager.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NESimulationManager))
NE_FWD_DECL_5(class, NESphLiquidManager, NESolidManager, NECollisionManager, NEDaemonManager, NEChemManager)

#if NE_GRIDFLUID
NE_FWD_DECL_2(class, NEGridFluidManager, NEGridFluid)
#endif

NE_FWD_DECL_2(class,  NESpatial, NESphLiquidSolver)

#ifdef NE_MSS
NE_FWD_DECL_1(class, NEMSSManager)
#endif

NE_FWD_DECL_3(class, NEParticleFluidSolver, NEFluidSolver, NEGridFluid)

NE_FWD_DECL_1(class, NEGravity)

NE_FWD_DECL_1(class, NEMeasurementField)

#if NE_VOLUME
NE_FWD_DECL_1(class, NEVolumeNode )
#endif

//! Simulation Manager holds pointers to all simulation object managers.
/*!
        Manages the update order of the simulation.
        Derefers the creation of object to the respective manager.
*/
class NEPhysical;


class NECEXP_PHYSICS NESimulationManager final : public NEManager
{
    Q_OBJECT

public:
    NESimulationManager();
    ~NESimulationManager();

    static NESimulationManager& Instance();
    void init(QObject* scene);

    //! Creates an object of an available simulation class.
    NENode* createObject(QString type, QString name, const NEObject* parent);
    //! Deletes an object and removes all dependencies.with trash index
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
    //! Deletes an object and removes all dependencies.without trash index
    NENode* deleteObject(const QString& type ,const QString& name);


    //! Returns the state of the simulation manager, true means initialized, false means deinitialized.
    bool state();




    //! Sends reset to all the participants of the simulation
    uint reset();
    //! The main update routine of the simulation.
    double update();
    //! Returns the elapsed simulation time
    double elapsedTime();
    //! Returns whether simulation is required
    bool requireSimulation();
    //! Returns the current timestep
    const double& timestep() const;
    void setTimestep(double ts);

    //! Returns the list of fluids solvers
    std::forward_list<NEFluidSolver*> fluidSolvers() const;
    //! Returns true if boundary particles should be generated for solid objects
    bool isSamplingRequiredForSolidMeshes();
    //!Returns this scenes default gravity
    NEGravity* defaultGravity() const;



    std::shared_ptr<NESphLiquidManager> sphLiquidManager();
#if NE_GRIDFLUID
    std::shared_ptr<NEGridFluidManager> gridFluidManager();
#endif
#ifdef NE_MSS
    std::shared_ptr<NEMSSManager> MSSManager();
#endif
    std::shared_ptr<NESolidManager> solidManager();
    std::shared_ptr<NECollisionManager> collisionManager();
    std::shared_ptr<NEDaemonManager> daemonManager();
    std::shared_ptr<NEChemManager> chemManager();

    QMap<NEMeasurementField*, NEMeasurementField*>& measurementFields();



public slots:
    //! Initializes the instance. Load all available plugins and static of simulation classes.
    uint init();
    //! Does some last initialization at a later time
    uint postInit();

    //! de initialization
    uint deInit();

    //! Unload Simulation related data
    bool unloadSim();

    //! Clear the simulation memory and all solver information
    void clearSimData();

    //! Deletes all data in memory
    uint cleanMemData();

    //! Initialize the default simulation-related nodes
    void initBasicSceneElements();

    //! Resets simulation manager state
    void resetValues();
    //! Returns the current simulation step
    const int& simulationStep() const;
    //! Resets the simulation step to zero
    void resetSimulationStep();
    //! If an entity changes its requireSimulation state, this method is called/notified.
    void setRequireSimulation(bool b);
    //! Prepares all physicals for simulation.
    uint preparePhysicalsForSimulation(bool timeLineEdited = false);
    //! Makes the default connections among related physical nodes
    void setPhysicalDefaultConnections(bool b);

    //! Load frame information
    uint loadFrameInfo(QString filePath);
    //! Loops over all managers to write the frame info (cache information) of each solver into the frame info file.
    uint writeFrameInfo(QString filePath, int frameNumber);

    //! Crops the saved data (at the end or at the beginning)
    uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);

    //! Loads stored data of simulation objects for the current frame.
    double loadFrameData(QString dataPath, int frameNumber);
    //! Stores data of simulation objects for the current frame.
    double saveFrameData(QString dataPath, int frameNumber);
    //! Deletes all stored data
    uint cleanSavedData(QString dataPath);

    //! Temporarily the simulation manager triggers the default connections of solids, fluids, sources. Works only for single phase fluids
    void makePhysicalDefaultConnection(NENode* node);


    void readAllSimConnections( QXmlStreamReader& xmlReader,
                                QString sceneFormatVersion,
                                std::vector<QString>& m_ConnStrings,
                                std::vector<QString>& m_ConnFromStrings,
                                std::vector<QString>& m_ConnToStrings );

    //! Sets the mode of all physical nodes to cache/inactive or active. Used as a shortcut for reconstructing the surfaces for example
    void setPhysicalNodesCacheMode(bool b);
    //! When a physical object is deleted.
    void physicalObjectDeleted(NE::NEObjectType type, NESpatial* object);
    //! When a physical object is deleted.
    void physicalObjectCreated(NE::NEObjectType type, NESpatial* object);
    //! Called by Scene manager or other manager when the system needs to be simulated from time.
    uint simulate();
    //! Called by Scene Manager or other manager to update the solid/Rigid forces
    uint updateSolidForces();

    //! Called by Scene Manager or other manager to update the colliders/reset and clear
    uint updateCollider();

    uint clearCollider();
    uint resetCollider();

    //! Called by Scene Manager or other manager to update the data collection systems
    void updateDataCollector();


signals:
    //! Signal after all the various sub managers are created
    void managersCreated( QList< std::shared_ptr< NEManager > > managerList );
private:
    NE_DECL_PRIVATE(NESimulationManager)
};

#define NESIMMGR NESimulationManager::Instance()
