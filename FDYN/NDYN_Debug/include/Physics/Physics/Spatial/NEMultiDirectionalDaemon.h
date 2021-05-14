#pragma once

class NECEXP_PHYSICS NEMultiDirectionalDaemon
{
public:
    NEMultiDirectionalDaemon() {}
    virtual ~NEMultiDirectionalDaemon(){}

    //! Return the velocity for the given source index and position
    virtual vec3f velocity(uint sourceId, const vec3f& pos) const =0;
    //! Return the adjusted speed along the normal direction for the given source index and position
    //! This adjusted speed is to ensure a right flow rate and to be used for the displacement of particles
    virtual double normalAdjSpeed(uint sourceId, const vec3f& pos) const =0;
    //! Return the adjusted velocity along the normal direction for the given source index and position
    //! This adjusted velocity is to ensure a right flow rate and to be used for the displacement of particles
    virtual vec3f normalAdjVelocity(uint sourceId, const vec3f& pos) const =0;
};
