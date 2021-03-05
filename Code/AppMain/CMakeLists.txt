#-------------------------------------------------
#
#
#-------------------------------------------------
#

cmake_minimum_required (VERSION 3.1.0)

set (PRODUCT_NEUTRINO "FDYN" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)
set ( CMAKE_AUTORCC ON )

include(../cmake/common.cmake)
include(utility)


set ( PRECOMPILED_SOURCE ${ROOTPATH}/Code/AppMain/PCH_AppMain.cpp )
set ( PRECOMPILED_HEADER PCH_AppMain.h )

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5Gui REQUIRED )

include_directories( 
    ./ 
    ${ROOTPATH}/Code  
    ${ROOTPATH}/Code/UI
)

# Platform Specific Configurations #

if(WIN32 AND MSVC ) 
	include_directories( ${ROOTPATH}/External/windows/include )
    link_libraries(OpenGL32.lib glew32.lib )
    set (RC_FILE ${ROOTPATH}/Code/UI/Resources/icons/Neutrino.rc )
    
    link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NECore.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NECore.lib)
    link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEGeometry.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEGeometry.lib)
    link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NERendering.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NERendering.lib)
    link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEGLI.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEGLI.lib)
    link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEUI.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEUI.lib)


endif()

if(UNIX AND NOT APPLE)
	# INCLUDEPATH +=  $$ROOTPATH/External/Linux/include/
    # QMAKE_CXXFLAGS += -fopenmp
    # LIBS += -L$$ROOTPATH/External/Linux/lib -lGL
    # QMAKE_LFLAGS += '-Wl,-rpath,\'$$ORIGIN/Libs\''
endif()

if(APPLE)
    # LIBS += -L$$ROOTPATH/External/macosx/lib
    # QMAKE_LFLAGS += '-Wl,-rpath,\'$$ORIGIN/Libs\''
    # ICON = $$ROOTPATH/Code/UI/Resources/icons/Simple.icns
endif()


set ( MAIN_SOURCES AppMain.cpp )

set (MAIN_HEADERS 
	# PCH_AppMain.h
)


file (GLOB MOC_FILES 
	${MAIN_HEADERS}
)

qt5_wrap_cpp(MOC_SOURCES ${MOC_FILES})

list ( APPEND ALL_SOURCES 
	${MAIN_SOURCES}
	${MOC_SOURCES}
)

ADD_MSVC_PRECOMPILED_HEADER(${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE} ALL_SOURCES)


add_executable( ${PRODUCT_NEUTRINO}
	${MAIN_HEADERS}
	${RC_FILE}
	${ALL_SOURCES}
)
set_target_properties(${PRODUCT_NEUTRINO} PROPERTIES FOLDER ${CODE_FILTER_NAME})
if( WIN32 AND MSVC )
set_target_properties(${PRODUCT_NEUTRINO} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/../../Data)
endif()

set_target_properties(${PRODUCT_NEUTRINO} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)


qt5_use_modules(${PRODUCT_NEUTRINO} Gui Core Xml Network Widgets OpenGL OpenGLExtensions)


add_dependencies( ${PRODUCT_NEUTRINO} ${PRODUCT_CORE} ${PRODUCT_GEOMETRY} ${PRODUCT_RENDERING})

if( WIN32 AND MSVC )
    if(EXISTS ${CMAKE_SOURCE_DIR}/External/Windows )
        file(GLOB_RECURSE ALL_DLLS "${CMAKE_SOURCE_DIR}/External/Windows/lib/*.dll")
        foreach(dll in ${ALL_DLLS})
            get_filename_component(dll_name ${dll} NAME ) 
            
            if( ${dll_name} MATCHES ".dll" )
                if( ${DEBUG_BUILD} )
                add_custom_command(TARGET ${PRODUCT_NEUTRINO}  POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy ${dll} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/${dll_name}
                    COMMENT "Copying library ${dll_name} to output directory."
                    VERBATIM
                )
                else()
                add_custom_command(TARGET ${PRODUCT_NEUTRINO}  POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy ${dll} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/${dll_name}
                    COMMENT "Copying library ${dll_name} to output directory."
                    VERBATIM
                )
                endif()
                # message( " dll is ${dll_name}, ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${dll_name} " )
            endif()
        endforeach()
    else()
        message("External libraries is not exist, please copy external library and includes.")
    endif()
endif()
