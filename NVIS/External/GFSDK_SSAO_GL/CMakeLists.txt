cmake_minimum_required(VERSION 3.1.0)

set (PRODUCT_GFSDK "GFSDK_SSAO_GL" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake )

include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

find_package(Qt5Core REQUIRED)
find_package(Qt5OpenGL REQUIRED)

if(WIN32 AND MSVC)
    link_libraries(OpenGL32.lib glew32.lib)
endif()

if(APPLE)
    # DEFINES += MAC_OS MACOSX
elseif(UNIX)
    # LIBS += -lGL
    # LIBS += -lGLEW
endif()

if(WIN32 AND MSVC)
add_definitions(-D_WINDLL -D_WINDOWS -DUNICODE)
endif()

add_definitions( -DGFSDK_SSAO_GL_LIBRARY -DGLEW_STATIC)
add_definitions( -DSUPPORT_GL=1 )

set ( GFSDK_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/API.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/AppState_GL.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ConstantBuffers.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/ProjectionMatrixInfo.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/RandomTexture.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/Renderer_GL.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/Shaders_GL.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/States_GL.cpp 
)

set ( SHADER_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/shaders/BlurX_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BlurY_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/CoarseAO_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/CopyDepth_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/DebugNormals_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/DeinterleaveDepth_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/LinearizeDepth_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/ReconstructNormal_PS.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/ReinterleaveAO_PS.cpp
)

set ( GFSDK_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/AppState_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/BuildVersion.h 
    ${CMAKE_CURRENT_LIST_DIR}/Common.h 
    ${CMAKE_CURRENT_LIST_DIR}/Common_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/ConstantBuffers.h 
    ${CMAKE_CURRENT_LIST_DIR}/GLSLProgram.h 
    ${CMAKE_CURRENT_LIST_DIR}/InputDepthInfo.h 
    ${CMAKE_CURRENT_LIST_DIR}/InputNormalInfo.h 
    ${CMAKE_CURRENT_LIST_DIR}/InputViewport.h 
    ${CMAKE_CURRENT_LIST_DIR}/MathUtil.h 
    ${CMAKE_CURRENT_LIST_DIR}/MatrixView.h 
    ${CMAKE_CURRENT_LIST_DIR}/OutputInfo.h 
    ${CMAKE_CURRENT_LIST_DIR}/PerfMarkers.h 
    ${CMAKE_CURRENT_LIST_DIR}/ProjectionMatrixInfo.h 
    ${CMAKE_CURRENT_LIST_DIR}/RandomTexture.h 
    ${CMAKE_CURRENT_LIST_DIR}/Renderer_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/RenderOptions.h 
    ${CMAKE_CURRENT_LIST_DIR}/RenderTargets_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/Shaders_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/States_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/TextureUtil.h 
    ${CMAKE_CURRENT_LIST_DIR}/TimestampQueries.h 
    ${CMAKE_CURRENT_LIST_DIR}/UserTexture.h 
    ${CMAKE_CURRENT_LIST_DIR}/VAO_GL.h 
    ${CMAKE_CURRENT_LIST_DIR}/Viewports.h 
    ${CMAKE_CURRENT_LIST_DIR}/GFSDK_SSAO.h 
)

set ( SHADER_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BlurX_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/BlurY_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/CoarseAO_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/CopyDepth_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/DebugNormals_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/DeinterleaveDepth_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/LinearizeDepth_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/ReconstructNormal_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/ReinterleaveAO_PS.h 
    ${CMAKE_CURRENT_LIST_DIR}/shaders/SharedDefines.h 
    #${CMAKE_CURRENT_LIST_DIR}/shaders/ConstantBuffers.hlsl
)

source_group ( "Sources" FILES ${GFSDK_SOURCES} )
source_group ( "Headers" FILES ${GFSDK_HEADERS} )

source_group ( "Sources\\shaders" FILES ${SHADER_SOURCES} )
source_group ( "Headers\\shaders" FILES ${SHADER_HEADERS} )


add_library( ${PRODUCT_GFSDK} SHARED
	${GFSDK_SOURCES} ${GFSDK_HEADERS}
	#${SHADER_SOURCES} ${SHADER_HEADERS}
)

SET_TARGET_PROPERTIES(${PRODUCT_GFSDK} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_GFSDK} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

qt5_use_modules(${PRODUCT_GFSDK} Core OpenGL) 

if(UNIX)
    # target.path = /usr/lib
    # INSTALLS += target
endif()