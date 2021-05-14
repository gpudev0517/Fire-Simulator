#-------------------------------------------------
#
# Project created by QtCreator 2013-12-20T19:30:13
#
#-------------------------------------------------
include(../../QMake/utility.pri)
include(../../QMake/external_lib_config.pri)

QT -= core gui

#DESTDIR = ../../../../Data/Libs
TARGET = cudpp
TEMPLATE = lib

DEFINES += CUDPP_LIBRARY

INCLUDEPATH += cudpp \
		cudpp_hash

QMAKE_CXXFLAGS += -fPIC

CUDA_SOURCES += \
		cudpp/app/compact_app.cu \
		cudpp/app/compress_app.cu \
		cudpp/app/listrank_app.cu \
		cudpp/app/mergesort_app.cu \
		cudpp/app/radixsort_app.cu \
		cudpp/app/rand_app.cu \
		cudpp/app/reduce_app.cu \
		cudpp/app/scan_app.cu \
		cudpp/app/segmented_scan_app.cu \
		cudpp/app/spmvmult_app.cu \
		cudpp/app/stringsort_app.cu \
		cudpp/app/tridiagonal_app.cu

CUDA_HEADERS += \


OTHER_FILES += $$CUDA_SOURCES $$CUDA_HEADERS


SOURCES += \
	cudpp/cudpp_manager.cpp \
	cudpp/cudpp_maximal_launch.cpp \
	cudpp/cudpp_plan.cpp \
	cudpp/cudpp.cpp

HEADERS += \
	cudpp/cuda_util.h \
	cudpp/cudpp_compact.h \
	cudpp/cudpp_compress.h \
	cudpp/cudpp_globals.h \
	cudpp/cudpp_listrank.h \
	cudpp/cudpp_manager.h \
	cudpp/cudpp_maximal_launch.h \
	cudpp/cudpp_mergesort.h \
	cudpp/cudpp_plan.h \
	cudpp/cudpp_radixsort.h \
	cudpp/cudpp_rand.h \
	cudpp/cudpp_reduce.h \
	cudpp/cudpp_scan.h \
	cudpp/cudpp_segscan.h \
	cudpp/cudpp_spmvmult.h \
	cudpp/cudpp_stringsort.h \
	cudpp/cudpp_tridiagonal.h \
	cudpp/cudpp_util.h \
	cudpp/sharedmem.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32 {
    DEFINES += cudpp_EXPORTS
	#QMAKE_LFLAGS += -MD
}

CUDA_INC = $$join(PWD,"","","/cudpp")
CUDA_INC = $$join(CUDA_INC,"",'-I"','"')
CUDA_LIBS = -lcuda -lcudart
#message(Cuda includes: $$CUDA_INC)

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

include(../../QMake/cuda_build.pri)

DISTFILES += \

