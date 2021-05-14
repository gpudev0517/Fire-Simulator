# Builds the NEGeometry module

include(../QMake/common.pri)
include($$ROOTPATH/QMake/utility.pri)
include(install.pri)

TARGET = NEGeometry
TEMPLATE = lib
QT  +=  core
CONFIG += precompile_header shared
DEFINES += NEUTRINO_GEOMETRY_BUILD NOMINMAX
PRECOMPILED_HEADER = PCH_Geometry.h


LIBS += -lNECore -lOpenVDB -lHalf -ltbb


# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}


# Common Includes #

INCLUDEPATH += \
        $$NDYNPATH/include/Core \
        $$ROOTPATH \
	$$ROOTPATH/External \
	$$ROOTPATH/External/OpenVDB

# Platform Specific Configurations #

win32 {
	INCLUDEPATH +=\
                $$ROOTPATH/External/Windows/include
}

unix:!macx {
    INCLUDEPATH += \
                $$ROOTPATH/External/Linux/include
}


SOURCES += \
        Manager/NEMeshVolGeometryManager.cpp \
        Mesh/NESpatialMesh.cpp \
        Mesh/NESpatialEngine.cpp \
        Mesh/NEScenario.cpp \
        Mesh/NEZone.cpp \
        Mesh/NEZoneCompartment.cpp \
        Mesh/NEZoneItem.cpp \
        Mesh/NEScenarioItem.cpp \
        Mesh/NEMeshConstructor.cpp \
        Resource/Mesh/NEMeshContainer.cpp \
        Utility/NECableProperty.cpp \
        Utility/NECustomVariants.cpp \
        Utility/NESurfaceMaterial.cpp \
        Utility/NEFireMaterial.cpp \
        Utility/NEFailureCriteria.cpp \
        Utility/NEVentCriteria.cpp


HEADERS += \
        Manager/NEMeshVolGeometryManager.h \
        Mesh/NESpatialMesh.h \
        Mesh/NESpatialEngine.h \
        Mesh/NEZone.h \
        Mesh/NEZoneCompartment.h \
        Mesh/NEScenario.h \
        Mesh/NEZoneItem.h \
        Mesh/NEScenarioItem.h \
        Mesh/NEMeshConstructor.h \
        Resource/Mesh/NEMeshContainer.h \
        Utility/NECableProperty.h \
        Utility/NECustomVariants.h \
        Utility/NESurfaceMaterial.h \
        Utility/NEFireMaterial.h \
        Utility/NEFailureCriteria.h \
        Utility/NEVentCriteria.h


if($$USE_VOLUME) {
	VOLPATH = $$ROOTPATH/Code/Geometry/Volume
	INCLUDEPATH += $$VOLPATH

	SOURCES += \
		$$VOLPATH/Core/NEVolume.cpp \
		$$VOLPATH/Core/NEVolumeNode.cpp \


	HEADERS += \
		$$VOLPATH/Core/NEVolume.h \
		$$VOLPATH/Core/NEVolumeNode.h \

}


#message(LIBS(NEGeometry): $$LIBS)
#message(INCLUDE(NEGeometry): $$INCLUDEPATH)
