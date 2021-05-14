

TARGET = NESampleSimpleSolver
DEFINES += SAMPLESPHSOLVER_LIBRARY
CONFIG += precompile_header
PRECOMPILED_HEADER = $${PWD}/Precomp.h
NE_BINARY_PATH = $${PWD}/../../../release

include(../sample_plugin_config.pri)

samplePluginCopyDir(Spatial/Physical/SphFluid,$${NE_BINARY_PATH})

SOURCES += NESampleSimpleSolver.cpp

HEADERS += NESampleSimpleSolver.h\
    NESampleSphParticle.h \
    ../../Include/Core/NENode.h \
    ../../Include/Core/NEObject.h \
    ../../Include/Core/NESpatial.h \
    ../../Include/Spatial/Physical/Fluid/NEParticleFluid.h \
    ../../Include/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h \
    ../../Include/Spatial/Physical/NEFluid.h \
    ../../Include/Spatial/NEPhysical.h \
    Precomp.h

