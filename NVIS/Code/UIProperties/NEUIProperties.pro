# Builds the NEUI module

include(../QMake/common.pri)
include(install.pri)

UIPPATH = $$ROOTPATH/Code/UIProperties
#include($$ROOTPATH/QMake/utility.pri)
#include($$ROOTPATH/QMake/install.pri)


TARGET   = NEUIProperties
TEMPLATE = lib
QT      += core widgets   
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NEUIPROPERTIES_BUILD
PRECOMPILED_HEADER = PCH_UIProperties.h

INCLUDEPATH += \
	$$ROOTPATH/Code \
        $$ROOTPATH/Code/Core \
        $$ROOTPATH/Code/UIUtilities \
	$$ROOTPATH/External \
        $$ROOTPATH/External/include \
        $$ROOTPATH/External/PythonQt/src/

INCLUDEPATH += \
        $$NDYNPATH/include/Core


QMAKE_LIBDIR += $$OUT_PWD
LIBS += -lNECore -lNEUIUtilities

# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}

win32 {
	if($$RELEASE) {
		LIBS += -lPythonQt
	} else {
		LIBS += -lPythonQt_d
	}
        LIBS += -L$$ROOTPATH/External/Windows/lib
        LIBS += -lassimp-vc140-mt
}

unix:!macx {
        if($$RELEASE) {
                LIBS += -lPythonQt
        } else {
                LIBS += -lPythonQt_d
        }
        LIBS += -lassimp
}

macx {
	LIBS += -lPythonQt
        LIBS += -lassimp
}



win32 {
        INCLUDEPATH += \
                $$ROOTPATH/External/Windows/include \
                $$ROOTPATH/External/Windows/include/python27 \
        INCLUDEPATH += $$ROOTPATH/External/Windows/include/Freetype2
}

macx {
        INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
}

unix:!macx {
    INCLUDEPATH += \
                $$ROOTPATH/External/Linux/include \
                /usr/include/python2.7 \
                /usr/include/freetype2
}



SOURCES += \
    Dialogs/File/NEFileDialog.cpp \
    Dialogs/Curve/NECurveDialog.cpp \
    Dialogs/ColorPreset/NEColorPresetDialog.cpp \
    Windows/PropertyManager/NEDoubleSpinBox.cpp \
    Windows/PropertyManager/fileedit.cpp \
    Windows/PropertyManager/NEColorRampEditor.cpp \
    Windows/PropertyManager/NEObjectController.cpp \
    Windows/PropertyManager/NEVariantFactory.cpp \
    Windows/PropertyManager/NEVariantManager.cpp \
    Windows/PropertyManager/qtbuttonpropertybrowser.cpp \
    Windows/PropertyManager/qteditorfactory.cpp \
    Windows/PropertyManager/qtgroupboxpropertybrowser.cpp \
    Windows/PropertyManager/qtpropertybrowser.cpp \
    Windows/PropertyManager/qtpropertybrowserutils.cpp \
    Windows/PropertyManager/qtpropertymanager.cpp \
    Windows/PropertyManager/qttreepropertybrowser.cpp \
    Windows/PropertyManager/qtvariantproperty.cpp 



HEADERS  += \
    Dialogs/File/NEFileDialog.h \
    Dialogs/Curve/NECurveDialog.h \
    Dialogs/ColorPreset/NEColorPresetDialog.h \
    Windows/PropertyManager/NEDoubleSpinBox.h \
    Windows/PropertyManager/fileedit.h \
    Windows/PropertyManager/NEColorRampEditor.h \
    Windows/PropertyManager/NEObjectController.h \
    Windows/PropertyManager/NEObjectController_p.h \
    Windows/PropertyManager/NEVariantFactory.h \
    Windows/PropertyManager/NEVariantManager.h \
    Windows/PropertyManager/qtbuttonpropertybrowser.h \
    Windows/PropertyManager/qteditorfactory.h \
    Windows/PropertyManager/qtgroupboxpropertybrowser.h \
    Windows/PropertyManager/qtpropertybrowser.h \
    Windows/PropertyManager/qtpropertybrowserutils_p.h \
    Windows/PropertyManager/qtpropertymanager.h \
    Windows/PropertyManager/qttreepropertybrowser.h \
    Windows/PropertyManager/qtvariantproperty.h \
    globals_UIProperties.h \
    PCH_UIProperties.h



FORMS += \
    Dialogs/Curve/NECurveDialog.ui

RESOURCES += \
    Windows/PropertyManager/qtpropertybrowser.qrc 
