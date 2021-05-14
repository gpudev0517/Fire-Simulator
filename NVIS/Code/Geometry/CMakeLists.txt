cmake_minimum_required (VERSION 3.1.0)

set (PRODUCT_GEOMETRY "NEGeometry" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

include (../cmake/common.cmake)

include (utility )
#include (install )

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED )
find_package(Qt5Network REQUIRED)

add_definitions( -DNEUTRINO_GEOMETRY_BUILD  -DNOMINMAX )

set ( PRECOMPILED_SOURCE ${ROOTPATH}/Code/Geometry/PCH_Geometry.cpp )
set ( PRECOMPILED_HEADER PCH_Geometry.h )

if( WIN32 AND MSVC )
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/openVDB.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/openVDB.lib)
	link_libraries(Half.lib)
	link_libraries(tbb.lib)
else()
	# insert for other platform
endif()

# Common Includes #

include_directories(
	${ROOTPATH} 
	${ROOTPATH}/Code 
	${ROOTPATH}/Code/Core 
	${ROOTPATH}/External 
	${ROOTPATH}/External/OpenVDB
)

if(WIN32 AND MSVC )
include_directories(
	${ROOTPATH}/External/Windows/include
)
endif()


if(${USE_GEOM}) 
	set ( GEOMPATH ${ROOTPATH}/Code/Geometry )
	include_directories(${GEOMPATH})
	set ( GEO_RESOURCE_SOURCES 
		Resource/Mesh/NEMeshConstructor.cpp
		Resource/Mesh/NEVolumeToMesh.cpp
	)
	set ( GEO_MANAGER_SOURCES
		Manager/NEMeshVolGeometryManager.cpp 
	)
	set ( GEO_RENDERING_SOURCES
		#Rendering/MeshConstructor/NEClavetMC.cpp 
		Rendering/MeshConstructor/NEMarchingCubes.cpp 
		Rendering/MeshConstructor/NEMCGrid.cpp 
		#Rendering/MeshConstructor/NEOnderikOptimizedMC.cpp 
		Rendering/MeshConstructor/NEParticleInputMC.cpp 
		#Rendering/MeshConstructor/NESolenthalerMC.cpp 
		Rendering/MeshConstructor/NESolenthalerOptimizedMC.cpp 
		#Rendering/MeshConstructor/NESolenthalerOptimizedMCSplitMerge.cpp 
		#Rendering/MeshConstructor/NESolenthalerOptimizedVarGridMC.cpp 
		Rendering/MeshConstructor/NEYuTurkMC.cpp 
		Rendering/MeshConstructor/skinning/kdTree.cpp 
		Rendering/MeshConstructor/skinning/marchingTet.cpp 
		#Rendering/MeshConstructor/skinning/NESkinning.cpp 
		Rendering/MeshConstructor/skinning/slMatrix.cpp 
		Rendering/MeshConstructor/skinning/slUtil.cpp 
		Rendering/MeshConstructor/skinning/slVector.cpp 
		Rendering/MeshConstructor/skinning/smoothingGrid.cpp 	
	)

	set ( GEO_RESOURCE_HEADERS
		Resource/Mesh/NEMeshConstructor.h 
		Resource/Mesh/Volume/NEVolumeToMesh.h 

	)
	set (GEO_MANAGER_HEADERS
		Manager/NEMeshVolGeometryManager.h  
	)
	set ( GEO_RENDERING_HEADERS	
		#Rendering/MeshConstructor/NEClavetMC.h 
		Rendering/MeshConstructor/NEMarchingCubes.h 
		Rendering/MeshConstructor/NEMCGrid.h 
		#Rendering/MeshConstructor/NEOnderikOptimizedMC.h 
		Rendering/MeshConstructor/NEParticleInputMC.h 
		#Rendering/MeshConstructor/NESolenthalerMC.h 
		Rendering/MeshConstructor/NESolenthalerOptimizedMC.h 
		#Rendering/MeshConstructor/NESolenthalerOptimizedMCSplitMerge.h 
		#Rendering/MeshConstructor/NESolenthalerOptimizedMC_Cuda.h 
		#Rendering/MeshConstructor/NESolenthalerOptimizedVarGridMC.h 
		Rendering/MeshConstructor/NEYuTurkMC.h 
		Rendering/MeshConstructor/skinning/kdTree.h 
		Rendering/MeshConstructor/skinning/marchingTet.h 
		#Rendering/MeshConstructor/skinning/NESkinning.h 
		Rendering/MeshConstructor/skinning/slArray.h 
		Rendering/MeshConstructor/skinning/slIO.h 
		Rendering/MeshConstructor/skinning/slMatrix.h 
		Rendering/MeshConstructor/skinning/slUtil.h 
		Rendering/MeshConstructor/skinning/slVector.h 
		Rendering/MeshConstructor/skinning/smoothingGrid.h 
		globals_Geometry.h 
		# PCH_Geometry.h
	)


	if(${CUDA_INSTALLED})
		list( APPEND GEO_RENDERING_SOURCES Rendering/MeshConstructor/NEGVDBSurface.cpp  Rendering/NEGVDB.cpp )
		list( APPEND GEO_RENDERING_HEADERS Rendering/MeshConstructor/NEGVDBSurface.h  Rendering/NEGVDB.h )
	endif()

	source_group ( "Sources\\Resources" FILES ${GEO_RESOURCE_SOURCES} )
	source_group ( "Headers\\Resources" FILES ${GEO_RESOURCE_HEADERS} )

	source_group ( "Sources\\Manager" FILES ${GEO_MANAGER_SOURCES} )
	source_group ( "Headers\\Manager" FILES ${GEO_MANAGER_HEADERS} )

	source_group ( "Sources\\Rendering" FILES ${GEO_RENDERING_SOURCES} )
	source_group ( "Headers\\Rendering" FILES ${GEO_RENDERING_HEADERS} )

