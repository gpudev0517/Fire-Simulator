#pragma once

#include "NENode.h"
#include "Base/NENodeFactory.h"

NE_FWD_DECL_4(class, NE_PRIVATE(NEChemSpecies), NEFluid, NEGrains, NEChemReaction)

/*!
  Class defining a chemical species.
*/

class NECEXP_PHYSICS NEChemSpecies : public NENode
{
    Q_OBJECT
    NE_CLASS("Chemical Species Properties")

    NE_PROP_RW(double, MolarMass, "Molar Mass", "Molar mass of the chemical species", true, )

public:


public:
    NEChemSpecies(NENodeFactory* factory);
    ~NEChemSpecies();

    //! NENode overriden functions
    uint init(uint initMode = NE::kInitOnCreate) override;
    void resetProperties() override;

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

    //! Returns the list of connected chemical reactions
    std::vector<NEChemReaction*> connectedChemReactions() const;
    //! Connects to a chemical reaction
    uint connectToChemReaction(NEChemReaction* chemReaction);
    //! Disconnects from a chemical reaction
    uint disconnectFromChemReaction(NEChemReaction* chemReaction);

private:
    NE_DECL_PRIVATE(NEChemSpecies)
};

class NEChemSpeciesFactory : public NENodeFactory
{
public:
    NEChemSpeciesFactory(NEManager* m);
    ~NEChemSpeciesFactory();
    NE_DERIVED(NENodeFactory, Public)
};
