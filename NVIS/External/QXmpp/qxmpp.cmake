# Common definitions

find_library(Qt5Network REQUIRED)
find_library(Qt5Xml REQUIRED)

 
set ( QXMPP_VERSION "0.9.3" )
set ( QXMPP_INCLUDEPATH ${CMAKE_CURRENT_LIST_DIR}/src/base ${CMAKE_CURRENT_LIST_DIR}/src/client ${CMAKE_CURRENT_LIST_DIR}/src/server )

set ( PRODUCT_QXMPP "qxmpp" )
set ( QXMPP_LIBRARY_NAME ${PRODUCT_QXMPP})

# Determine library type (shared or staticlib)

set (QXMPP_LIBRARY_TYPE shared )

# Libraries used internally by QXmpp
if( WIN32 AND MSVC )

    set (QXMPP_INTERNAL_LIBS dnsapi.lib ws2_32.lib )
endif()

# Libraries for apps which use QXmpp
set (QXMPP_LIBS ${QXMPP_LIBRARY_NAME} )
if ( ${QXMPP_LIBRARY_TYPE} STREQUAL staticlib) 
    # Symbian needs a .lib extension to recognise the library as static

    # symbian: QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}.lib

    # FIXME: we should be able to use the link_prl option to automatically pull
    # in the extra libraries which the qxmpp library needs, but this does not
    # seem to work on win32, so we specify the dependencies here:
    list ( APPEND QXMPP_LIBS ${QXMPP_INTERNAL_LIBS} )
	add_definitions ( -DQXMPP_STATIC )
else()
    # Windows needs the major library version
	
    if( WIN32 AND MSVC)
	set (QXMPP_LIBS ${QXMPP_LIBRARY_NAME}0 )
	endif()
	add_definitions ( -DQXMPP_SHARED )
endif()

# # Installation prefix and library directory
# isEmpty(PREFIX) {
    # contains(MEEGO_EDITION,harmattan) {
        # PREFIX = /usr
    # } else:unix {
        # PREFIX = /usr/local
    # } else {
        # PREFIX = $$[QT_INSTALL_PREFIX]
    # }
# }
# isEmpty(LIBDIR) {
    # LIBDIR = lib
# }

# # Internal API auto-tests
# !isEmpty(QXMPP_AUTOTEST_INTERNAL) {
    # DEFINES += QXMPP_AUTOTEST_INTERNAL
# }