endif()

if(${USE_VOLUME}) 
	set ( VOLPATH ${ROOTPATH}/Code/Geometry/Volume )
	include_directories(${VOLPATH})

	set ( GEO_VOLUME_SOURCES
	${VOLPATH}/Core/NEVolume.cpp 
	${VOLPATH}/Core/NEVolumeFactory.cpp 
	${VOLPATH}/Core/NEVolumeNode.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeKiller.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEMeshToVolume.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEPartToVolume.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEPartToMesh.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeCombine.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeFile.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeFilter.cpp 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB.cpp 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_HashGrid.cpp 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_SDF.cpp 
	${VOLPATH}/Physics/Spatial/Volume/NEPartVolumeTools.cpp

	)

	set ( GEO_VOLUME_HEADERS
	${VOLPATH}/Core/NEVolume.h 
	${VOLPATH}/Core/NEVolumeNode.h 
	${VOLPATH}/Core/NEVolumeFactory.h 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeKiller.h 
	${VOLPATH}/Physics/Spatial/Volume/NEMeshToVolume.h 
	${VOLPATH}/Physics/Spatial/Volume/NEPartToVolume.h 
	${VOLPATH}/Physics/Spatial/Volume/NEPartToMesh.h 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeCombine.h 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeFile.h 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeFilter.h 
	${VOLPATH}/Physics/Spatial/Volume/NEVolumeUtils.h 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/kdtree/kdtree.h 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB.h 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_HashGrid.h 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_SDF.h 
	${VOLPATH}/Physics/Spatial/Volume/ParticleVolume/svdcmp.c 
	${VOLPATH}/Physics/Spatial/Volume/NEPartVolumeTools.h
	)

	source_group ( "Sources\\Volume" FILES ${GEO_VOLUME_SOURCES} )
	source_group ( "Headers\\Volume" FILES ${GEO_VOLUME_HEADERS} )

endif()

#message(INCLUDE(NEGeometry): $$USE_FIRE)

if(${USE_FIRE})
	set ( FIREPATH ${ROOTPATH}/Code/Geometry/Volume/Fire )
	include_directories(${FIREPATH})
	set ( GEO_FIRE_SOURCES
			${FIREPATH}/Physics/Spatial/NEFireFile.cpp 
			${FIREPATH}/Physics/Spatial/NEFireCombine.cpp
			${FIREPATH}/Utility/NEFireVolume.cpp
			${FIREPATH}/Utility/NEPlot3DIO.cpp
			${FIREPATH}/Utility/NEPlot3DRead.cpp
	)

	set ( GEO_FIRE_HEADERS
			${FIREPATH}/Physics/Spatial/NEFireFile.h
			${FIREPATH}/Physics/Spatial/NEFireCombine.h
			${FIREPATH}/Utility/NEFireVolume.h
			${FIREPATH}/Utility/NEPlot3DIO.h
			${FIREPATH}/Utility/NEPlot3DRead.h
			${FIREPATH}/Utility/cgnslib.h
	)

	source_group( "Sources\\Fire" FILES ${GEO_FIRE_SOURCES} )
	source_group( "Headers\\Fire" FILES ${GEO_FIRE_HEADERS} )
endif()

if(${CUDA_INSTALLED}) 
	add_definitions( -DCUDA_AVAILABLE )
if( WIN32 AND MSVC )
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/GVDB.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/GVDB.lib)
else()
	# insert for other platform
endif()
endif()


file (GLOB MOC_FILES 
	${GEO_RESOURCE_HEADERS}
	${GEO_MANAGER_HEADERS}
	${GEO_RENDERING_HEADERS}
	${GEO_VOLUME_HEADERS}
	${GEO_FIRE_HEADERS}
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_FILES})


list ( APPEND ALL_SOURCES 
	${GEO_RESOURCE_SOURCES}
	${GEO_MANAGER_SOURCES}
	${GEO_RENDERING_SOURCES}
	${GEO_VOLUME_SOURCES}
	${GEO_FIRE_SOURCES}
	${MOC_SOURCES}
)
ADD_MSVC_PRECOMPILED_HEADER(${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE} ALL_SOURCES)

if( ${CUDA_INSTALLED})
	cuda_add_library( ${PRODUCT_GEOMETRY} SHARED
		${GEO_RESOURCE_HEADERS}
		${GEO_MANAGER_HEADERS}
		${GEO_RENDERING_HEADERS}
		${GEO_VOLUME_HEADERS}
		${GEO_FIRE_HEADERS}
		${ALL_SOURCES}
)
else()
	add_library( ${PRODUCT_GEOMETRY} SHARED
		${GEO_RESOURCE_HEADERS}
		${GEO_MANAGER_HEADERS}
		${GEO_RENDERING_HEADERS}
		${GEO_VOLUME_HEADERS}
		${GEO_FIRE_HEADERS}
		${ALL_SOURCES}
	)
endif()

SET_TARGET_PROPERTIES(${PRODUCT_GEOMETRY} PROPERTIES FOLDER ${CODE_FILTER_NAME})


set_target_properties(${PRODUCT_GEOMETRY} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

qt5_use_modules(${PRODUCT_GEOMETRY} Core Xml Network Widgets)

add_dependencies( ${PRODUCT_GEOMETRY} ${PRODUCT_CORE})

if(${CUDA_INSTALLED})
	add_dependencies(${PRODUCT_GEOMETRY} ${PRODUCT_GVDB})
endif()

if( WIN32 AND MSVC )
	target_link_libraries( ${PRODUCT_GEOMETRY} optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${PRODUCT_CORE}.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${PRODUCT_CORE}.lib)
else()
	# insert for other platform
endif()
