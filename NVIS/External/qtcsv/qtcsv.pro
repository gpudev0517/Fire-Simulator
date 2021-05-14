#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T02:27:57
#
#-------------------------------------------------
include(../../QMake/external_lib_config.pri)

QT       -= gui

TARGET = qtcsv
TEMPLATE = lib
#CONFIG += sharedlib

DEFINES += QTCSV_LIBRARY

SOURCES += \
    reader.cpp \
    stringdata.cpp \
    variantdata.cpp \
    writer.cpp

HEADERS += \
    abstractdata.h \
    filechecker.h \
    qtcsv.h \
    qtcsv_global.h \
    reader.h \
    stringdata.h \
    variantdata.h \
    writer.h




INSTALL_PREFIX = $$INSTALL_INC_DIR
INSTALL_HEADERS = $$HEADERS
for(header, INSTALL_HEADERS) {
  path = $${INSTALL_PREFIX}/$${dirname(header)}/qtcsv
  path = $$relative_path($$path)
  eval(headers_$${path}.files += $$header)
  eval(headers_$${path}.path = $$path)
  eval(INSTALLS *= headers_$${path})
}


