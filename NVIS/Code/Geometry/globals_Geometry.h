#pragma once

#if NEUTRINO_GEOMETRY_BUILD
#define NEFEXP_GEOMETRY Q_DECL_EXPORT
#define NECEXP_GEOMETRY Q_DECL_EXPORT
#else
#define NEFEXP_GEOMETRY Q_DECL_IMPORT
#define NECEXP_GEOMETRY Q_DECL_IMPORT
#endif

struct FDYNVariantIds
{
    uint surfaceMaterialId;
    uint fireMaterialId;
    uint failureCriteriaId;
    uint dynamicComboId;
    uint ventCriteriaId;
    uint cableMaterialId;
};

NEFEXP_GEOMETRY struct FDYNVariantIds& getFDYNVarIds();
