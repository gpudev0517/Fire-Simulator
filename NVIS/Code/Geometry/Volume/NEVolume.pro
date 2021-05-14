
ROOTPATH = $$PWD/../../..

include(../QMake/common.pri)

QT      +=  core widgets

#message($$ROOTPATH)


include($$ROOTPATH/QMake/utility.pri)
include($$ROOTPATH/QMake/install.pri)
include($$ROOTPATH/QMake/config.pri)

TARGET = NEVolume


TEMPLATE = lib

LIBS += -lNECore


COREPATH = $$ROOTPATH/Code/Core
include(QMake/volume.pri)
