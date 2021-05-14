#pragma once

#include "Manager/NEPhysicalEntityManager.h"

NE_FWD_DECL_4(class, NE_PRIVATE(NEChemManager), NEChemSpecies, NEChemReaction, NEChemSolver)
NE_FWD_DECL_2(class, NEFluidParticle, NEFluidSolver)

class NECEXP_PHYSICS NEChemManager final : public NEPhysicalEntityManager
{
    Q_OBJECT

public:
    NEChemManager(QObject* parent);
    virtual ~NEChemManager();

    //! Does some initialization
    //! Called at the start of the session
    uint init() override;

    //! Does some preprocessing
    //! Called at the start of the time step
    void preprocess(const std::forward_list<NEFluidSolver*>& fluidSolvers);

    //! Solves for the chemical reactions, evolving the chemical species concentrations
    void solveChemReactions(std::vector<NEFluidParticle*>& fluidParticles, const std::forward_list<NEFluidSolver*>& fluidSolvers);

    //! Creates a node and adds appropriate dependencies
    NENode* createObject(QString, QString, const NEObject*);
    //! Deletes a node and removes all dependencies
    NENode* deleteObject(const QString& type, const QString& name, uint trashIndex);
    NENode* deleteObject(const QString& type, const QString& name);

    //! Returns the list of chemical species
    std::vector<NEChemSpecies*> chemSpecies() const;
    //! Returns the list of chemical reactions
    std::vector<NEChemReaction*> chemReactions() const;
    //! Returns the list of chemistry solvers
    std::vector<NEChemSolver*> chemSolvers() const;

private:
    NE_DECL_PRIVATE(NEChemManager)
};
