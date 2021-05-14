cmake_minimum_required(VERSION 3.1.0)

set (PRODUCT_QTCSV "qtcsv" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake )

include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

find_package(Qt5Core REQUIRED)

add_definitions( -DQTCSV_LIBRARY )

set ( QTCSV_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/reader.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/stringdata.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/variantdata.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/writer.cpp
)

set ( QTCSV_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/abstractdata.h 
    ${CMAKE_CURRENT_LIST_DIR}/filechecker.h 
    ${CMAKE_CURRENT_LIST_DIR}/qtcsv.h 
    ${CMAKE_CURRENT_LIST_DIR}/qtcsv_global.h 
    ${CMAKE_CURRENT_LIST_DIR}/reader.h 
    ${CMAKE_CURRENT_LIST_DIR}/stringdata.h 
    ${CMAKE_CURRENT_LIST_DIR}/variantdata.h 
    ${CMAKE_CURRENT_LIST_DIR}/writer.h
)

source_group ( "Sources" FILES ${QTCSV_SOURCES} )
source_group ( "Headers" FILES ${QTCSV_HEADERS} )

add_library( ${PRODUCT_QTCSV} SHARED
	${QTCSV_SOURCES} ${QTCSV_HEADERS}
)

SET_TARGET_PROPERTIES(${PRODUCT_QTCSV} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_QTCSV} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

qt5_use_modules(${PRODUCT_QTCSV} Core ) 

INSTALL_PREFIX = $$INSTALL_INC_DIR
INSTALL_HEADERS = $$HEADERS
for(header, INSTALL_HEADERS) {
   path = $${INSTALL_PREFIX}/$${dirname(header)}/qtcsv
   path = $$relative_path($$path)
   eval(headers_$${path}.files += $$header)
   eval(headers_$${path}.path = $$path)
   eval(INSTALLS *= headers_$${path})
}
