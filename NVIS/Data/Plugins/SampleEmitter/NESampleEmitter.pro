#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T00:34:24
#
#-------------------------------------------------

TARGET = SampleEmitter
DEFINES += SAMPLEEMITTER_LIBRARY
CONFIG += precompile_header
PRECOMPILED_HEADER = $${PWD}/Precomp.h

include(../sample_plugin_config.pri)

samplePluginCopyDir(Spatial/ParticleSource)

SOURCES += NESampleEmitter.cpp

HEADERS += NESampleEmitter.h\
    ../../Code/Core/NENode.h \
    ../../Code/Core/NEObject.h \
    ../../Code/Core/NESpatial.h \
    ../../Code/Spatial/NESource.h \
    ../../Code/Spatial/NEPhysical.h \
    ../../Code/Spatial/Source/NEParticleSource.h \
    Precomp.h

