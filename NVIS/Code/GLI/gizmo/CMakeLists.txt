cmake_minimum_required(VERSION 3.1.0)

set (PRODUCT_GIZMO "gizmo" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake )

include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    link_libraries( OpenGL32.lib glew32.lib)
endif()

if(APPLE)
add_definitions(-DMAC_OS)
endif()

add_definitions( -DGIZMO_LIBRARY )

set ( GIZMO_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/GizmoTransformMove.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformRender.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformRotate.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformScale.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ZBaseMaths.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ZBaseMaths.inl 
    ${CMAKE_CURRENT_LIST_DIR}/ZMathsFunc.cpp
)

set ( GIZMO_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/GizmoTransform.h 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformMove.h 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformRender.h 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformRotate.h 
    ${CMAKE_CURRENT_LIST_DIR}/GizmoTransformScale.h 
    ${CMAKE_CURRENT_LIST_DIR}/IGizmo.h 
    ${CMAKE_CURRENT_LIST_DIR}/ZBaseDefs.h 
    ${CMAKE_CURRENT_LIST_DIR}/ZBaseMaths.h 
    ${CMAKE_CURRENT_LIST_DIR}/ZMathsFunc.h 
    ${CMAKE_CURRENT_LIST_DIR}/gizmo_global.h
)

source_group ( "Sources" FILES ${GIZMO_SOURCES} )
source_group ( "Headers" FILES ${GIZMO_HEADERS} )

add_library( ${PRODUCT_GIZMO} SHARED
	${GIZMO_SOURCES} ${GIZMO_HEADERS}
)

SET_TARGET_PROPERTIES(${PRODUCT_GIZMO} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_GIZMO} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)


# INSTALL_PREFIX = $$INSTALL_INC_DIR
# INSTALL_HEADERS = $$HEADERS
# for(header, INSTALL_HEADERS) {
#   path = $${INSTALL_PREFIX}/$${dirname(header)}/gizmo
#   path = $$relative_path($$path)
#   eval(headers_$${path}.files += $$header)
#   eval(headers_$${path}.path = $$path)
#   eval(INSTALLS *= headers_$${path})
# }
