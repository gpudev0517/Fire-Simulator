#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_4(class, NESphLiquidSolver, NENodeFactory, NESpatial, NESimulationManager)
NE_FWD_DECL_3(class, NEFluid, NEGrains, NEMixture)
NE_FWD_DECL_6(class, NEParticleFluidSolver, NERigid, NERigidSolver, NEFluidSolver, NEFluidParticle, NE_PRIVATE(NESphLiquidManager))

//!  Manages all SPH Liquid instances. @see NESphLiquidSolver.
/*!
	Sph Liquids can only be instantiated by this class using createObject(QString,QString).
*/
class NESphLiquidManager: public NEPhysicalEntityManager
{
    Q_OBJECT
public:
    //! This class can only be instantiated by the simulation manager
    NESphLiquidManager(QObject* parent);
    virtual ~NESphLiquidManager();
    //! Initializes the manager
    uint init();
    //! Loads stored data of fluids for the current frame.
    double loadFrameData(QString dataPath, int frameNumber);
    //! Stores data of fluids for the current frame.
    double saveFrameData(QString dataPath, int frameNumber);
    //! Deletes all data
    uint cleanSavedData(QString dataPath);
    //! Clears data in memory
    uint cleanMemData();
    //! Deletes all data in the given range
    uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
    //! Writes the cache information of each solver into fout.
    uint writeFrameInfo(QXmlStreamWriter& xmlSW, int frameNumber);
    //! The physical update function. Returns false if e.g. a shock occured. Resimulating required.
    bool integrate();
    //! Updates the drawing buffers of all SPH fluids
    void updateDrawingBuffers();
    //! Preprocesses, at the beginning of the time step
    void preProcess(const std::vector<NERigidSolver*>& rigidSolvers);
    //! Updates all forces acting on the SPH fluids
    uint updateForces();
    //! Mark the periodic ghosts for deletion
    void markPeriodicGhostsForDeletion();
    //! Postprocesses, at the end of the time step
    void postProcess();
    //! Computes the optimal time step for each solver and the scene.
    bool updateTimestep();
    //! Creates a new NESphLiquidSolver instance.
    NENode* createObject(QString, QString, const NEObject*);
    //! Deletes an object and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
    //! Deletes an object and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name);

    //! Returns the list of fluids
    std::forward_list<NEFluid*> fluids() const;
    //! Returns the list of grains
    std::forward_list<NEGrains*> grains() const;
    //! Returns the list of mixtures
    std::forward_list<NEMixture*> mixtures() const;
    //! Returns the list of fluid solvers
    std::forward_list<NEFluidSolver*> fluidSolvers() const;

    //! Reset all SPH liquid related nodes that are managed by this manager
    void resetValues();
    //! Returns the memory consumption of the particles of the liquid solvers
    virtual uint memoryConsumption();
    //! For applying constraints to the solvers
    void performConstraints();
    //! Returns all fluid particles (as NEPhysicalParticle) listed per solver
    std::vector<std::vector<NEFluidParticle*>> fluidParticles() const;
    //! Returns all interaction radii listed per solver
    std::vector<double> interactionRadii() const;

private:
    NE_DECL_PRIVATE(NESphLiquidManager)
};
