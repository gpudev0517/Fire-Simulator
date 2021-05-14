include(../../QMake/install.pri)



INSTALL_GEOM_HEADERS = \
    $$INSTALL_CODE_ROOT/Geometry/globals_Geometry.h \
    $$INSTALL_CODE_ROOT/Geometry/Manager/NEMeshVolGeometryManager.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEMeshConstructor.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NESpatialMesh.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NESpatialEngine.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEZone.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEZoneCompartment.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEScenario.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEZoneItem.h \
    $$INSTALL_CODE_ROOT/Geometry/Mesh/NEScenarioItem.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NECustomVariants.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NESurfaceMaterial.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NEFireMaterial.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NEFailureCriteria.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NEVentCriteria.h \
    $$INSTALL_CODE_ROOT/Geometry/Utility/NECableProperty.h \
    $$INSTALL_CODE_ROOT/Geometry/Resource/Mesh/NEMeshContainer.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Core/NEVolumeNode.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Core/NEVolume.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Spatial/NEVolumeFilter.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Spatial/NEVolumeCombine.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Spatial/NEVolumeFile.h \
    $$INSTALL_CODE_ROOT/Geometry/Volume/Spatial/NEVolumeUtils.h



for(header, INSTALL_GEOM_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}



INSTALL_BINARY_FILES += \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEGeometry.$$DLIBEXT

CONFIG(release, debug|release) {
    INSTALL_BINARY_FILES += \
        $${ROOTPATH}/Build/$${BUILD_CONF}/PythonQt.$$DLIBEXT
} else {
    INSTALL_BINARY_FILES += \
        $${ROOTPATH}/Build/$${BUILD_CONF}/PythonQt_d.$$DLIBEXT
}


InstallGeomBin.files = $$INSTALL_BINARY_FILES
InstallGeomBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGeomBin


INSTALL_LIB_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEGeometry.$$SLIBEXT

CONFIG(release, debug|release) {
    INSTALL_LIB_FILES += \
        $${ROOTPATH}/Build/$${BUILD_CONF}/PythonQt.$$SLIBEXT
} else {
    INSTALL_LIB_FILES += \
        $${ROOTPATH}/Build/$${BUILD_CONF}/PythonQt_d.$$SLIBEXT
}


InstallGeomLib.files = $$INSTALL_LIB_FILES
InstallGeomLib.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGeomLib



#
# External Libraries built within
#


INSTALL_NEXT_LIBS = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/OpenVDB.$$DLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/qtcsv.$$DLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/quazip.$$DLIBEXT

ExternalSelfLibs.files = $$INSTALL_NEXT_LIBS
ExternalSelfLibs.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= ExternalSelfLibs


#
# Externals libraries prebuilt
#

CONFIG(release, debug|release) {
    INSTALL_EXT_LIBS = \
        $$INSTALL_CODE_ROOT/../External/windows/lib/tbb.$$DLIBEXT \
        $$INSTALL_CODE_ROOT/../External/windows/lib/Half.$$DLIBEXT

} else {
    INSTALL_EXT_LIBS = \
        $$INSTALL_CODE_ROOT/../External/windows/lib/tbb.$$DLIBEXT \
        $$INSTALL_CODE_ROOT/../External/windows/lib/tbb_debug.$$DLIBEXT \
        $$INSTALL_CODE_ROOT/../External/windows/lib/Half.$$DLIBEXT
}

ExternalLibs.files = $$INSTALL_EXT_LIBS
ExternalLibs.path  = $$INSTALL_PREFIX/External/windows/lib
INSTALLS *= ExternalLibs

