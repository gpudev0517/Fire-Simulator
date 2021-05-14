include(../../QMake/install.pri)


INSTALL_UIU_HEADERS = \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Abstract/NEAbstractDialog.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Curve/NECurveScene.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Curve/NECurveView.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Curve/NEKey.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Curve/NEEnumCurveScene.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Dialogs/Plot/NECustomPlot.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Utilities/ganalytics.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Utilities/NEGUIStyle.h \
    $$INSTALL_CODE_ROOT/UIUtilities/Utilities/NELogBrowser.h \
    $$INSTALL_CODE_ROOT/UIUtilities/globals_UIUtilities.h
	
  

for(header, INSTALL_UIU_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}


INSTALL_BINARY_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEUIUtilities.$$DLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEUIUtilities.$$SLIBEXT 



InstallRenderBin.files = $$INSTALL_BINARY_FILES
InstallRenderBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallRenderBin



