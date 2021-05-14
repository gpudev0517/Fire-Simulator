cmake_minimum_required (VERSION 3.1.0)

set (PRODUCT_PYTHONQT "PythonQt" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake)

include (utility)
include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# find Qt, assume qt version is 5
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

foreach(value ${Qt5Core_INCLUDE_DIRS})
	if(${value} MATCHES "QtCore")
		include_directories( ${value}/${Qt5Core_VERSION}/QtCore ${value}/${Qt5Core_VERSION} )
	endif()
endforeach()

if( ${PYTHONQT_STATIC} )
	set(LIB_TYPE STATIC)
else()
	set(LIB_TYPE SHARED)
endif()


add_definitions(-DPYTHONQT_CATCH_ALL_EXCEPTIONS)
add_definitions(/wd4273)

set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Zc:strictStrings")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR})

include (../build/common.cmake)
include (../build/python.cmake)

include (./src.cmake)

# INSTALL_PREFIX = $$INSTALL_INC_DIR
# INSTALL_HEADERS = $$HEADERS
# for(header, INSTALL_HEADERS) {
#   path = $${INSTALL_PREFIX}/$${dirname(header)}/pythonqt/src
#   path = $$relative_path($$path)
#   eval(headers_$${path}.files += $$header)
#   eval(headers_$${path}.path = $$path)
#   eval(INSTALLS *= headers_$${path})
# }

add_library(${PRODUCT_PYTHONQT} ${LIB_TYPE}
	${MAIN_SOURCES}
	${MAIN_HEADERS}
	${GUI_SOURCES}
	${GUI_HEADERS} 
)

SET_TARGET_PROPERTIES(${PRODUCT_PYTHONQT} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_PYTHONQT} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

qt5_use_modules(${PRODUCT_PYTHONQT} Core Widgets) 

include(${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin.cmake)
include(${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin.cmake)

