win32 {
    PGICC = pgcpp
    EXT = .obj
    PGICC_OPTIONS = -fast -Minfo -acc -ta=tesla:cuda5.5

    #QMAKE_LIBDIR += "E:/Progra~1/PGI/win64/14.3/lib"
    LIBS += -llibpgc

    #QMAKE_LIBDIR += $$PGICC_PATH/lib
    #INCLUDEPATH += $$PGICC_PATH/include
    message($$QMAKE_LIBDIR)
    #QMAKE_LFLAGS_RELEASE += /FORCE:MULTIPLE
}

unix:!macx {
    PGICC = pgc++
    EXT = .o
    PGICC_OPTIONS = -fast -Minfo -acc -ta=tesla:cuda5.5
    QMAKE_LIBDIR += /opt/pgi/linux86-64/14.3/lib
    LIBS += -lpgc
#    LIBS += -laccapi
#    LIBS += -laccg
#    LIBS += -lstd
#    LIBS += -C
#LIBS += -laccn
#LIBS += -lacco
#LIBS += -laccao
#LIBS += -laccg2
#LIBS += -laccnc
#LIBS += -laccgmp
#LIBS += -laccnmp
#LIBS += -laccomp
#LIBS += -laccg2mp
#LIBS += -laccncmp
#LIBS += -laccapimp
}
macx {
    PGICC_OPTIONS = -fast -Minfo -acc
}


#message(Libraries: $$QMAKE_LIBS_WINDOWS)


#message($$PWD)
#message(Cuda includes: $$CUDA_INC)

OPENACC_OBJECTS_DIR = ./



CONFIG(debug, debug|release) {
    # Debug mode
    openacc_d.input = OPENACC_SOURCES
    openacc_d.output = $$OPENACC_OBJECTS_DIR/${QMAKE_FILE_BASE}$$EXT
    openacc_d.commands = $$PGICC $$PGICC_OPTIONS $$OPENACC_INC -c ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    openacc_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += openacc_d
}
else {
    # Release mode
    openacc.input = OPENACC_SOURCES
    openacc.output = $$OPENACC_OBJECTS_DIR/${QMAKE_FILE_BASE}$$EXT
    openacc.commands = $$PGICC $$PGICC_OPTIONS $$OPENACC_INC -c ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    openacc.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += openacc
}

