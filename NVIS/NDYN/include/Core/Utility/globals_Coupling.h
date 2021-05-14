#pragma once

#ifdef _WIN32
    #define NE_DECL_EXPORT __declspec(dllexport)
    #define NE_DECL_IMPORT __declspec(dllimport)
#else
    #define NE_DECL_EXPORT
    #define NE_DECL_IMPORT
#endif

#if NEUTRINO_CORE_BUILD
#define NEFEXP_COUPLING NE_DECL_EXPORT
#define NECEXP_COUPLING NE_DECL_EXPORT
#else
#define NEFEXP_COUPLING NE_DECL_IMPORT
#define NECEXP_COUPLING NE_DECL_IMPORT
#endif

