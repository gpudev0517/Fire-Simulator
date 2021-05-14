#pragma once

#include "Core/Manager/NEPostProcessorManager.h"

NE_FWD_DECL_1(class, NE_PRIVATE(NEMeshPostProcessorManager))

class NEPostProcessor;
//! Post-processor Manager holds pointers to all post-processors in the scene.
/*!
        A more elaborate class description.
*/
class NECEXP_NEPOSTPROCESSOR NEMeshPostProcessorManager  : public NEPostProcessorManager
{
public:
    NEMeshPostProcessorManager(QObject* scene);
    virtual ~NEMeshPostProcessorManager();

    uint update();
    //! Creates a new post-processor instance.
    NENode* createObject(QString, QString, const NEObject*);
    //! Deletes the post-processor instance and removes all dependencies.
    NENode* deleteObject(const QString& type ,const QString& name, uint trashIndex);
    //! Initializes the instance. Load all available plugins and static of simulation classes.
    uint init();
    //! Loads stored data of simulation objects for the current frame.
    uint loadFrameData(QString dataPath);
    //! Stores data of simulation objects for the current frame.
    uint saveFrameData(QString dataPath);
    //! Deletes all stored data
    uint cleanSavedData(QString dataPath);
    bool hasActivePostProcessors();

private:
    NE_DECL_PRIVATE(NEMeshPostProcessorManager)
};
