TEMPLATE = subdirs

SUBDIRS = \
UI \
Plugins \
App

CONFIG += ordered recursive

App.depends = UI Plugins
UI.depends = Plugins

Plugins.file = Plugins/FDYNPlugins.pro
UI.file   = FDYNUI/FDYNUI.pro
App.file  = AppMain/FDYNAppMain.pro
