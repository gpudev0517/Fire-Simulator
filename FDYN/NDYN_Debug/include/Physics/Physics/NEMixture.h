#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_4(class, NE_PRIVATE(NEMixture), NEFluid, NEGrains, NEFluidSolver)

/*!
 * Class defining a mixture of fluids or a mixture of fluid and grains
 */

class NECEXP_PHYSICS NEMixture : public NENode
{
    Q_OBJECT
    NE_CLASS("Fluid-Mixture Properties")

public:
    NEMixture(NENodeFactory* factory);
    ~NEMixture();

    //! NENode overriden functions
    bool addSource(NENode* node, NEConn& conn) override;
    void removeSource(NENode* node, uint type, uint sIndex, uint tIndex) override;

    //! Returns the list of connected fluids
    std::vector<NEFluid*> connectedFluids() const;
    //! Connects to a fluid
    uint connectToFluid(NEFluid* fluid);
    //! Disconnects from a fluid
    uint disconnectFromFluid(NEFluid* fluid);

    //! Returns the list of connected grains
    std::vector<NEGrains*> connectedGrains() const;
    //! Connects to grains
    uint connectToGrains(NEGrains* grains);
    //! Disconnects from grains
    uint disconnectFromGrains(NEGrains* grains);

    //! Returns the list of connected fluid solvers
    std::vector<NEFluidSolver*> connectedFluidSolvers() const;
    //! Connects to a fluid solver
    uint connectToFluidSolver(NEFluidSolver* fluidSolver);
    //! Disconnects from a fluid solver
    uint disconnectFromFluidSolver(NEFluidSolver* fluidSolver);

public slots:
    void connectedChemSpeciesChangedCallback();

signals:
    void connectedFluidsChanged();
    void connectedGrainsChanged();
    void connectedChemSpeciesChanged();

private:
    NE_DECL_PRIVATE(NEMixture)
};

class NEMixtureFactory : public NENodeFactory
{
public:
    NEMixtureFactory(NEManager* m);
    virtual ~NEMixtureFactory();
    NE_DERIVED(NENodeFactory, Public)
};
