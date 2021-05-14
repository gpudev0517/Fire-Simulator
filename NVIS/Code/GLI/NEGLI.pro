# Builds the NEUI module

include(../QMake/common.pri)
include(install.pri)
UIPATH = $$ROOTPATH/Code/GLI
#include($$ROOTPATH/QMake/utility.pri)

TARGET   = NEGLI
TEMPLATE = lib
QT      += core widgets network printsupport gui opengl openglextensions
CONFIG += precompile_header shared
DEFINES += NEUTRINO_GLI_BUILD
PRECOMPILED_HEADER = PCH_GLI.h

INCLUDEPATH += \
        $$NDYNPATH/include/Core \
        $$ROOTPATH/Code \
        $$ROOTPATH/Code/Geometry \
        $$ROOTPATH/Code/Render \
	$$ROOTPATH/External \
	$$ROOTPATH/External/include


win32 {
        INCLUDEPATH += \
                $$ROOTPATH/External/Windows/include \
                $$ROOTPATH/External/Windows/include/FFMpeg
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



# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}


QMAKE_LIBDIR += $$OUT_PWD


LIBS += -lNECore \
        -lNEGeometry \
        -lNERender \
        -lgizmo



SOURCES += \
    NEGLWidget.cpp


HEADERS  += \
	NEGLWidget.h \
        NEGLWidget_p.h \
