include(config.pri)

DATAPATH=$$ROOTPATH/../../Data
INSTALL_PREFIX=$$DATAPATH/Include

INSTALL_CODE_ROOT = $$ROOTPATH/Code
#message(INSTALLROOT)
#message($$INSTALL_CODE_ROOT)

INSTALL_COMMON_HEADERS = \
    $$INSTALL_CODE_ROOT/Core/Utility/globals.h \
    $$INSTALL_CODE_ROOT/Core/Utility/Singleton.h \
    $$INSTALL_CODE_ROOT/Core/Utility/mathUtil.h \
    $$INSTALL_CODE_ROOT/Core/Utility/Math/includeMath.h \
    $$INSTALL_CODE_ROOT/Core/Utility/NEAABB.h \
    $$INSTALL_CODE_ROOT/Core/Utility/NEAABB.inl \
    $$INSTALL_CODE_ROOT/Core/Utility/Math \
    $$INSTALL_CODE_ROOT/Core/Utility/NEFrameInfo.h \
    $$INSTALL_CODE_ROOT/Core/Utility/NEOsd.h \
    $$INSTALL_CODE_ROOT/Core/Utility/NETimer.h \
    $$INSTALL_CODE_ROOT/Core/Utility/NEMocDefs.h

for(header, INSTALL_COMMON_HEADERS) {
  path = $$INSTALL_PREFIX/$$relative_path($$header, $$ROOTPATH)
  filename = $$relative_path($$header)
  eval(headers_$${filename}.files += $$header)
  eval(headers_$${filename}.path = $$path)
  eval(INSTALLS *= headers_$${filename})
}

INSTALL_CODE_HEADERS = \
    $$INSTALL_CODE_ROOT/Core/Base/NEApplication.h \
    $$INSTALL_CODE_ROOT/Core/Base/NEBase.h \
    $$INSTALL_CODE_ROOT/Core/Manager/NEManager.h \
    $$INSTALL_CODE_ROOT/Core/Collider/NECollider.h \
    $$INSTALL_CODE_ROOT/Core/NENode.h \
    $$INSTALL_CODE_ROOT/Core/NERenderable.h \
    $$INSTALL_CODE_ROOT/Core/Base/NENodeFactory.h \
    $$INSTALL_CODE_ROOT/Core/NEObject.h \
    $$INSTALL_CODE_ROOT/Core/NESpatial.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/NEPhysical.h \
    $$INSTALL_CODE_ROOT/Core/Base/NESceneManager.h \
    $$INSTALL_CODE_ROOT/Core/Manager/NECollisionManager.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/NERigid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/NEParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/NEPhysicalParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/NEParticleRigid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidSphParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NESphRigid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NEParticleFluid.inl \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/NEParticleFluid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/ParticleFluid/SphLiquid/NESphLiquidParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NERigidPciSphParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NEPciSphRigid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Rigid/ParticleRigid/NESimpleSphRigid.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NESphLiquidSolver.inl \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/Fluid/ParticleFluid/NEFluidParticle.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Source/NEParticleSource.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/Physical/NEFluid.h \
    $$INSTALL_CODE_ROOT/Core/Resource/Mesh/NEIndexedTriMesh.h \
    $$INSTALL_CODE_ROOT/Core/Resource/NEMesh.h \
    $$INSTALL_CODE_ROOT/Core/NEResource.h \
    $$INSTALL_CODE_ROOT/Core/Utility/geometry.h \
    $$INSTALL_CODE_ROOT/Core/Physics/Spatial/NEPhysical.h



for(header, INSTALL_CODE_HEADERS) {
  H = $$header
  path = $$INSTALL_PREFIX/$$relative_path($$H, $$ROOTPATH)
  filename = $$relative_path($$header)
  eval(headers_$${filename}.files += $$header)
  eval(headers_$${filename}.path = $$path)
  eval(INSTALLS *= headers_$${filename})
}




INSTALL_EXTERNAL_PREFIX = $$DATAPATH
INSTALL_EXTERNAL = \
    External/include/3DeLight


for(scenes, INSTALL_EXTERNAL) {
  path = $${INSTALL_EXTERNAL_PREFIX}/$$relative_path($$scenes, $$ROOTPATH/include)
  filename = $$relative_path($$scenes, $$ROOTPATH)
  eval(scenes_$${filename}.files += $$scenes)
  eval(scenes_$${filename}.path = $$path)
  eval(INSTALLS *= scenes_$${filename})
}

INSTALL_EXTERNAL_INCLUDE_PREFIX = $$DATAPATH/External/include
ROOT_EXTERNAL_PATH = $$ROOTPATH/External

unix:!macx {
	PLATFORM = Linux
}

win32 {
	PLATFORM = Windows
}

macx {
	PLATFORM = Mac
}

