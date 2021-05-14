include(../../QMake/install.pri)


INSTALL_UIP_HEADERS = \
    $$INSTALL_CODE_ROOT/UIProperties/Dialogs/File/NEFileDialog.h \
    $$INSTALL_CODE_ROOT/UIProperties/Dialogs/Curve/NECurveDialog.h \
    $$INSTALL_CODE_ROOT/UIProperties/Dialogs/ColorPreset/NEColorPresetDialog.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEDoubleSpinBox.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/fileedit.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEColorRampEditor.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEObjectController.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEObjectController_p.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEVariantFactory.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/NEVariantManager.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtbuttonpropertybrowser.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qteditorfactory.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtgroupboxpropertybrowser.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtpropertybrowser.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtpropertybrowserutils_p.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtpropertymanager.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qttreepropertybrowser.h \
    $$INSTALL_CODE_ROOT/UIProperties/Windows/PropertyManager/qtvariantproperty.h \
    $$INSTALL_CODE_ROOT/UIProperties/globals_UIProperties.h
	


for(header, INSTALL_UIP_HEADERS) {
  path = $$INSTALL_INC_PREFIX/$$relative_path($$header, $$INSTALL_CODE_ROOT)
  filename = $$basename(header)


  eval(headers_$${filename}.files += $$clean_path($$header))
  eval(headers_$${filename}.path = $$clean_path($$dirname(path)))
  eval(INSTALLS *= headers_$${filename})
}


INSTALL_BINARY_FILES = \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEUIProperties.$$DLIBEXT \
    $${ROOTPATH}/Build/$${BUILD_CONF}/NEUIProperties.$$SLIBEXT 



InstallRenderBin.files = $$INSTALL_BINARY_FILES
InstallRenderBin.path  = $$INSTALL_LIB_PREFIX
INSTALLS *= InstallRenderBin



