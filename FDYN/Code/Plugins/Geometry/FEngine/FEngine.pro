# Builds the Fire Engine DB Module in FDYN

include(../../../QMake/common.pri)
include($$ROOTPATH/QMake/plugin_config.pri)

TARGET   = FEngine
QT      += core
QT      += opengl
DEFINES += FDYN_PLUGINS_BUILD FENGINE_LIBRARY
CONFIG  += precompile_header
PRECOMPILED_HEADER = PCH_FEngine.h


INCLUDEPATH += $${NDYNPATH}/include
INCLUDEPATH += $${NDYNPATH}/include/Core
INCLUDEPATH += $${NDYNPATH}/include/Geometry
INCLUDEPATH += $${NDYNPATH}/include/Physics
INCLUDEPATH += $${NDYNPATH}/include/Render
INCLUDEPATH += $${NDYNPATH}/External/include
INCLUDEPATH += $${NDYNPATH}/External/include/python27
INCLUDEPATH += $${NDYNPATH}/External/include/PythonQt/src

INCLUDEPATH += $${ROOTPATH}/Code/Plugins
INCLUDEPATH += $${ROOTPATH}/External/include


QMAKE_LIBDIR += $$OUT_PWD
QMAKE_LIBDIR += $$NDYNPATH/lib

LIBS += -lNECore -lNEGeometry -lNEPhysics -lNERender

win32 {
    QMAKE_LIBDIR += $$PWD/lib



    if ($$RELEASE) {
        LIBS += -lPythonQt
    }
    else {
        LIBS += -lPythonQt_d
    }
}

HEADERS += \
        FFactory.h \
        FEngine.h \
        FEngineRenderer.h \
        FZone.h \
        FZoneCompartment.h \
        FZoneRenderer.h \
        FZoneCompartmentRenderer.h \
        FZoneItem.h \
        FCables.h \
        FZoneItemRenderer.h \
        FFireRenderer.h \
        FScenario.h \
        FFireMaterial.h \
        FFire.h \
        FVent.h \
        FVentRenderer.h \
        FMechVent.h \
        FMechVentRenderer.h \
        FCablesRenderer.h \
        FCFAST.h \
        FCFASTPlane.h \
        FCFASTRenderer.h \
        FFRANX.h \
        FFRANXProperties.h \
        FFireDetect.h \
        FFireDetectRenderer.h \
        $${ROOTPATH}/Code/Plugins/globals_Plugin.h \
        FEngineWorker.h


SOURCES += \
        FFactory.cpp \
        FEngine.cpp \
        FEngineRenderer.cpp \
        FZone.cpp \
        FZoneCompartment.cpp \
        FZoneRenderer.cpp \
        FZoneCompartmentRenderer.cpp \
        FZoneItem.cpp \
        FCables.cpp \
        FCablesRenderer.cpp \
        FZoneItemRenderer.cpp \
        FFireRenderer.cpp \
        FScenario.cpp \
        FFireMaterial.cpp \
        FFire.cpp \
        FVent.cpp \
        FVentRenderer.cpp \
        FMechVentRenderer.cpp \
        FMechVent.cpp \
        FCFAST.cpp \
        FCFASTPlane.cpp \
        FCFASTRenderer.cpp \
        FFRANX.cpp \
        FFireDetect.cpp \
        FFireDetectRenderer.cpp



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

