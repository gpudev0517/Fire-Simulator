# Builds the NERendering module

include(../QMake/common.pri)
include($$ROOTPATH/QMake/utility.pri)
#include($$ROOTPATH/QMake/install.pri)

TARGET   = NERenderNEO
TEMPLATE = lib
QT += core
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NERENDERNEO_BUILD
PRECOMPILED_HEADER = PCH_RenderNEO.h

RENDERNEOPATH = $$ROOTPATH/Code/RenderNEO


# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}


# Common Includes #

INCLUDEPATH += \
	$$ROOTPATH \
	$$ROOTPATH/External \
        $$ROOTPATH/External/include


INCLUDEPATH += \
        $$NDYNPATH/include \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/include/PostProcessor \
        $$NDYNPATH/include/Physics \
        $$ROOTPATH/Code \
        $$ROOTPATH/Code/Geometry \
        $$ROOTPATH/Code/Render

win32 {
        INCLUDEPATH += \
                $$ROOTPATH/External/Windows/include \
                $$ROOTPATH/External/Windows/include/FFMpeg \
        INCLUDEPATH += $$ROOTPATH/External/Windows/include/Freetype2
}


macx {
        INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
}

unix:!macx {
        INCLUDEPATH += $$ROOTPATH/External/Linux/include \
                       $$ROOTPATH/External/Linux/include/FFMpeg \
                       $$ROOTPATH/External/Linux/include/freetype2 \
                       /usr/include/freetype2
}





LIBS += -lNECore -lNEGeometry -lNERender -lNEPhysics  -lNEPostProcessor

if($$CUDA_INSTALLED) {
	DEFINES += CUDA_AVAILABLE
#	LIBS += -lGVDB
}

LIBS += -lftgl

win32 {
        VTK_VERSION=7.1
        INCLUDEPATH += $$ROOTPATH/External/Windows/include/VTK/
        INCLUDEPATH += $$ROOTPATH/External/Windows/include/Freetype2
}

macx {
        VTK_VERSION=7.1
        INCLUDEPATH += $$ROOTPATH/External/macosx/include/vtk-$$VTK_VERSION
        INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
}

unix:!macx {
        VTK_VERSION=7.1
        INCLUDEPATH += $$ROOTPATH/External/Linux/include/vtk-$$VTK_VERSION
        INCLUDEPATH += $$ROOTPATH/External/Linux/include/freetype2
}

if($$USE_VTK) {
        LIBS += -lvtkCommonTransforms-$$VTK_VERSION
        LIBS += -lvtkIOXML-$$VTK_VERSION
        LIBS += -lvtkFiltersModeling-$$VTK_VERSION
        LIBS += -lvtkFiltersGeneral-$$VTK_VERSION
        LIBS += -lvtkFiltersSources-$$VTK_VERSION
        LIBS += -lvtkFiltersCore-$$VTK_VERSION
        LIBS += -lvtkCommonColor-$$VTK_VERSION
        LIBS += -lvtkCommonExecutionModel-$$VTK_VERSION
        LIBS += -lvtkCommonDataModel-$$VTK_VERSION
        LIBS += -lvtkCommonMisc-$$VTK_VERSION
        LIBS += -lvtkCommonCore-$$VTK_VERSION
        LIBS += -lvtkCommonMath-$$VTK_VERSION
        LIBS += -lvtkCommonSystem-$$VTK_VERSION
        LIBS += -lvtksys-$$VTK_VERSION
        LIBS += -lvtkIOCore-$$VTK_VERSION
        LIBS += -lvtkIOXMLParser-$$VTK_VERSION
        LIBS += -lvtkzlib-$$VTK_VERSION
        LIBS += -lvtkexpat-$$VTK_VERSION
}


SOURCES += \
        Rendering/Manager/NERenderNEOManager.cpp \
        Rendering/Spatial/Renderers/NEParticleRigidRenderer.cpp \
        Rendering/Spatial/Renderers/NEParticleFluidSolverRenderer.cpp \
        Rendering/Spatial/Renderers/svdcmp.c \
        Rendering/Resource/Material/NEParticleFluidMaterial.cpp \
        Rendering/Spatial/Renderers/NERigidTopographyRenderer.cpp \
        Rendering/Spatial/Renderers/NEMeasurementFieldRenderer.cpp \
        Rendering/Spatial/Renderers/NEMeshPostProcessorRenderer.cpp \
        Rendering/Spatial/Renderers/NEMCRenderer.cpp \
        Rendering/Spatial/Renderers/NEParticleInputMCRenderer.cpp \
        Rendering/Spatial/NEOSDMeasurement.cpp \
        Rendering/Spatial/NEOSDParticleFluid.cpp \
        Rendering/Spatial/NEOSDParticleRigid.cpp \
        Rendering/Spatial/NEOSDRigidTopography.cpp \
        Rendering/Utility/NEOsd.cpp

	

HEADERS  += \
        globals_RenderNEO.h \
        Rendering/Manager/NERenderNEOManager.h \
        Rendering/Spatial/Renderers/NEParticleRigidRenderer.h \
        Rendering/Spatial/Renderers/NEParticleFluidSolverRenderer.h \
        Rendering/Resource/Material/NEParticleFluidMaterial.h \
        Rendering/Spatial/Renderers/NERigidTopographyRenderer.h \
        Rendering/Spatial/Renderers/NEMeasurementFieldRenderer.h \
        Rendering/Spatial/Renderers/NEMeshPostProcessorRenderer.h \
        Rendering/Spatial/Renderers/NEMCRenderer.h \
        Rendering/Spatial/Renderers/NEParticleInputMCRenderer.h \
        Rendering/Spatial/NEOSDMeasurement.h \
        Rendering/Spatial/NEOSDParticleFluid.h \
        Rendering/Spatial/NEOSDParticleRigid.h \
        Rendering/Spatial/NEOSDRigidTopography.h \
        Rendering/Utility/NEOsd.h \
        PCH_RenderNEO.h


if($$USE_GRIDFLUID) {
	SOURCES += \
		Rendering/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridRenderer.cpp
	HEADERS += \
		Rendering/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridRenderer.h
}



if($$USE_COUPLING) {
		SOURCES += \	
			Rendering/Spatial/Renderers/NECouplerRenderer.cpp \
			#Rendering/Spatial/NEHeightFieldSourceRenderer.cpp \
			Rendering/Spatial/Renderers/NELinkRenderer.cpp

		HEADERS += \
			Rendering/Spatial/Renderers/NECouplerRenderer.h \
			#Rendering/Spatial/NEHeightFieldSourceRenderer.h \
			Rendering/Spatial/Renderers/NELinkRenderer.h
}

