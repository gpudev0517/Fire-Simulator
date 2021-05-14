#pragma once

class NECEXP_PHYSICS NEThermalParticle
{
public:
    NEThermalParticle();

    NEThermalParticle(double temperature);

     double temperature() const;
     double& temperature();

protected:
    double m_Temperature;
};
