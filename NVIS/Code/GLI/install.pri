include(../../QMake/install.pri)



INSTALL_GLI_HEADERS = \
    $$INSTALL_CODE_ROOT/GLI/globals_GLI.h \
    $$INSTALL_CODE_ROOT/GLI/NEGLWidget.h \
    $$INSTALL_CODE_ROOT/GLI/NEGLWidget_p.h



for(header, INSTALL_GLI_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}



INSTALL_BINARY_FILES += \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEGLI.$$DLIBEXT


InstallGliBin.files = $$INSTALL_BINARY_FILES
InstallGliBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGliBin

INSTALL_LIB_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEGLI.$$SLIBEXT


InstallGliLib.files = $$INSTALL_LIB_FILES
InstallGliLib.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallGliLib


