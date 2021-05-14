TEMPLATE = subdirs

SUBDIRS = \
Plugins \
UI \
App

CONFIG += ordered recursive


App.depends = Plugins UI

Plugins.file = Plugins/FDYNPlugins_OUI.pro
UI.file   = UI/FDYNUI.pro
App.file  = AppMain/FDYNAppMain_OUI.pro




