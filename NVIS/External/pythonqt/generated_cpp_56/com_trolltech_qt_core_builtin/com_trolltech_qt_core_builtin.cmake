cmake_minimum_required(VERSION 3.1.0)

set (TROLTECH_CORE_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_core_builtin0.cpp
    ${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_core_builtin_init.cpp
)

set (TROLTECH_CORE_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_core_builtin0.h
)

source_group("com_trolltech_qt_core_builtin\\Sources" FILES ${TROLTECH_CORE_SOURCES} )
source_group("com_trolltech_qt_core_builtin\\Headers" FILES ${TROLTECH_CORE_HEADERS} )

target_sources( ${PRODUCT_PYTHONQT}
	PUBLIC ${TROLTECH_CORE_SOURCES} 
	PUBLIC ${TROLTECH_CORE_HEADERS}
)
	
