#pragma once

// Particle killer nodes
#define NE_PARTICLE_KILLER_EXTENT 1
#define NE_TELEPORT_KILLER 1
#define NE_FLOW_KILLER 1
#define NE_VOLUME_KILLER 1

// Measurement nodes
#define NE_MEASUREMENT_FIELD 1

// Force field nodes
#define NE_GRAVITY 1
#define NE_VORTEX_FORCE 1
#define NE_BOUNDED_FORCE 1

// Communication nodes
#define NE_NET_CMD_SIM_DATA 1

// Particle neigborhood detection nodes
#define NE_BASIC_GRID_COLLIDER 1
#define NE_CELL_INDEXING_COLLIDER 1
#define NE_COMPACT_HASHING_COLLIDER 1
#define NE_SORTED_HASHING_COLLIDER 1

// Data field nodes
#define NE_COMMON_PROJECTION_FIELD 1



// Surface reconstruction nodes
#define NE_SOLENTHALER_MC 0
#define NE_ZHU_BRIDSON_MC 0
#define NE_SOLENTHALER_OPTIMIZED_MC 1
#define NE_SOLENTHALER_OPTIMIZED_VAR_GRID_MC 0
#define NE_ONDERIK_OPTIMIZED_MC 0
#define NE_SOLENTHALER_OPTIMIZED_MC_SPLIT_MERGE 0
#define NE_YUTURK_MC 0
#define NE_ADAPTIVE_GRIDS 0
#define NE_CLAVET_MC 0
#define NE_SKINNING 0



// Grid-based fluid solver manager nodes
#define NE_MAC_SOLVER 0
#define NE_FLIP_SOLVER 0
#define NE_FOG_MAC_SOLVER 0
#define NE_FOG_FLIP_SOLVER 0

// Hair solver nodes
#define NE_FTL_HAIR 0
#define NE_PBD_HAIR 0

// Mesh post-processing nodes
#define NE_MESH_POST_PROCESSOR_TESSENDORF 0
#define NE_MESH_DECIMATOR 1

// Rendering related nodes
#define NE_CAMERA_FACTORY 1
#define NE_PBR_MATERIAL 1
#define NE_PHONG_MATERIAL 1
#define NE_STRUCTURE_MATERIAL 1
#define NE_TOON_MATERIAL 1
#define NE_TONE_MATERIAL 1
#define NE_OCEAN_MATERIAL 1
#define NE_GLASS_MATERIAL 1
#define NE_CLEAR_WATER_MATERIAL 1
#define NE_PARTICLE_WATER_MATERIAL 1
#define NE_VOLUME_MATERIAL 1
#define NE_DIRECTIONAL_LIGHT 1
#define NE_POINT_LIGHT 1
#define NE_SPOT_LIGHT 1
#define NE_AREA_LIGHT 1
#define NE_IMAGE_BASED_LIGHT 1
#define NE_SKY_LIGHT 1

// Grouping nodes
#define NE_SPATIAL_GROUP 1
#define NE_SPATIAL_LAYER 1

// Rigid nodes
#define NE_RIGID_PLANE 1
#define NE_SPH_WAVE_MAKER 1
#define NE_RIGID_BOX 1
#define NE_RIGID_CUBOID 1
#define NE_RIGID_CUSTOM 1
#define NE_RIGID_ANIMATED 1
#define NE_RIGID_CYLINDER 1
#define NE_RIGID_SPHERE 1
#define NE_RIGID_CAPSULE 0

//Clip plane
#define NE_CLIP_PLANE 1
//Null Gizmo
#define NE_GIZMO 1

// Articulated rigid nodes
#define NE_RAG_DOLL 1
#define NE_BRIDGE 1

// SPH solver nodes
#define NE_SE_SPH_SOLVER 1
#define NE_C_SPH_SOLVER 0
#define NE_SI_SPH_SOLVER 0
#define NE_II_SPH_SOLVER 1
#define NE_II_SPH_MP_SOLVER 0
#define NE_I_SPH_SOLVER 0
#define NE_II_SPH_CG_SOLVER 0
#define NE_II_SPH_LOCAL_SOLVER 0
#define NE_SAND_PCI_SPH_SOLVER 0
#define NE_II_SPH_SOLID 0
#define NE_N_II_SPH_SOLVER 1

// Particle source nodes
#define NE_SQUARE_SOURCE 1
#define NE_FLOW_SOURCE 1
#define NE_CIRCLE_SOURCE 1
#define NE_BLOCK_SOURCE 1
#define NE_VOLUME_SOURCE 1
#define NE_TELEPORT_SOURCE 1

// Secondary simulation nodes
#define NE_SPH_FOAM 1
#define NE_UP_SAMPLED_PARTICLE_FLUID 0
#define NE_FA_SPH 0
#define NE_FS_SPH_HR 0

// Misc. nodes
#define NE_GHOST_VOLUME 1
#define NE_PARTICLE_CONSTRAINER 1
#define NE_INFLOW_COUPLER 1
#define NE_PERIODIC_BOUNDARIES 1







