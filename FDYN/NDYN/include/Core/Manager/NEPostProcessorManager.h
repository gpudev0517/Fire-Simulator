#pragma once

#include "Manager/NEManager.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEPostProcessorManager))

//! Post-processor Manager holds pointers to all post-processors in the scene.
/*!
	A more elaborate class description.
*/
class NECEXP_CORE NEPostProcessorManager  : public NEManager
{
public:
	NEPostProcessorManager(QObject* scene);
	virtual ~NEPostProcessorManager();

    virtual uint update();
	//! Creates a new post-processor instance.
	NENode* createObject(QString, QString, const NEObject*);
	//! Deletes the post-processor instance and removes all dependencies.
	NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
	//! Initializes the instance. Load all available plugins and static of simulation classes.
    virtual uint init();
	//! Loads stored data of simulation objects for the current frame.
    virtual uint loadFrameData(QString dataPath);
	//! Stores data of simulation objects for the current frame.
    virtual uint saveFrameData(QString dataPath);
	//! Deletes all stored data
    virtual uint cleanSavedData(QString dataPath);
    virtual bool hasActivePostProcessors();

private:
	NE_DECL_PRIVATE(NEPostProcessorManager)
};
