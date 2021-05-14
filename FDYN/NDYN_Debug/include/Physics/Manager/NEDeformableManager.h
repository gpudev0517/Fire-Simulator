#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEDeformableManager))

class NEDeformableManager final : public NEPhysicalEntityManager
{
	Q_OBJECT
public:
	NEDeformableManager(QObject* parent);
	virtual ~NEDeformableManager();
	NENode* createObject(QString type, QString name, const NEObject* parent);
	//! Deletes an object and removes all dependencies.
	NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
	NENode* deleteObject(const QString& type ,const QString& name);
	uint init();
	void update();
	uint prepareDeformablesForSimulation();
	void updateForces();
	void syncDeformables();
	//// SAVING AND LOADING OF DATA /////
	//! Loads stored data of deformable for the current frame.
	double loadFrameData(QString dataPath, int frameNumber);
	//! Stores data of deformable for the current frame.
	double saveFrameData(QString dataPath, int frameNumber);
	//! Deletes all data
	uint cleanSavedData(QString dataPath);
	//! Deletes all data at the end or at the beginning
	uint cropSavedData(QString dataPath, uint fromFrame, uint toFrame, bool moveToZeroFrame=false);
	//! Writes the cache information of each solver into fout.
	uint writeFrameInfo(QXmlStreamWriter& xmlSW, int frameNumber);
	//! Sets the behavior of the nodes either to cache, if b is true otherwise to active 
	void setCacheMode(bool b);
	//! Returns the memory consumption of the particles of the deformables
	uint memoryConsumption();



signals:
	void deformableObjectCreated(NE::NEObjectType);

private:
	NE_DECL_PRIVATE(NEDeformableManager)
};
