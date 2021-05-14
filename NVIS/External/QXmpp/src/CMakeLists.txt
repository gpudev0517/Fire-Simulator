cmake_minimum_required(VERSION 3.1.0)


set (PRODUCT_QXMPP "qxmpp" CACHE STRING "")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


include(../qxmpp.cmake)
include(external_lib_config)

find_package(Qt5Core REQUIRED)

add_definitions( -DQXMPP_BUILD -DQXMPP_INTERNAL_DEFINES )

include_directories( ${QXMPP_INCLUDEPATH} ${QXMPP_INTERNAL_INCLUDES} )
link_libraries ( ${QXMPP_INTERNAL_LIBS} )

include(base/base.cmake)
include(client/client.cmake)
include(server/server.cmake)


add_library ( ${PRODUCT_QXMPP} SHARED
	${QXMPP_BASE_HEADERS}
	${QXMPP_BASE_SOURCES}
	${QXMPP_CLIENT_HEADERS}
    ${QXMPP_CLIENT_SOURCES}
	${QXMPP_SERVER_HEADERS}
    ${QXMPP_SERVER_SOURCES}
)

SET_TARGET_PROPERTIES(${PRODUCT_QXMPP} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

set_target_properties(${PRODUCT_QXMPP} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

qt5_use_modules(${PRODUCT_QXMPP} Core Xml Network)



# HEADERS += $$INSTALL_HEADERS

# # Installation
# headers.files = $$INSTALL_HEADERS
# headers.path = $$PREFIX/include/qxmpp
# target.path = $$PREFIX/$$LIBDIR
# INSTALLS += headers target


# # pkg-config support
# CONFIG += create_pc create_prl no_install_prl
# QMAKE_PKGCONFIG_DESTDIR = pkgconfig
# QMAKE_PKGCONFIG_LIBDIR = $$target.path
# QMAKE_PKGCONFIG_INCDIR = $$headers.path
# equals(QXMPP_LIBRARY_TYPE,staticlib) {
    # QMAKE_PKGCONFIG_CFLAGS = -DQXMPP_STATIC
# } else {
    # QMAKE_PKGCONFIG_CFLAGS = -DQXMPP_SHARED
# }
# unix:QMAKE_CLEAN += -r pkgconfig lib$${TARGET}.prl