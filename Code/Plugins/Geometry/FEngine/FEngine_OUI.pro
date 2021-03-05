# Builds the Fire Engine DB Module in FDYN

include(../../../QMake/common.pri)
include($$ROOTPATH/QMake/plugin_config_OUI.pri)

TARGET   = FEngine
QT      += core
DEFINES += FDYN_PLUGINS_BUILD FENGINE_LIBRARY
CONFIG  += precompile_header
PRECOMPILED_HEADER = PCH_FEngine.h

NDYNPATH = $$ROOTPATH/NDYN
INCLUDEPATH += $${NDYNPATH}/include/Core
INCLUDEPATH += $${NDYNPATH}/include/Geometry
INCLUDEPATH += $${NDYNPATH}/include/Render

INCLUDEPATH += $${ROOTPATH}/Code/Plugins
INCLUDEPATH += $${ROOTPATH}/External/include


QMAKE_LIBDIR += $$OUT_PWD
QMAKE_LIBDIR += $$NDYNPATH/lib

LIBS += -lNECore -lNEGeometry -lNERender 

HEADERS += \
        FFactory.h \
        FEngine.h \
        FEngineRenderer.h \
        FZone.h \
        FZoneRenderer.h \
        FZoneItem.h \
        FZoneItemRenderer.h \
        FScenario.h \
        $${ROOTPATH}/Code/Plugins/globals_Plugin.h


SOURCES += \
        FFactory.cpp \
        FEngine.cpp \
        FEngineRenderer.cpp \
        FZone.cpp \
        FZoneRenderer.cpp \
        FZoneItem.cpp \
        FZoneItemRenderer.cpp \
        FScenario.cpp


pluginCopyDir(Spatial/Geometry, $$ROOTPATH)

# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1
    prefixdir = $$2


    for(FILE, files) {
        DDIR = $$DESTDIR

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        destsubdirpath = $$relative_path($$FILE, $$prefixdir)

        destfile =  $$quote($$DDIR)/$$destsubdirpath
        destfile = $$dirname(destfile)
        win32: destfile ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$FILE) $$destfile $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

win32: SDIR = $$PWD/lib
DDIR = $$DESTDIR
win32:DDIR ~= s,/,\\,g
win32:SDIR ~= s,/,\\,g


QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$SDIR $$DDIR $$escape_expand(\\n\\t)
export(QMAKE_POST_LINK)

