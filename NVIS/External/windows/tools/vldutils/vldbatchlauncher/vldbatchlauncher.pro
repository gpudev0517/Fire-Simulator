QT += core
QT -= gui

CONFIG += c++11

TARGET = vldbatchlauncher
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    vldbatchlauncher.cpp
DESTDIR = $$PWD/../
