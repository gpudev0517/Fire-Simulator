
CONFIG += plugin c++11
TEMPLATE = lib

PLUGINEXT = .so
win32:PLUGINEXT = .dll
macx:PLUGINEXT = .dylib


if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}

# Copies the given plugin to the destination directory at post-link
defineTest(pluginCopyDir) {
    DPATH = $$1
    SPATH = $$2
    PPATH = $${DESTDIR}
    CONFIG(release, debug|release) {
        QMAKE_LIBDIR += ../../Build/Release
        DPATH = $$SPATH/Data/Plugins/Release/$$DPATH
    } else {
        QMAKE_LIBDIR += ../../Build/Debug
        DPATH = $$SPATH/Data/Plugins/Debug/$$DPATH
    }
    unix {
        PPATH = $${PPATH}/lib$${TARGET}$${PLUGINEXT}
    }
    win32 {
        PPATH = $${PPATH}/$${TARGET}$${PLUGINEXT}
    }
    # Replace slashes in paths with backslashes for Windows
    win32:DPATH ~= s,/,\\,g
    win32:PPATH ~= s,/,\\,g

    QMAKE_POST_LINK += $$sprintf($$QMAKE_MKDIR_CMD, $$DPATH) '"'$${DPATH}'"' $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$QMAKE_COPY '"'$${PPATH}'"' '"'$${DPATH}'"' $$escape_expand(\\n\\t)

    export(QMAKE_POST_LINK)
    export(QMAKE_LIBDIR)
}



unix:!macx {
INCLUDEPATH +=\
    $${ROOTPATH}/Code \
    $${ROOTPATH}/External \
    $${ROOTPATH}/External/Linux/include \
    ../../
}

unix:macx {
INCLUDEPATH +=\
    $${ROOTPATH}/Code \
    $${ROOTPATH}/External \
    $${ROOTPATH}/External/Mac/include \
    ../../
}

win32 {
INCLUDEPATH +=\
    ./ \
    $${ROOTPATH}/Code \
    $${ROOTPATH}/Code/Core \
    $${ROOTPATH}/External \
    $${ROOTPATH}/External/include \
    $${ROOTPATH}/External/Windows/include/ \
    ../../
}
