QT       += core gui opengl xml network

CONFIG += plugin c++11

TEMPLATE = lib

PLUGINEXT = .so
win32:PLUGINEXT = .dll
macx:PLUGINEXT = .dylib


# Copies the given plugin to the destination directory at post-link
defineTest(samplePluginCopyDir) {
    DPATH = $$1
    NELIBPATH = $$2
    PPATH = $${OUT_PWD}
    CONFIG(release, debug|release) {
        QMAKE_LIBDIR += $${PWD}/../../External/lib/release
        DPATH = $${PWD}/../../../Data/Plugins/release/$$DPATH
	PPATH = $${PPATH}/release/
    } else {
        QMAKE_LIBDIR += $${PWD}/../../External/lib/debug
        DPATH = $${PWD}/../../../Data/Plugins/debug/$$DPATH
	PPATH = $${PPATH}/debug/
    }
    PPATH = $${PPATH}$${TARGET}$${PLUGINEXT}
    # Replace slashes in paths with backslashes for Windows
    win32:DPATH ~= s,/,\\,g
    win32:PPATH ~= s,/,\\,g

    QMAKE_POST_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DPATH) '"'$${DPATH}'"' $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$QMAKE_COPY '"'$${PPATH}'"' '"'$${DPATH}'"' $$escape_expand(\\n\\t)

    QMAKE_LIBDIR += $${NELIBPATH}

    export(QMAKE_POST_LINK)
    message($$QMAKE_LIBDIR)
    export(QMAKE_LIBDIR)
}



unix:!macx {
INCLUDEPATH +=\
    ../../Include \
    ../../External \
    ../../External/include \
    /usr/include/freetype2 \
    /usr/include/python2.7 \
    /usr/include/bullet \
    ../../
}

win32 {
INCLUDEPATH +=\
    ./ \
    ../../Include \
    ../../Include/Core \
    ../../External \
    ../../External/include \
    ../../External/platform/include/ \
    ../../External/platform/include/freetype2 \
    ../../External/platform/include/python27 \
    ../../External/platform/include/bullet \
    ../../
}

LIBS += -lneutrino

unix {
LIBS += -lGLEW
}

win32 {
LIBS += -lOpenGL32
LIBS += -lglew32
}




