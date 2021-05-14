include(../../QMake/common.pri)
include(install.pri)

QT       += opengl
QT       -= gui

TARGET = gizmo
TEMPLATE = lib
#CONFIG += staticlib
#CONFIG -= dll

macx {
    DEFINES += MAC_OS
}

# Output Configuration

if($$RELEASE) {
        DESTDIR = $$ROOTPATH/Build/Release
} else {
        DESTDIR = $$ROOTPATH/Build/Debug
}


DEFINES += GIZMO_LIBRARY

SOURCES += \
    GizmoTransformMove.cpp \
    GizmoTransformRender.cpp \
    GizmoTransformRotate.cpp \
    GizmoTransformScale.cpp \
    ZBaseMaths.cpp \
    ZBaseMaths.inl \
    ZMathsFunc.cpp

HEADERS += \
    GizmoTransform.h \
    GizmoTransformMove.h \
    GizmoTransformRender.h \
    GizmoTransformRotate.h \
    GizmoTransformScale.h \
    IGizmo.h \
    ZBaseDefs.h \
    ZBaseMaths.h \
    ZMathsFunc.h \
    gizmo_global.h





