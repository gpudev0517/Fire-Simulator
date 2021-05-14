include(../../../QMake/install.pri)



INSTALL_GIZMO_HEADERS = \
    $$INSTALL_CODE_ROOT/GLI/gizmo/gizmo_global.h \
    $$INSTALL_CODE_ROOT/GLI/gizmo/IGizmo.h



for(header, INSTALL_GIZMO_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}



INSTALL_BINARY_FILES += \
    $${ROOTPATH}/Build/$${BUILD_CONF}/gizmo.$$DLIBEXT



InstallGizmoBin.files = $$INSTALL_BINARY_FILES
InstallGizmoBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGizmoBin

INSTALL_LIB_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/gizmo.$$SLIBEXT


InstallGizmoLib.files = $$INSTALL_LIB_FILES
InstallGizmoLib.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGizmoLib


