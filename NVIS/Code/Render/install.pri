include(../../QMake/install.pri)



INSTALL_RENDER_HEADERS = \
    $$INSTALL_CODE_ROOT/Render/globals_Render.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Filter/NEFilterBase.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Filter/NEFilterFXAA.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Manager/NEGLManager.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Manager/NEIGLSurface.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Manager/NERenderManager.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/NEIndexBuffer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/NEMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NETransparentMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEClearWaterMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEPhongMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEStructureMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NETerrainMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEToneMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEToonMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEVolumeMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEOceanMaterial.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/Material/NEPBRMaterial.h \
    #$$INSTALL_CODE_ROOT/Render/Rendering/Resource/NESurface.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/NEVertexBuffer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Resource/NEVertexFormats.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NECamera.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEClipPlane.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEInteractiveMeasurementGizmo.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NELight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NECamera.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NENullGizmo.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEOSDBase.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEMeasurementItem.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEFloorPlane.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEOSDGlobalInformation.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/NEOSDVolume.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NEAreaLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NEDirectionalLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NEImageBasedLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NEPointLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NESkyLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Light/NESpotLight.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/NEGLRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/NEGLRenderableMesh.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/NEOsd3d.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NEOSDBaseRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NELightsRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NECameraRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NEObjectRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NESpatialRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NESpatialMeshRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NEVolumeRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NESpatialGroupRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NEZoneRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Spatial/Renderers/NEZoneCompartmentRenderer.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Utility/RenderSettings/NERenderSetting.h \
    $$INSTALL_CODE_ROOT/Render/Rendering/Utility/RenderSettings/NESpecificRenderSetting.h


for(header, INSTALL_RENDER_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}


INSTALL_BINARY_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NERender.$$DLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NERender.$$SLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/GFSDK_SSAO_GL.$$DLIBEXT



InstallRenderBin.files = $$INSTALL_BINARY_FILES
InstallRenderBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallRenderBin




#
# Externals libraries prebuilt
#


INSTALL_EXT_LIBS = \
    $$INSTALL_CODE_ROOT/../External/windows/lib/mupdf-curl.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/ftgl.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/avcodec-58.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/avdevice-58.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/avfilter-7.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/avformat-58.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/avutil-56.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/postproc-55.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/swresample-3.$$DLIBEXT \
    $$INSTALL_CODE_ROOT/../External/windows/lib/swscale-5.$$DLIBEXT


ExternalLibs.files = $$INSTALL_EXT_LIBS
ExternalLibs.path  = $$clean_path($$INSTALL_PREFIX/External/windows/lib)
INSTALLS *= ExternalLibs

# Externals libraries prebuilt
#

INSTALL_EXT_INC = \
    $$INSTALL_CODE_ROOT/../External/include/glm \
    $$INSTALL_CODE_ROOT/../External/windows/include/FTGL \
    $$INSTALL_CODE_ROOT/../External/windows/include/Freetype2



ExternalInc.files = $$INSTALL_EXT_INC
ExternalInc.path  = $$clean_path($$INSTALL_PREFIX/External/include)
INSTALLS *= ExternalInc

#CONFIG(release, debug|release) {
#    INSTALL_EXT_LIBS =
#} else {
#    INSTALL_EXT_LIBS =
#}



ExternalLibs.files = $$INSTALL_EXT_LIBS
ExternalLibs.path  = $$clean_path($$INSTALL_PREFIX/External/windows/lib)
INSTALLS *= ExternalLibs

