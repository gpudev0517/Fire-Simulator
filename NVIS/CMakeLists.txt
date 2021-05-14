cmake_minimum_required(VERSION 3.6)

project(NeutrinoAll)

set ( DEBUG_BUILD OFF  CACHE BOOL "Switch Debug/Release")
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Qt5 COMPONENTS Widgets Xml Network PrintSupport OpenGL Test)

set( EXTERNAL_FILTER_NAME "External")
set( CODE_FILTER_NAME "Neutrino")
set( PLUGIN_FILTER_NAME "Plugins")

add_subdirectory(External)
add_subdirectory(Code)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PRODUCT_NEUTRINO})

