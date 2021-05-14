# Builds the NERendering module

include(../QMake/common.pri)
include($$ROOTPATH/QMake/utility.pri)
include(install.pri)

TARGET   = NERender
TEMPLATE = lib
QT += core opengl openglextensions svg
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NERENDERING_BUILD _USE_MATH_DEFINES
PRECOMPILED_HEADER = PCH_Render.h




# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}

# Common Includes #

INCLUDEPATH += \
        $$NDYNPATH/include \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/include/Physics \
        $$NDYNPATH/include/PostProcessor \
	$$ROOTPATH \
        $$ROOTPATH/Code \
        #$$ROOTPATH/Code/Core \
        $$ROOTPATH/Code/Geometry \
	$$ROOTPATH/External \
	$$ROOTPATH/External/include \
        $$ROOTPATH/External/OpenVDB


# Platform Specific Configurations #

win32 {
	INCLUDEPATH +=\
                $$ROOTPATH/External/Windows/include \
                $$ROOTPATH/External/Windows/include/FFMpeg

}

unix:!macx {
    INCLUDEPATH += \
                $$ROOTPATH/External/Linux/include \
                $$ROOTPATH/External/Linux/include/FFMpeg \
                /usr/include/freetype2
}



LIBS += -lavcodec -lavformat -lswscale -lswresample -lavutil
LIBS += -lNECore -lNEPhysics -lNEPostProcessor -lNEGeometry -lOpenVDB -lHalf -ltbb

win32 {
    LIBS += -lGFSDK_SSAO_GL
    LIBS += -lmupdf-curl
}


if($$CUDA_INSTALLED) {
	DEFINES += CUDA_AVAILABLE
        #LIBS += -lGVDB
}

LIBS += -lftgl


win32 {
        INCLUDEPATH += $$ROOTPATH/External/Windows/include/Freetype2
}

macx {
        INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
}

unix:!macx {
        INCLUDEPATH += $$ROOTPATH/External/Linux/include/freetype2
}


SOURCES += \
	Rendering/Filter/NEFilterFXAA.cpp \
	Rendering/Manager/NEGLManager.cpp \
        Rendering/Manager/NEIGLSurface.cpp \
	Rendering/Manager/NERenderManager.cpp \
	Rendering/NEOsd3d.cpp \
	Rendering/NEGLRenderer.cpp \
	Rendering/NEGLRenderableMesh.cpp \
	Rendering/Resource/Material/NEClearWaterMaterial.cpp \
	Rendering/Resource/Material/NEOceanMaterial.cpp \
	Rendering/Resource/Material/NEPBRMaterial.cpp \
	Rendering/Resource/Material/NEPhongMaterial.cpp \
	Rendering/Resource/Material/NEStructureMaterial.cpp \
	Rendering/Resource/Material/NETerrainMaterial.cpp \
	Rendering/Resource/Material/NEToneMaterial.cpp \
	Rendering/Resource/Material/NEToonMaterial.cpp \
	Rendering/Resource/Material/NETransparentMaterial.cpp \
	Rendering/Resource/Material/NEVolumeMaterial.cpp \
	Rendering/Resource/NEIndexBuffer.cpp \
	Rendering/Resource/NEMaterial.cpp \
        #Rendering/Resource/NESurface.cpp \
	Rendering/Resource/NEVertexBuffer.cpp \
	Rendering/Spatial/Light/NEAreaLight.cpp \
	Rendering/Spatial/Light/NEDirectionalLight.cpp \
	Rendering/Spatial/Light/NEImageBasedLight.cpp \
	Rendering/Spatial/Light/NEPointLight.cpp \
	Rendering/Spatial/Light/NESkyLight.cpp \
	Rendering/Spatial/Light/NESpotLight.cpp \
	Rendering/Spatial/NECamera.cpp \
    	Rendering/Spatial/NEFloorPlane.cpp \
	Rendering/Spatial/NELight.cpp \
        Rendering/Spatial/NEMeasurementItem.cpp \
        Rendering/Spatial/Renderers/NESpatialRenderer.cpp \
        Rendering/Spatial/Renderers/NESpatialMeshRenderer.cpp \
        Rendering/Spatial/Renderers/NEZoneRenderer.cpp \
        Rendering/Spatial/Renderers/NEZoneCompartmentRenderer.cpp \
        Rendering/Spatial/Renderers/NESpatialGroupRenderer.cpp \
        Rendering/Spatial/Renderers/NELightsRenderer.cpp \
        Rendering/Spatial/Renderers/NECameraRenderer.cpp \
        Rendering/Spatial/Renderers/NEObjectRenderer.cpp \
        Rendering/Spatial/Renderers/NEOSDBaseRenderer.cpp \
	Rendering/Spatial/NEOSDBase.cpp \
	Rendering/Spatial/NEOSDGlobalInformation.cpp \
        Rendering/Spatial/NEClipPlane.cpp \
	Rendering/Spatial/NENullGizmo.cpp \
	Rendering/Spatial/NEInteractiveMeasurementGizmo.cpp \
    	Rendering/Utility/RenderSettings/NEPovrayExport.cpp \
    	Rendering/Utility/RenderSettings/NESpecificRenderSetting.cpp \
	Rendering/Utility/RenderSettings/NEPovRaySettings.cpp \
	Rendering/Utility/RenderSettings/NEOpenGLSettings.cpp \
	Rendering/Utility/RenderSettings/NERenderSetting.cpp \
	Utility/glutils.cpp

