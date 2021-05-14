QT       -= core gui

TARGET = acclib
TEMPLATE = lib
CONFIG += shared

QMAKE_CFLAGS -= -o -O2 -D_REENTRANT
QMAKE_CXXFLAGS -= -pipe -fPIE -o -O2 -D_REENTRANT
QMAKE_CXXFLAGS_WARN_ON -= -W -Wall
QMAKE_LFLAGS += -fPIC

message($$QMAKE_CXXFLAGS)

SOURCES += NEOpenACCInit.cpp

win32 {
    QMAKE_CXX = pgcpp
    QMAKE_LINK = pgcpp
    EXT = .obj
    QMAKE_CXXFLAGS = -fast -Minfo -acc -ta=tesla:cuda5.5
    LIBS += -llibpgc
}

unix:!macx {
    QMAKE_CXX = pgc++
    QMAKE_LINK = pgc++
    EXT = .o
    QMAKE_CXXFLAGS = -fast -Minfo -acc -ta=tesla:cuda5.5
    QMAKE_LIBDIR += /opt/pgi/linux86-64/14.3/lib
}
macx {
    PGICC_OPTIONS = -fast -Minfo -acc
}

HEADERS += \
    NEOpenACCInit.h

CONFIG(release, debug|release) {
    CONFIG -= debug dll
    CONFIG += release dll
    DESTDIR = $$PWD/../lib/release
}else {
    CONFIG -= release dll
    CONFIG += debug dll
    DEFINES += _DEBUG
    DESTDIR = $$PWD/../lib/debug
}

