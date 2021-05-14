#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"
#include "Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h"
#include "Physics/Spatial/Physical/Fluid/NEParticleFluidSolver.h"

NE_FWD_DECL_2(class, NE_PRIVATE(NEChemSolver), NEChemReaction)

/*!
  Class defining a solver for chemical system kinetics.
*/

class NECEXP_PHYSICS NEChemSolver : public NENode
{
    Q_OBJECT
    NE_CLASS("Chemistry Solver Properties")

public:
    enum TimeIntegrationType {ExplicitEuler = 0, RungeKutta_4 = 1};

    NE_PROP_RW(TimeIntegrationType, TimeIntegration, "Time Integration", "Numerical method for time integration", false, )
    NE_PROP_RW(double, TimeStep, "Time Step", "Time step", true, )

    Q_ENUMS(TimeIntegrationType)

public:
    NEChemSolver(NENodeFactory* factory);
    ~NEChemSolver();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Does some preprocessing
    //! Called at the start of time step
    void preprocess(const NEParticleFluidSolver* fluidSolver);

    //! Solves for the chemical reactions, evolving the chemical species concentrations
    void solveChemReactions(std::vector<NEFluidParticle*>& fluidParticles, const NEParticleFluidSolver* fluidSolver) const;

    //! Returns the list of connected chemical reactions
    std::vector<NEChemReaction*> connectedChemReactions() const;
    //! Connects to a chemical reaction
    uint connectToChemReaction(NEChemReaction* chemReaction);
    //! Disconnects from a chemical reaction
    uint disconnectFromChemReaction(NEChemReaction* chemReaction);

private:
    NE_DECL_PRIVATE(NEChemSolver)
};

class NEChemSolverFactory : public NENodeFactory
{
public:
    NEChemSolverFactory(NEManager* m);
    ~NEChemSolverFactory();
    NE_DERIVED(NENodeFactory, Public)
};
