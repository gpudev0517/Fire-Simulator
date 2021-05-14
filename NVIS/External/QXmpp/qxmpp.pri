# Common definitions

QT += network xml
QXMPP_VERSION = 0.9.3
QXMPP_INCLUDEPATH = $$PWD/src/base $$PWD/src/client $$PWD/src/server

# Determine library name
CONFIG(debug, debug|release) {
    QXMPP_LIBRARY_NAME = qxmpp
} else {
    QXMPP_LIBRARY_NAME = qxmpp
}

# Determine library type (shared or staticlib)

QXMPP_LIBRARY_TYPE = shared

# Libraries used internally by QXmpp
win32 {
    QXMPP_INTERNAL_LIBS = -ldnsapi -lws2_32
}



# Libraries for apps which use QXmpp
QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}
contains(QXMPP_LIBRARY_TYPE,staticlib) {
    # Symbian needs a .lib extension to recognise the library as static
    symbian: QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}.lib

    # FIXME: we should be able to use the link_prl option to automatically pull
    # in the extra libraries which the qxmpp library needs, but this does not
    # seem to work on win32, so we specify the dependencies here:
    QXMPP_LIBS += $$QXMPP_INTERNAL_LIBS
    DEFINES += QXMPP_STATIC
} else {
    # Windows needs the major library version
    win32: QXMPP_LIBS = -l$${QXMPP_LIBRARY_NAME}0
    DEFINES += QXMPP_SHARED
}

# Installation prefix and library directory
isEmpty(PREFIX) {
    contains(MEEGO_EDITION,harmattan) {
        PREFIX = /usr
    } else:unix {
        PREFIX = /usr/local
    } else {
        PREFIX = $$[QT_INSTALL_PREFIX]
    }
}
isEmpty(LIBDIR) {
    LIBDIR = lib
}

# Internal API auto-tests
!isEmpty(QXMPP_AUTOTEST_INTERNAL) {
    DEFINES += QXMPP_AUTOTEST_INTERNAL
}
