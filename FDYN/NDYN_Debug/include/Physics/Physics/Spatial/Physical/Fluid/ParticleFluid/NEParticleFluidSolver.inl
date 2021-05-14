#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"

#include "Manager/NECollisionManager.h"

template<class T> 
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vel);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vels[i-oldSize]);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, double temperature, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vel, temperature);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, double temperature, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vels[i-oldSize], temperature);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vel, concnsChemSpecies);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vels[i-oldSize], concnsChemSpecies);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, const vec3f& vel, double temperature, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vel, temperature, concnsChemSpecies);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addParticles(std::vector<NEParticle>& newParticles, std::vector<vec3f>& vels, double temperature, std::array<double, maxNumOfChemSpecies>& concnsChemSpecies, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());

    // First resize std::vector.
    int oldSize = (int) particles.size();
    int newSize = (int) newParticles.size() + oldSize;
    int newParticlesCount = newSize - oldSize;

    target.resize(newSize);

    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);


//#pragma omp parallel for
    for(int i=newSize - 1; i >= oldSize; i--)
    {
        particles[i] = T(newParticles[i-oldSize], vels[i-oldSize], temperature, concnsChemSpecies);
        newParticles.resize((uint) newParticles.size() - 1);
        particles[i].setDensity(restDensitySingle());

        //the particle id is set based on m_OldParticleCount, in order to guarantee unique id's
        particles[i].setId(m_OldParticleCount+ (i-oldSize));
        particles[i].setSolver(this);
        m_LiquidParticleNeighbors[i].reserve(40);
    }
    //increment the alltime particle count for this solver, in order to get unique ids
    m_OldParticleCount += newParticlesCount;
}

template<class T>
void NEParticleFluidSolver::addPeriodicGhosts(std::vector<NEPhysicalParticle>& newParticles, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());
    int oldSize = (int)particles.size();
    int newSize = (int)newParticles.size()+oldSize;
    target.resize(newSize);
    m_LiquidParticleNeighbors.resize(newSize);
    m_RigidParticleNeighbors.resize(newSize);

    #pragma omp parallel for schedule(guided)
    for (int i = oldSize; i < newSize; i++)
    {
        particles[i] = T(newParticles[i-oldSize]);
        particles[i].setId(m_OldParticleCount+(i-oldSize));
        particles[i].setCellId(NE::kUnregisteredCellId);
        particles[i].setType(NEParticle::kPeriodicGhost);
        particles[i].setParticleType(NEFluidParticle::kInternalParticle);
        particles[i].setSolver(this);
    }

    // Increment the all-time particle count, in order to get unique ids
    m_OldParticleCount += newSize-oldSize;
}

/*!
*/
template<class T>
void NEParticleFluidSolver::deleteParticle(NEParticle* particle, NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());
    int pEnd = (int)particles.size();

    uint index = particle->index();

    if(index != pEnd-1)
    {
        //copy last particle to particle at index
        particles[index] = particles[pEnd-1];
    }
    resizeParticlesVector(pEnd-1);
}

/*!
*/
template<class T>
void NEParticleFluidSolver::deleteParticles(NEVector<T>& target)
{
    std::vector<T>& particles = *(target.getVector());
    int pEnd = (int)particles.size();
    int i = 0;

    while(i<pEnd)
    {
        T& particle = particles[i];
        if(particle.id() != NE::kParticleDeletionId)
        {
            i++;
        }
        else
        {
            particles[i] = particles[pEnd-1];
            pEnd--;
        }
    }
    resizeParticlesVector(pEnd);
}
