cmake_minimum_required(VERSION 3.1.0)

set (PRODUCT_OPENVDB "OpenVDB" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake )

include (utility)
include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

add_definitions(/DOPENVDB_LIBRARY /DOPENVDB_PRIVATE)

set ( OPENVDB_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/openvdb/Grid.cc
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/openvdb.cc
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/Platform.cc 
)
set (OPENVDB_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/Exceptions.h
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/Grid.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/Metadata.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/openvdb.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/Platform.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/PlatformConfig.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/Types.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/version.h 
)
set (IO_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Archive.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Compression.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/File.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/GridDescriptor.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Queue.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Stream.cc 
	${CMAKE_CURRENT_LIST_DIR}/openvdb/io/TempFile.cc
)
set (IO_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Archive.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Compression.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/File.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/GridDescriptor.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Queue.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/io/Stream.h 
	${CMAKE_CURRENT_LIST_DIR}/openvdb/io/io.h 
	${CMAKE_CURRENT_LIST_DIR}/openvdb/io/TempFile.h 
)
set (MATH_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Hermite.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Maps.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Proximity.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/QuantizedUnitVec.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Transform.cc 
)
set (MATH_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/math/BBox.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Coord.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/FiniteDifference.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Hermite.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/LegacyFrustum.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Maps.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Mat.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Mat3.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Mat4.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Math.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Operators.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Proximity.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/QuantizedUnitVec.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Quat.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Ray.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Stats.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Stencils.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Transform.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Tuple.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Vec2.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Vec3.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/math/Vec4.h 
	${CMAKE_CURRENT_LIST_DIR}/openvdb/math/DDA.h
)
set (METADATA_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/openvdb/metadata/Metadata.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/metadata/MetaMap.cc 
)
set (METADATA_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/metadata/Metadata.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/metadata/MetaMap.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/metadata/StringMetadata.h 
)
set (UTIL_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/openvdb/util/Formats.cc 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/Util.cc 
)
set (UTIL_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/util/Formats.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/logging.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/MapsUtil.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/Name.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/NodeMasks.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/NullInterrupter.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/util/Util.h 
)

set (TOOLS_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Composite.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/ChangeBackground.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Dense.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Filter.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/GridOperators.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/GridTransformer.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Interpolation.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetAdvect.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetFilter.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetFracture.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetMeasure.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetMorph.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetRebuild.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetSphere.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetTracker.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/LevelSetUtil.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/MeshToVolume.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Morphology.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/ParticlesToLevelSet.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/PointAdvect.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/PointPartitioner.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/PointScatter.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Prune.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/RayIntersector.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/RayTracer.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/SignedFloodFill.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/Statistics.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/ValueTransformer.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/VectorTransformer.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/VolumeToMesh.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tools/VolumeToSpheres.h 
)
set (TREE_HEADERS 
	${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/InternalNode.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/Iterator.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/LeafManager.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/LeafNode.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/LeafNodeBool.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/NodeManager.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/NodeUnion.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/RootNode.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/Tree.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/TreeIterator.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/Util.h 
    ${CMAKE_CURRENT_LIST_DIR}/openvdb/tree/ValueAccessor.h 
)

source_group ( "Sources\\openvdb" FILES ${OPENVDB_SOURCES} )
source_group ( "Headers\\openvdb" FILES ${OPENVDB_HEADERS} )
source_group ( "Sources\\openvdb\\io" FILES ${IO_SOURCES} )
source_group ( "Headers\\openvdb\\io" FILES ${IO_HEADERS} )
source_group ( "Sources\\openvdb\\math" FILES ${MATH_SOURCES} )
source_group ( "Headers\\openvdb\\math" FILES ${MATH_HEADERS} )
source_group ( "Sources\\openvdb\\metadata" FILES ${METADATA_SOURCES} )
source_group ( "Headers\\openvdb\\metadata" FILES ${METADATA_HEADERS} )
source_group ( "Sources\\openvdb\\util" FILES ${UTIL_SOURCES} )
source_group ( "Headers\\openvdb\\util" FILES ${UTIL_HEADERS} )
source_group ( "Headers\\openvdb\\tools" FILES ${TOOLS_HEADERS} )
source_group ( "Headers\\openvdb\\tree" FILES ${TREE_HEADERS} )

if(WIN32 AND MSVC)
	add_definitions(/DZLIB_DLL /DZLIB_WINAPI)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
	link_libraries(zlibwapi.lib Half.lib)
elseif(APPLE)
endif()

add_library(${PRODUCT_OPENVDB} SHARED
	${OPENVDB_SOURCES} ${OPENVDB_HEADERS}
	${IO_SOURCES} ${IO_HEADERS}
	${MATH_SOURCES} ${MATH_HEADERS}
	${METADATA_SOURCES} ${METADATA_HEADERS} 
	${UTIL_SOURCES} ${UTIL_HEADERS}
	${TOOLS_HEADERS}
	${TREE_HEADERS}
)

SET_TARGET_PROPERTIES(${PRODUCT_OPENVDB} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_OPENVDB} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