HEADERS  += \
    	Rendering/Spatial/NEFloorPlane.h \
        Rendering/Spatial/NEMeasurementItem.h \
    	Rendering/Utility/RenderSettings/NEPovrayExport.h \
        globals_Render.h \
	Rendering/Filter/NEFilterBase.h \
	Rendering/Filter/NEFilterFXAA.h \
	Rendering/Manager/NEGLManager.h \
        Rendering/Manager/NEIGLSurface.h \
	Rendering/Manager/NERenderManager.h \
	Rendering/NEOsd3d.h \
	Rendering/NEGLRenderer.h \
	Rendering/NEGLRenderableMesh.h \
	Rendering/NEGLRenderer_p.h \
	Rendering/Resource/Material/NEClearWaterMaterial.h \
	Rendering/Resource/Material/NEOceanMaterial.h \
	Rendering/Resource/Material/NEPBRMaterial.h \
	Rendering/Resource/Material/NEPhongMaterial.h \
	Rendering/Resource/Material/NEStructureMaterial.h \
	Rendering/Resource/Material/NETerrainMaterial.h \
	Rendering/Resource/Material/NEToneMaterial.h \
	Rendering/Resource/Material/NEToonMaterial.h \
	Rendering/Resource/Material/NETransparentMaterial.h \
	Rendering/Resource/Material/NEVolumeMaterial.h \
	Rendering/Resource/NEIndexBuffer.h \
	Rendering/Resource/NEMaterial.h \
        #Rendering/Resource/NESurface.h \
	Rendering/Resource/NEVertexBuffer.h \
	Rendering/Resource/NEVertexFormats.h \
	Rendering/Spatial/Light/NEAreaLight.h \
	Rendering/Spatial/Light/NEDirectionalLight.h \
	Rendering/Spatial/Light/NEImageBasedLight.h \
	Rendering/Spatial/Light/NEPointLight.h \
	Rendering/Spatial/Light/NESkyLight.h \
	Rendering/Spatial/Light/NESpotLight.h \
	Rendering/Spatial/NECamera.h \
	Rendering/Spatial/NECamera_p.h \
	Rendering/Spatial/NELight.h \
        Rendering/Spatial/Renderers/NESpatialRenderer.h \
        Rendering/Spatial/Renderers/NESpatialMeshRenderer.h \
        Rendering/Spatial/Renderers/NEZoneRenderer.h \
        Rendering/Spatial/Renderers/NEZoneCompartmentRenderer.h \
        Rendering/Spatial/Renderers/NESpatialGroupRenderer.h \
        Rendering/Spatial/Renderers/NELightsRenderer.h \
        Rendering/Spatial/Renderers/NECameraRenderer.h \
        Rendering/Spatial/Renderers/NEObjectRenderer.h \
        Rendering/Spatial/Renderers/NEOSDBaseRenderer.h \
	Rendering/Spatial/NEOSDBase.h \
	Rendering/Spatial/NEOSDGlobalInformation.h \
        Rendering/Spatial/NEClipPlane.h \
	Rendering/Spatial/NENullGizmo.h \
	Rendering/Spatial/NEInteractiveMeasurementGizmo.h \
	Rendering/Utility/RenderSettings/NESpecificRenderSetting.h \
	Rendering/Utility/RenderSettings/NEPovRaySettings.h \
	Rendering/Utility/RenderSettings/NEOpenGLSettings.h \
	Rendering/Utility/RenderSettings/NERenderSetting.h \
	Utility/glutils.h \
	PCH_Render.h \
	SearchTex.h


if($$USE_VTK) {
        HEADERS += 	Rendering/Utility/RenderSettings/NEVTKExport.h \
                        Rendering/Utility/RenderSettings/NEVTKSettings.h
        SOURCES +=      Rendering/Utility/RenderSettings/NEVTKExport.cpp \
			Rendering/Utility/RenderSettings/NEVTKSettings.cpp
}




if($$USE_VOLUME) {
	SOURCES += Rendering/Spatial/NEOSDVolume.cpp \
                   Rendering/Spatial/Renderers/NEVolumeRenderer.cpp

	HEADERS += Rendering/Spatial/NEOSDVolume.h \
                   Rendering/Spatial/Renderers/NEVolumeRenderer.h
}

