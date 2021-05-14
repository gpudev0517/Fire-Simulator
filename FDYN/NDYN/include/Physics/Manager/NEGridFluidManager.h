#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_3(class, NESpatial, NEFluid, NE_PRIVATE(NEGridFluidManager))

class NEGridFluidManager final : public NEPhysicalEntityManager
{
	Q_OBJECT
public:
	NEGridFluidManager(QObject* parent);
	virtual ~NEGridFluidManager();

	uint init();
	//! The physical update functions.
	uint updatePredict();
	uint updateCorrect();
	//! Creates a new grid solver instance.
	NENode* createObject(QString, QString, const NEObject*);
	//! Deletes an object and removes all dependencies.
	NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
	//! Deletes an object and removes all dependencies.
	NENode* deleteObject(const QString& type ,const QString& name);

	//// SAVING AND LOADING OF DATA /////
	//! Loads stored data of fluids for the current frame.
	double loadFrameData(QString dataPath, const int frameNumber);
	//! Stores data of fluids for the current frame.
	double saveFrameData(QString dataPath, const int frameNumber);
	//! Deletes all data
	uint cleanSavedData(QString dataPath);
	//! Crops the data
	uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
	//! Writes the cache information of each solver into fout.
	uint writeFrameInfo(QXmlStreamWriter& xmlSW, int frameNumber);
	//!Fills a list of pointers to all fluids in the manager (given as a parameter)
	void fluids(QList<NEFluid*>& currentFluids);
	//! Reinitializes the fluid grids. 
	void prepareFluidGridsForSimulation();
        bool updateTimestep();
	//! Sets the timestep of all solvers to the scene timestep
	bool clampToSceneTimestep();
	//! Returns the memory consumption of the particles of this solver
	uint memoryConsumption();
    void updateDrawingBuffers();

signals:
	void gridFluidCreated(NE::NEObjectType, NESpatial*);
	void gridFluidDeleted(NE::NEObjectType, NESpatial*);

private:
	NE_DECL_PRIVATE(NEGridFluidManager)

};
