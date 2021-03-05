# Builds the NENetCmdSimData module

include(../../../../Code/QMake/common.pri)
include($$ROOTPATH/QMake/plugin_config.pri)

TARGET  = FCFAST
QT      += core xml network
DEFINES += FDYN_PLUGINS_BUILD NCLOUDDATA_LIBRARY
CONFIG  += precompile_header
PRECOMPILED_HEADER = PCH_FCFAST.h

INCLUDEPATH +=  $${ROOTPATH}/Code/Plugins


INCLUDEPATH += $${NDYNPATH}/include
INCLUDEPATH += $${NDYNPATH}/include/Core
INCLUDEPATH += $${NDYNPATH}/include/Geometry
INCLUDEPATH += $${NDYNPATH}/include/Physics
INCLUDEPATH += $${NDYNPATH}/include/Render

QMAKE_LIBDIR += $$OUT_PWD
QMAKE_LIBDIR += $$NDYNPATH/lib
LIBS += -lNECore -lNEPhysics -lNEGeometry

SOURCES += FCFAST.cpp

HEADERS += FCFAST.h \
           PCH_FCFAST.h

pluginCopyDir(Spatial/DataCollector, $$ROOTPATH)
