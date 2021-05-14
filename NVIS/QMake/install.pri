DATAPATH=$$ROOTPATH/../../Data
CONFIG(release, debug|release) {
    APIPATH=$$ROOTPATH/../FDYN/NDYN
} else {
    APIPATH=$$ROOTPATH/../FDYN/NDYN_Debug
}

INSTALL_PREFIX=$$APIPATH
INSTALL_INC_PREFIX=$$APIPATH/include
INSTALL_BINARY_PREFIX=$$APIPATH/bin
INSTALL_LIB_PREFIX=$$APIPATH/lib

INSTALL_CODE_ROOT = $$ROOTPATH/Code

#
# Library Ext
#
DLIBEXT = so
win32:DLIBEXT = dll
macx:DLIBEXT = so

SLIBEXT = a
win32:SLIBEXT = lib
macx:SLIBEXT = a


BUILD_CONF = Debug
CONFIG(release, debug|release) {
    BUILD_CONF = Release
} else {
    BUILD_CONF = Debug
}




#
# Installs files
#
defineTest(copyInstall) {
    H = $$1
    path = $$3/$$relative_path($$H, $$2)
    filename = $$basename(H)
    message(HERE: $$path)

    eval(headers_$${filename}.files += $$clean_path($$H))
    eval(headers_$${filename}.path = $$clean_path($$path))
    eval(INSTALLS *= headers_$${filename})


    export(INSTALLS)
}




#
# Following might not be needed during installation
#

#CONFIG(release, debug|release) {
#    INSTALL_EXTERNAL_LIB_PREFIX = $$DATAPATH/Bin/Release
#}

#CONFIG(debug, debug|release) {
#    INSTALL_EXTERNAL_LIB_PREFIX = $$DATAPATH/Bin/Debug
#}

#INSTALL_EXTERNAL_LIB = \
#    $$files($$ROOT_EXTERNAL_PATH/$$PLATFORM/lib/*, true)


#for(scenes, INSTALL_EXTERNAL_LIB) {
#    path = $${INSTALL_EXTERNAL_LIB_PREFIX}/$$basename($$scenes)
#    filename = $$relative_path($$scenes, $$ROOTPATH)
#    eval(scenes_$${filename}.files += $$scenes)
#    eval(scenes_$${filename}.path = $$path)
#    eval(INSTALLS *= scenes_$${filename})
#}


#CONFIG(release, debug|release) {
#    INSTALL_LIB_PREFIX = $$DATAPATH/Bin/Release
#    INSTALL_LIB = $$files($ROOT_EXTERNAL_PATH/Build/Release/*, true)
#}

#CONFIG(debug, debug|release) {
#    INSTALL_LIB_PREFIX = $$DATAPATH/Bin/Debug
#    INSTALL_LIB = $$ROOT_EXTERNAL_PATH/Build/Debug/*
#}

#for(scenes, INSTALL_LIB) {
#	path = $${INSTALL_LIB_PREFIX}/$$basename($$scenes)
#	filename = $$relative_path($$scenes,$$ROOTPATH)
#	eval(scenes_$${filename}.files += $$scenes)
#	eval(scenes_$${filename}.path = $$path)
#	eval(INSTALLS *= scenes_$${filename})
#}

#
# Sample Plugins
#

#INSTALL_SAMPLES_PREFIX = ../../Data/Plugins/
#INSTALL_SAMPLES = \
#    Setups/Samples/Plugins

#for(samples, INSTALL_SAMPLES) {
#  path = $${INSTALL_SAMPLES_PREFIX}/$${dirname($samples)}
#  pathsamples = $$replace(path, "Plugins", "")
#  eval(samples_$${path}.files += $$samples)
#  eval(samples_$${path}.path = $$pathsamples)
#  eval(INSTALLS *= samples_$${path})
#}


#INSTALL_SCENES_PREFIX = ../../Data/NEScenes/
#INSTALL_SCENES = \
#    Setups/Samples/NEScenes

#for(scenes, INSTALL_SCENES) {
#  path = $${INSTALL_SCENES_PREFIX}
#  pathscenes = $$replace(path, "NEScenes", "")
#  eval(scenes_$${path}.files += $$scenes)
#  eval(scenes_$${path}.path = $$pathscenes)
#  eval(INSTALLS *= scenes_$${path})
#}
