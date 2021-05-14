# Builds the main executable

include($$PWD/../../Code/QMake/common.pri)

TARGET = Neutrino
TEMPLATE = app
QT += widgets core gui network opengl openglextensions xml
CONFIG += precompile_header
PRECOMPILED_HEADER = PCH_AppMain.h


# Common Configurations #

INCLUDEPATH +=\
        ./ \
        $$NDYNPATH/include \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/include/Physics \
        $$ROOTPATH/Code \
        $$ROOTPATH/Code/Geometry \
        $$ROOTPATH/Code/Render \
        $$ROOTPATH/Code/RenderNEO \
	$$ROOTPATH/Code/UI



if($$RELEASE) {
    DESTDIR = $${OUT_PWD}/../../Build/Release
    LIBS += -L$$ROOTPATH/Build/Release
} else {
    DESTDIR = $${OUT_PWD}/../../Build/Debug
    LIBS += -L$$ROOTPATH/Build/Debug
}


# Platform Specific Configurations #



win32 {
	INCLUDEPATH  += $$ROOTPATH/External/windows/include
        RC_FILE = $$ROOTPATH/Code/UI/Resources/icons/Neutrino.rc
}

unix:!macx {
	INCLUDEPATH +=  $$ROOTPATH/External/Linux/include/
        QMAKE_CXXFLAGS += -fopenmp
        LIBS += -L$$ROOTPATH/External/Linux/lib
        QMAKE_LFLAGS += '-Wl,-rpath,\'$$ORIGIN/Libs\''
        QMAKE_LFLAGS += '-Wl,-Bdynamic'
        if($$RELEASE) {
                LIBS += -lPythonQt
        } else {
                LIBS += -lPythonQt_d
        }
        LIBS += \
                -lgizmo -lquazip -lOpenVDB  -lqtcsv \
                -Wl,-Bdynamic -lassimp -lftgl -lHalf -ltbb -lboost_system -lboost_iostreams \
                -Wl,-Bdynamic -lavutil -lavformat -lavcodec -lswscale -lswresample  -Wl,-Bdynamic
}

macx {
    LIBS += -L$$ROOTPATH/External/macosx/lib
    QMAKE_LFLAGS += '-Wl,-rpath,\'$$ORIGIN/Libs\''
    ICON = $$ROOTPATH/Code/UI/Resources/icons/Simple.icns
}


LIBS += -lNEUI -lNEGLI -lNERender -lNERenderNEO -lNEPostProcessor -lNEPhysics -lNEGeometry -lNECore




# Sources #

SOURCES += AppMain.cpp
HEADERS += \
    PCH_AppMain.h


