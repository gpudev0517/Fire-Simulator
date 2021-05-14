
include(../../QMake/utility.pri)

if($$CUDA_INSTALLED) {

include(../../QMake/external_lib_config.pri)

    QT       += opengl openglextensions
    QT       += gui

    TARGET = GVDB
    TEMPLATE = lib


    DEFINES += NOMINMAX GLEW_NO_GLU USECUDA GVDB_EXPORTS BUILD_OPENGL

    unix {
        target.path = /usr/lib
        INSTALLS += target
    }

    INCLUDEPATH += src
    DEPENDPATH += $$PWD/

    SOURCES += \
        src/app_perf.cpp \
        src/gvdb_allocator.cpp \
        src/gvdb_camera.cpp \
        src/gvdb_model.cpp \
        src/gvdb_node.cpp \
        src/gvdb_render_opengl.cpp \
        src/gvdb_scene.cpp \
        src/gvdb_types.cpp \
        src/gvdb_vec.cpp \
        src/gvdb_volume_3D.cpp \
        src/gvdb_volume_base.cpp \
        src/gvdb_volume_gvdb.cpp \
        src/loader_ObjarReader.cpp \
        src/loader_OBJReader.cpp \
        src/loader_Parser.cpp \
        src/string_helper.cpp

    HEADERS += \
        src/app_perf.h \
        src/gvdb.h \
        src/gvdb_allocator.h \
        src/gvdb_camera.h \
        src/gvdb_model.h \
        src/gvdb_node.h \
        src/gvdb_render.h \
        src/gvdb_scene.h \
        src/gvdb_types.h \
        src/gvdb_vec.h \
        src/gvdb_volume_3D.h \
        src/gvdb_volume_base.h \
        src/gvdb_volume_gvdb.h \
        src/loader_ObjarReader.h \
        src/loader_OBJReader.h \
        src/loader_Parser.h \
        src/string_helper.h

    CUDA_PTX_SOURCES += kernels/cuda_gvdb_copydata.cu \
                    kernels/cuda_gvdb_module.cu

    CUDA_PTX_INC = $$join(PWD,"","","/kernels")
    CUDA_PTX_INC = $$join(CUDA_PTX_INC,"",'-I"','"')

    OTHER_FILES += $$CUDA_PTX_SOURCES

    CUDA_LIBS = -lcuda -lcudart

    #message(Cuda includes: $$CUDA_INC)

    SYSTEM_TYPE = 64            # '32' or '64', depending on your system
    CUDA_ARCH = compute_30      # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
    CUDA_CODE = sm_30
    CUDA_PTXAS_OPTIONS = -v -O3
    CUDA_MAXRREGCOUNT = 64

    win32 {
            NVCC_PTX_OPTIONS = --ptx --use_fast_math
            CUDA_SDK = $$(CUDA_PATH)
            INCLUDEPATH += $$CUDA_SDK/include
            LIBS += $$CUDA_SDK/lib/x64/cuda.lib \
            $$CUDA_SDK/lib/x64/cudart.lib
    }

    unix:!macx {
            NVCC_OPTIONS += -fpic

            QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
                                            /usr/lib/nvidia-319
            LIBS += $$CUDA_LIBS
    }
    macx {
            QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
                                            /usr/lib/nvidia-319
            LIBS += $$CUDA_LIBS
    }

    CONFIG(debug, debug|release) {
            # Debug mode
            CUDA_PTX_DEST_DIR = $$PWD/../../Build/Debug
            cuda_ptx_d.input = CUDA_PTX_SOURCES
            cuda_ptx_d.output = $$CUDA_PTX_DEST_DIR/${QMAKE_FILE_BASE}.ptx
            cuda_ptx_d.commands = nvcc -D_DEBUG $$NVCC_PTX_OPTIONS --machine=$$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE --ptxas-options=$$CUDA_PTXAS_OPTIONS ${QMAKE_FILE_NAME} $$CUDA_PTX_INC -o ${QMAKE_FILE_OUT}
            cuda_ptx_d.dependency_type = TYPE_C
            cuda_ptx_d.CONFIG += target_predeps no_link
            QMAKE_EXTRA_COMPILERS += cuda_ptx_d
    }
    else {
            # Release mode
            CUDA_PTX_DEST_DIR = $$PWD/../../Build/Release
            cuda_ptx.input = CUDA_PTX_SOURCES
            cuda_ptx.output = $$CUDA_PTX_DEST_DIR/${QMAKE_FILE_BASE}.ptx
            cuda_ptx.commands = nvcc $$NVCC_PTX_OPTIONS --machine=$$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE --ptxas-options=$$CUDA_PTXAS_OPTIONS ${QMAKE_FILE_NAME} $$CUDA_PTX_INC -o ${QMAKE_FILE_OUT}
            cuda_ptx.dependency_type = TYPE_C
            cuda_ptx.CONFIG += target_predeps no_link
            QMAKE_EXTRA_COMPILERS += cuda_ptx
    }


    win32 {
            INCLUDEPATH += $$PWD/../shared_cudpp/include
        CONFIG(debug, debug|release) {
            # Debug mode
            LIBS += $$PWD/../shared_cudpp/lib/cudpp_2017x64d.lib
        }
        else {
            # Release mode
            LIBS += $$PWD/../shared_cudpp/lib/cudpp_2017x64.lib
        }
    }

}
else{
        message("No CUDA compiler found: Can't build gvdb.")
}
