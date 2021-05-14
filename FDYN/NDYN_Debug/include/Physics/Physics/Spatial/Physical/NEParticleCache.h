#pragma once

#include "Physics/Spatial/Physical/NEParticle.h"

class NEFEXP_CORE NEParticleCache
{
public:
    NEParticleCache();

    //! Loads stored data of particles.
    bool loadFrameData(QString dataPath, std::vector<NEParticle>& particles);

    //! Stores data of particles
    bool saveFrameData(QString dataPath, std::vector<NEParticle>& particles);


};

