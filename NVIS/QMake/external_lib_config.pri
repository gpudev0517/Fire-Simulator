CONFIG += c++11
#CONFIG += staticlib


unix:!macx {
    INCLUDEPATH += $$PWD/../External/Linux/include
}

macx {
    INCLUDEPATH += $$PWD/../External/macosx/include
    QMAKE_LIBDIR += $$PWD/../External/macosx/lib
}


unix{
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
        QMAKE_CXXFLAGS_RELEASE -= -O2
    }else {
        QMAKE_CXXFLAGS += -O0
    }
}

win32{
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS += /O2
    }else {
		QMAKE_CXXFLAGS += /O2
    }
    INCLUDEPATH += $$PWD/../External/windows/include \
                   ./
    QMAKE_LIBDIR += $$PWD/../External/windows/lib
    DEFINES -= UNICODE
}

CONFIG(release, debug|release) {
    CONFIG -= debug dll
    CONFIG += release dll
    DESTDIR = $$PWD/../Build/Release
}else {
    CONFIG -= release dll
    CONFIG += debug dll
    DEFINES += _DEBUG
    DESTDIR = $$PWD/../Build/Debug
}



#
# Installation of headers
#

INSTALL_INC_DIR = $$PWD/../External/include


