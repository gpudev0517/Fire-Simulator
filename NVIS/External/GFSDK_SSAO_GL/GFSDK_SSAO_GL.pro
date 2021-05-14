#-------------------------------------------------
#
# Project created by QtCreator 2017-07-28T14:27:19
#
#-------------------------------------------------
include(../../QMake/external_lib_config.pri)

QT       += opengl
QT       += gui

TARGET = GFSDK_SSAO_GL
TEMPLATE = lib


macx {
    DEFINES += MAC_OS MACOSX
}

win32 {
    DEFINES += _WINDLL _WINDOWS UNICODE
}

DEFINES += GFSDK_SSAO_GL_LIBRARY "SUPPORT_GL=1" GLEW_STATIC

SOURCES += \
    API.cpp \
    AppState_GL.cpp \
    ConstantBuffers.cpp \
    ProjectionMatrixInfo.cpp \
    RandomTexture.cpp \
    Renderer_GL.cpp \
    Shaders_GL.cpp \
    States_GL.cpp \
    shaders/BlurX_PS.cpp \
    shaders/BlurY_PS.cpp \
    shaders/CoarseAO_PS.cpp \
    shaders/CopyDepth_PS.cpp \
    shaders/DebugNormals_PS.cpp \
    shaders/DeinterleaveDepth_PS.cpp \
    shaders/LinearizeDepth_PS.cpp \
    shaders/ReconstructNormal_PS.cpp \
    shaders/ReinterleaveAO_PS.cpp

HEADERS += \
    AppState_GL.h \
    BuildVersion.h \
    Common.h \
    Common_GL.h \
    ConstantBuffers.h \
    GLSLProgram.h \
    InputDepthInfo.h \
    InputNormalInfo.h \
    InputViewport.h \
    MathUtil.h \
    MatrixView.h \
    OutputInfo.h \
    PerfMarkers.h \
    ProjectionMatrixInfo.h \
    RandomTexture.h \
    Renderer_GL.h \
    RenderOptions.h \
    RenderTargets_GL.h \
    Shaders_GL.h \
    States_GL.h \
    TextureUtil.h \
    TimestampQueries.h \
    UserTexture.h \
    VAO_GL.h \
    Viewports.h \
    GFSDK_SSAO.h \
    shaders/BlurX_PS.h \
    shaders/BlurY_PS.h \
    shaders/CoarseAO_PS.h \
    shaders/CopyDepth_PS.h \
    shaders/DebugNormals_PS.h \
    shaders/DeinterleaveDepth_PS.h \
    shaders/LinearizeDepth_PS.h \
    shaders/ReconstructNormal_PS.h \
    shaders/ReinterleaveAO_PS.h \
    shaders/SharedDefines.h \
    shaders/ConstantBuffers.hlsl

unix {
    target.path = /usr/lib
    INSTALLS += target
}
