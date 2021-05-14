# Builds the NEUI module

include(../QMake/common.pri)
include (install.pri)


TARGET   = NEUIUtilities
TEMPLATE = lib
QT      += core widgets network printsupport
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NEUIUTILITIES_BUILD
PRECOMPILED_HEADER = PCH_UIUtilities.h

INCLUDEPATH += \
	$$ROOTPATH/Code \
        $$ROOTPATH/Code/Core \
	$$ROOTPATH/External \
        $$ROOTPATH/External/include \
        $$ROOTPATH/External/PythonQt/src/


INCLUDEPATH += \
        $$NDYNPATH/include/Core


QMAKE_LIBDIR += $$OUT_PWD
LIBS += -lNECore

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
    Dialogs/Abstract/NEAbstractDialog.cpp \
    Dialogs/Curve/NECurveScene.cpp \
    Dialogs/Curve/NECurveView.cpp \
    Dialogs/Curve/NEEnumCurveScene.cpp \
    Dialogs/Curve/NEKey.cpp \
    Dialogs/Plot/NECustomPlot.cpp \
    Utilities/ganalytics.cpp \
    Utilities/NEGUIStyle.cpp \
    Utilities/NELogBrowser.cpp



HEADERS  += \
    Dialogs/Abstract/NEAbstractDialog.h \
    Dialogs/Curve/NECurveScene.h \
    Dialogs/Curve/NECurveView.h \
    Dialogs/Curve/NEKey.h \
    Dialogs/Plot/NECustomPlot.h \
    Dialogs/Curve/NEEnumCurveScene.h \
    Utilities/ganalytics.h \
    Utilities/NEGUIStyle.h \
    Utilities/NELogBrowser.h \
    globals_UIUtilities.h \
    PCH_UIUtilities.h

