#pragma once

#include "Spatial/NESpatial.h"


class NEKiller : public NESpatial
{
    Q_OBJECT

public:
	NEKiller(NENodeFactory* factory) : NESpatial(factory) {}
    virtual ~NEKiller() {}

    //! Resets some values
    //! Called when restarting the simulation from the start
    NEFEXP_CORE virtual void reset() =0;

    //! Does some pre-processing
    //! Called at the start of each time step
    virtual void preProcess() =0;

    //! Does some post-processing
    //! Called at the end of each time step
    virtual void postProcess() =0;

    //! For each fluid particle and solid particle, decides whether to "kill" it or not;
    //! if to kill, either marks it for deletion or transforms it into an outflow ghost (fluid only)
    //! Called at each time step
    virtual uint checkAndMarkToKill() =0;
};

Q_DECLARE_INTERFACE(NEKiller, "NEKiller")
