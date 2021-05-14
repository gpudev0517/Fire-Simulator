#pragma once

#include "Manager/NEManager.h"

class NESpatial;

/*! \brief NEPhysicalEntityManager class is the base class that can create and delete physics related nodes in Neutrino
 *
 * The class allows creation and deletion of various types of physcially based nodesin Neutrino.
 * Different manager classes can override the functionality in this class to create more
 * sophisticated managers
*/

class NEPhysicalEntityManager : public NEManager
{
    Q_OBJECT
public:
    NEPhysicalEntityManager(QObject* parent);
    virtual ~NEPhysicalEntityManager();
    //! Returns the memory consumption of the managed physical entities
    virtual uint memoryConsumption();

signals:
    //! Emitted when the number of nodes that requires simulation update changes
    void requireSimulationChanged(bool);
    void physicalObjectCreated(NE::NEObjectType, NESpatial*);
    void physicalObjectDeleted(NE::NEObjectType, NESpatial*);

public slots:
    //! If an entity changes its requireSimulation state, this method is called/notified.
    void setRequireSimulation(bool b);
    //! Returns whether simulation is required for any node maintained by this manager.
    bool requireSimulation();

protected:	
    int m_cSimulate;
};
