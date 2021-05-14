cmake_minimum_required(VERSION 3.1.0)

set (TROLTECH_GUI_SOURCES
	${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_gui_builtin0.cpp
    ${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_gui_builtin_init.cpp
)

set (TROLTECH_GUI_HEADERS
	${CMAKE_CURRENT_LIST_DIR}/com_trolltech_qt_gui_builtin0.h
)

source_group("com_trolltech_qt_gui_builtin\\Sources" FILES ${TROLTECH_GUI_SOURCES} )
source_group("com_trolltech_qt_gui_builtin\\Headers" FILES ${TROLTECH_GUI_HEADERS} )

target_sources( ${PRODUCT_PYTHONQT}
	PUBLIC ${TROLTECH_GUI_SOURCES} 
	PUBLIC ${TROLTECH_GUI_HEADERS}
)