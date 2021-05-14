#pragma once

class NECEXP_PHYSICS NEDirectionalDaemon
{
public:
    NEDirectionalDaemon() {}
    virtual ~NEDirectionalDaemon(){}

    //! Return the speed along the normal direction for the given position
    virtual double normalSpeed(const vec3f& pos) const =0;
    //! Return the velocity along the normal direction for the given position
    virtual vec3f normalVelocity(const vec3f& pos) const =0;
};
