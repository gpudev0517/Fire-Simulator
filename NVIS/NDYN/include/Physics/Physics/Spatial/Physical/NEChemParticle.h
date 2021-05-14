#pragma once

#include <array>

class NECEXP_PHYSICS NEChemParticle
{
public:
    NEChemParticle();

    NEChemParticle(const std::array<double, maxNumOfChemSpecies>& concnsChemSpecies);

     std::array<double, maxNumOfChemSpecies> concnsChemSpecies() const;
     std::array<double, maxNumOfChemSpecies>& concnsChemSpecies();

protected:
    std::array<double, maxNumOfChemSpecies> m_ConcnsChemSpecies;
};
