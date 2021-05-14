QT += core
QT -= gui

CONFIG += c++11

TARGET = sleepycsv2tasks
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    sleepycsv2tasks.cpp
DESTDIR = $$PWD/../
