#pragma once

#if NEUTRINO_CORE_BUILD
#define NEFEXP_CORE Q_DECL_EXPORT
#define NECEXP_CORE Q_DECL_EXPORT
#else
#define NEFEXP_CORE Q_DECL_IMPORT
#define NECEXP_CORE Q_DECL_IMPORT
#endif

// Metadata constants
#define NE_PARTICLE_DATA_V1 0xF001F001
#define NE_PARTICLE_DATA_V2 0xF001F002
#define NE_VEC3F_VECTOR_DATA_V1 0xF101F001
#define NE_MESH_DATA_V1 0xF2F1
#define NE_MESH_DATA_V2 0xF3F2F1
#define NE_GRID_DATA_V1 0x0


struct NEIcon
{
    QString iconPath;
    QSize iconSize;
};


typedef std::vector<NEIcon> NENodeIcon;