INSTALL_EXTERNAL_INCLUDES = \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/boost \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/Bullet \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/VTK \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/OpenVDB \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/OpenEXR \
	$$ROOT_EXTERNAL_PATH/$$PLATFORM/include/GL


for(scenes, INSTALL_EXTERNAL_INCLUDES) {
  path = $${INSTALL_EXTERNAL_INCLUDE_PREFIX}/$$relative_path($$scenes, $$ROOT_EXTERNAL_PATH/$$PLATFORM/include)
  filename = $$relative_path($$scenes, $$ROOTPATH)
  eval(scenes_$${filename}.files += $$scenes)
  eval(scenes_$${filename}.path = $$path)
  eval(INSTALLS *= scenes_$${filename})
}

CONFIG(release, debug|release) {
    INSTALL_BINARY_PREFIX = $$DATAPATH/Bin/Release
    INSTALL_EXT_PREFIX = Build/Release
}

CONFIG(debug, debug|release) {
    INSTALL_BINARY_PREFIX = $$DATAPATH/Bin/Debug
    INSTALL_EXT_PREFIX = Build/Debug
}



#
# Library Ext
#
DLIBEXT = so
win32:DLIBEXT = dll
macx:DLIBEXT = so

SLIBEXT = a
win32:SLIBEXT = lib
macx:SLIBEXT = a

INSTALL_BINARY_FILES = \
    $${DESTDIR}/NE.$$DLIBEXT \
    $${DESTDIR}/NE.$$SLIBEXT \
    $${DESTDIR}/NECore.$$DLIBEXT \
    $${DESTDIR}/NECore.$$SLIBEXT

for(scenes, INSTALL_BINARY_FILES) {
  path = $${INSTALL_BINARY_PREFIX}/$$basename($$scenes)
  filename = $$relative_path($$scenes, $$ROOTPATH)

  eval(scenes_$${filename}.files += $$scenes)
  eval(scenes_$${filename}.path = $$path)
  eval(INSTALLS *= scenes_$${filename})
}



#
# Following might not be needed during installation
#

#CONFIG(release, debug|release) {
#    INSTALL_EXTERNAL_LIB_PREFIX = $$DATAPATH/Bin/Release
#}

#CONFIG(debug, debug|release) {
#    INSTALL_EXTERNAL_LIB_PREFIX = $$DATAPATH/Bin/Debug
#}

#INSTALL_EXTERNAL_LIB = \
#    $$files($$ROOT_EXTERNAL_PATH/$$PLATFORM/lib/*, true)


#for(scenes, INSTALL_EXTERNAL_LIB) {
#    path = $${INSTALL_EXTERNAL_LIB_PREFIX}/$$basename($$scenes)
#    filename = $$relative_path($$scenes, $$ROOTPATH)
#    eval(scenes_$${filename}.files += $$scenes)
#    eval(scenes_$${filename}.path = $$path)
#    eval(INSTALLS *= scenes_$${filename})
#}


#CONFIG(release, debug|release) {
#    INSTALL_LIB_PREFIX = $$DATAPATH/Bin/Release
#    INSTALL_LIB = $$files($ROOT_EXTERNAL_PATH/Build/Release/*, true)
#}

#CONFIG(debug, debug|release) {
#    INSTALL_LIB_PREFIX = $$DATAPATH/Bin/Debug
#    INSTALL_LIB = $$ROOT_EXTERNAL_PATH/Build/Debug/*
#}

#for(scenes, INSTALL_LIB) {
#	path = $${INSTALL_LIB_PREFIX}/$$basename($$scenes)
#	filename = $$relative_path($$scenes,$$ROOTPATH)
#	eval(scenes_$${filename}.files += $$scenes)
#	eval(scenes_$${filename}.path = $$path)
#	eval(INSTALLS *= scenes_$${filename})
#}

#
# Sample Plugins
#

#INSTALL_SAMPLES_PREFIX = ../../Data/Plugins/
#INSTALL_SAMPLES = \
#    Setups/Samples/Plugins

#for(samples, INSTALL_SAMPLES) {
#  path = $${INSTALL_SAMPLES_PREFIX}/$${dirname($samples)}
#  pathsamples = $$replace(path, "Plugins", "")
#  eval(samples_$${path}.files += $$samples)
#  eval(samples_$${path}.path = $$pathsamples)
#  eval(INSTALLS *= samples_$${path})
#}


#INSTALL_SCENES_PREFIX = ../../Data/NEScenes/
#INSTALL_SCENES = \
#    Setups/Samples/NEScenes

#for(scenes, INSTALL_SCENES) {
#  path = $${INSTALL_SCENES_PREFIX}
#  pathscenes = $$replace(path, "NEScenes", "")
#  eval(scenes_$${path}.files += $$scenes)
#  eval(scenes_$${path}.path = $$pathscenes)
#  eval(INSTALLS *= scenes_$${path})
#}
