#pragma once

#if NEUTRINO_NERENDERNEO_BUILD
#define NEFEXP_NERENDERNEO Q_DECL_EXPORT
#define NECEXP_NERENDERNEO Q_DECL_EXPORT
#else
#define NEFEXP_NERENDERNEO Q_DECL_IMPORT
#define NECEXP_NERENDERNEO Q_DECL_IMPORT
#endif