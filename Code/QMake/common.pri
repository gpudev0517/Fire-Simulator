CONFIG -= c++11 debug_and_release debug_and_release_target precompile_header lex yacc warn_on # release debug
CONFIG += c++14 console force_debug_info
DEFINES -= _UNICODE UNICODE

RELEASE = false

CONFIG(release, debug|release) {
	RELEASE = true
	CONFIG -= debug release
	CONFIG += release
} else {
	DEFINES += _DEBUG
	CONFIG -= debug release
	CONFIG += debug
}

ROOTPATH = $$PWD/../..
ParentDirectory = $$ROOTPATH

if($$RELEASE){
NDYNPATH = $$ROOTPATH/NDYN
} else {
NDYNPATH = $$ROOTPATH/NDYN_Debug
}



#ODIR        = obj
RCC_DIR     = "$$OUT_PWD/rcc"
UI_DIR	    = "$$OUT_PWD/uic"
MOC_DIR	    = "$$OUT_PWD/moc"
OBJECTS_DIR = "$$OUT_PWD/obj"


# Common Libraries

if($$RELEASE) {
        LIBS += -L$$ROOTPATH/Build/Release
} else {
        LIBS += -L$$ROOTPATH/Build/Debug
}


# Platform Specific Configurations #

win32 {
    QMAKE_CXXFLAGS += -D_SCL_SECURE_NO_WARNINGS -openmp
    QMAKE_COMPILER_DEFINES += _WIN32 _OPENMP
        LIBS += -L$$NDYNPATH/External/Windows/lib
	DEFINES += NOMINMAX _OPENMP
    DISABLED_WARNINGS = /wd4100 /wd4661 /wd4503 /wd4005 /wd4267 /wd4273 /wd4133 /wd4146 /wd4002 /wd4138 /wd4189 /wd4521 /wd4996 /wd4576

    if($$RELEASE) {
        QMAKE_CXXFLAGS += /MD /bigobj $$DISABLED_WARNINGS
        QMAKE_CFLAGS += /MD /bigobj $$DISABLED_WARNINGS
        QMAKE_LFLAGS += /NODEFAULTLIB:LIBMT.lib # /LTCG
    } else {
        QMAKE_CXXFLAGS += /MDd /bigobj $$DISABLED_WARNINGS
        QMAKE_CFLAGS += /MDd /bigobj $$DISABLED_WARNINGS
        QMAKE_LFLAGS += /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:MSVCRT.lib# /LTCG
    }

}

unix {
    DISABLED_WARNINGS = -Wno-conversion-null \
                      -Wno-return-type \
                      -Wno-maybe-unitialized \
                      -Wno-overloaded-virtual \
                      -Wno-unused-variable \
                      -Wno-reorder \
                      -Wno-sign-compare \
                      -Wno-address \
                      -Wno-switch \
                      -Wno-narrowing \
                      -Wno-strict-aliasing \
                      -Wno-unused-but-set-variable
}

unix:!macx {
    QMAKE_CXXFLAGS += -fopenmp \
                      -fPIC \
                      $$DISABLED_WARNINGS \
                      -isystem $$NDYNPATH/External

    if($$RELEASE) {
        QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
        QMAKE_CXXFLAGS_RELEASE -= -O2
    } else {
        QMAKE_CXXFLAGS += -O0
        DEFINES += _DEBUG
    }

    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/Libs\''
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
    LIBS += -L$$NDYNPATH/External/Linux/lib

    if( $$USE_OPENCL ) {
        LIBS += -lcl
    }
    LIBS += -lgomp
}

macx {
    QMAKE_CXXFLAGS += $$DISABLED_WARNINGS \
                     -isystem $$NDYNPATH/External

    if($$RELEASE) {
        QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
        QMAKE_CXXFLAGS_RELEASE -= -O2
    } else {
        QMAKE_CXXFLAGS += -O0
    }

    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/Libs\''
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/\''
    LIBS += -L$$NDYNPATH/External/macosx/lib

    LIBS += -lz \
            -framework OpenCL

}
