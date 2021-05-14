SOURCES += \
        $$VOLPATH/Core/NEVolume.cpp \
        $$VOLPATH/Core/NEVolumeFactory.cpp \
        $$VOLPATH/Core/NEVolumeNode.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeKiller.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEMeshToVolume.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEPartToVolume.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEPartToMesh.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeCombine.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeFile.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeFilter.cpp \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeToMesh.cpp \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB.cpp \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_HashGrid.cpp \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_SDF.cpp \
        $$VOLPATH/Physics/Spatial/Volume/NEPartVolumeTools.cpp

HEADERS += \
        $$VOLPATH/Core/NEVolume.h \
        $$VOLPATH/Core/NEVolumeNode.h \
        $$VOLPATH/Core/NEVolumeFactory.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeKiller.h \
	$$VOLPATH/Physics/Spatial/Volume/NEMeshToVolume.h \
	$$VOLPATH/Physics/Spatial/Volume/NEPartToVolume.h \
	$$VOLPATH/Physics/Spatial/Volume/NEPartToMesh.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeCombine.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeFile.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeFilter.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeToMesh.h \
	$$VOLPATH/Physics/Spatial/Volume/NEVolumeUtils.h \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/kdtree/kdtree.h \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB.h \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_HashGrid.h \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/ParticleVDB_SDF.h \
	$$VOLPATH/Physics/Spatial/Volume/ParticleVolume/svdcmp.c \
        $$VOLPATH/Physics/Spatial/Volume/NEPartVolumeTools.h
