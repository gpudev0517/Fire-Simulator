cmake_minimum_required(VERSION 3.1.0)

set ( PRODUCT_NETCMDSIMDATA "NENetCmdSimData" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../../../cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

include(../../../../Code/cmake/common.cmake)
include(plugin_config)

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5Network REQUIRED )

add_definitions(-DNEUTRINO_PLUGINS_BUILD -DNETCMDSIMDATA_LIBRARY)


set ( PRECOMPILED_SOURCE ${ROOTPATH}/Code/Plugins/DataCollectors/NetCmdSimData/PCH_NetCmdSimData.cpp )
set ( PRECOMPILED_HEADER PCH_NetCmdSimData.h )

include_directories(
	${ROOTPATH}/Code/Core 
	${ROOTPATH}/Code/Plugins
	${ROOTPATH}/External/iniparser/iniparser 
	${ROOTPATH}/External/QXmpp/src/client 
	${ROOTPATH}/External/QXmpp/src/base
)
if( WIN32 AND MSVC )
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NECore.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NECore.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEGeometry.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEGeometry.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NENetCmd.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NENetCmd.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/iniparser.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/iniparser.lib)
else()
	# insert for other platform
endif()

set ( SIMDATA_SOURCES
	Physics/Spatial/NENetCmdSimData.cpp
)

set ( SIMDATA_HEADERS
	Physics/Spatial/NENetCmdSimData.h 
	# PCH_NetCmdSimData.h 
	../../globals_Plugin.h
)



file (GLOB MOC_FILES 
	${SIMDATA_HEADERS}
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_FILES})


list ( APPEND ALL_SOURCES 
	${SIMDATA_SOURCES}
	${MOC_SOURCES}
)
ADD_MSVC_PRECOMPILED_HEADER(${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE} ALL_SOURCES)



add_library( ${PRODUCT_NETCMDSIMDATA} SHARED
	${SIMDATA_HEADERS}
	${ALL_SOURCES}
)

SET_TARGET_PROPERTIES(${PRODUCT_NETCMDSIMDATA} PROPERTIES FOLDER "${CODE_FILTER_NAME}/${PLUGIN_FILTER_NAME}")

set_target_properties(${PRODUCT_NETCMDSIMDATA} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

add_dependencies( ${PRODUCT_NETCMDSIMDATA} ${PRODUCT_CORE} ${PRODUCT_GEOMETRY} ${PRODUCT_NETCMD})
qt5_use_modules(${PRODUCT_NETCMDSIMDATA} Core Xml Network Widgets)

pluginCopyDir(Spatial/DataCollector ${ROOTPATH}/../.. ${PRODUCT_NETCMDSIMDATA})
