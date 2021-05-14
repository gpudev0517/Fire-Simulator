CONFIG += c++11

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
    DESTDIR = $$PWD/../External/tools/release
    QMAKE_LIBDIR += $$PWD/../Build/Release
    CONFIG -= debug
    CONFIG += release

}else {
    DESTDIR = $$PWD/../External/tools/debug
    QMAKE_LIBDIR += $$PWD/../Build/Debug
    CONFIG -= release
    CONFIG += debug
    DEFINES += _DEBUG
}
