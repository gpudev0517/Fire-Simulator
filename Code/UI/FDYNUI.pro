# Builds the NEUI module

include(../QMake/common.pri)
UIPATH = $$ROOTPATH/Code/UI
NDYNPATH = $$ROOTPATH/NDYN

TARGET   = FDYNUI
TEMPLATE = lib
QT      += core widgets network printsupport opengl testlib
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NEUI_BUILD
PRECOMPILED_HEADER = PCH_UI.h


INCLUDEPATH += \
	$$UIPATH \
        $$NDYNPATH/include \
        $$NDYNPATH/include/Geometry \
        $$NDYNPATH/include/Render \
        $$NDYNPATH/include/GLI \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/External/include \
        $$NDYNPATH/External/include\python27

INCLUDEPATH += \
        $$ROOTPATH/Code/Plugins



QMAKE_LIBDIR += $$OUT_PWD
QMAKE_LIBDIR += $$NDYNPATH/lib
LIBS += -lNECore \
        -lNEGeometry \
        -lNERender \
        -lNEGLI \
        -lgizmo


# Output Configuration

if($$RELEASE) {
        DESTDIR = $$ROOTPATH/Build_OUI/Release
} else {
        DESTDIR = $$ROOTPATH/Build_OUI/Debug
}

win32 {
    QMAKE_LIBDIR += $$NDYNPATH/External/windows/lib


    if($$RELEASE) {
            LIBS += -lPythonQt
    } else {
            LIBS += -lPythonQt_d
    }
    #LIBS += -lPythonQt
}

unix:!macx {
    QMAKE_LIBDIR += $$NDYNPATH/External/Linux/lib

    LIBS += -lftgl
    LIBS += -lfreetype
    LIBS += -lPythonQt
    LIBS += -lassimp
}

macx {
    QMAKE_LIBDIR += $$NDYNPATH/External/macosx/lib

    LIBS += -lftgl
    LIBS += -lfreetype
    LIBS += -lPythonQt
    LIBS += -lassimp
}


SOURCES += \
    Dialogs/About/NEAboutDialog.cpp \
    Dialogs/Abstract/NEAbstractDialog.cpp \
    Dialogs/ColorPreset/NEColorPresetDialog.cpp \
    Dialogs/Curve/NECurveDialog.cpp \
    Dialogs/File/NEFileDialog.cpp \
    Dialogs/Help/NEHelpEngine.cpp \
    Dialogs/Process/NEProcessDialog.cpp \
    Dialogs/SceneManagement/NESceneManagementDialog.cpp \
    Managers/NEGUIManager.cpp \
    Dialogs/Curve/NECurveScene.cpp \
    Dialogs/Curve/NECurveView.cpp \
    Dialogs/Curve/NEEnumCurveScene.cpp \
    Dialogs/Curve/NEKey.cpp \
    Utilities/ganalytics.cpp \
    Utilities/NEGUIStyle.cpp \
    Utilities/NELogBrowser.cpp \
    Utilities/NEUIApplication.cpp \
    Widgets/Dock/NEDockWidget.cpp \
    Widgets/Dock/NETitlebarWidget.cpp \
    Widgets/LineNumberArea/NELineNumberArea.cpp \
    Widgets/Menu/NEMenu.cpp \
    Widgets/MenuBar/NEMenuBar.cpp \
    Widgets/ShelfBar/NEShelfBar.cpp \
    Widgets/Shortcut/NEShortcutWidget.cpp \
    Widgets/Timeline/NETimeline.cpp \
    Widgets/Timeline/NETimelineWidget.cpp \
    Windows/GLMain/NEGLMainWindow.cpp \
    Windows/KeyframeInspector/NEKeyframeInspector.cpp \
    Windows/Main/NEMainWindow.cpp \
    Windows/Message/NEMessageWindow.cpp \
    Windows/PropertyManager/fileedit.cpp \
    Windows/PropertyManager/NEColorRampEditor.cpp \
    Windows/PropertyManager/NEDoubleSpinBox.cpp \
    Windows/PropertyManager/NEObjectController.cpp \
    Windows/PropertyManager/NEPropertyEditor.cpp \
    Windows/PropertyManager/NEPropertyEditorWindow.cpp \
    Windows/PropertyManager/NEVariantFactory.cpp \
    Windows/PropertyManager/NEVariantManager.cpp \
    Windows/PropertyManager/qtbuttonpropertybrowser.cpp \
    Windows/PropertyManager/qteditorfactory.cpp \
    Windows/PropertyManager/qtgroupboxpropertybrowser.cpp \
    Windows/PropertyManager/qtpropertybrowser.cpp \
    Windows/PropertyManager/qtpropertybrowserutils.cpp \
    Windows/PropertyManager/qtpropertymanager.cpp \
    Windows/PropertyManager/qttreepropertybrowser.cpp \
    Windows/PropertyManager/qtvariantproperty.cpp \
    Windows/SceneBuilder/NESceneBuilderWindow.cpp \
    Windows/SceneInspector/NESceneInspector.cpp \
    Windows/SceneInspector/NESceneInspectorWindow.cpp \
    Windows/FZoneInspector/FZoneInspector.cpp \
    Windows/FZoneInspector/FZoneInspectorWindow.cpp \
    Windows/SystemInspector/NESystemInspector.cpp \
    Windows/SystemInspector/NESystemInspectorWindow.cpp


HEADERS  += \
    Dialogs/About/NEAboutDialog.h \
    Dialogs/Abstract/NEAbstractDialog.h \
    Dialogs/ColorPreset/NEColorPresetDialog.h \
    Dialogs/Curve/NECurveDialog.h \
    Dialogs/File/NEFileDialog.h \
    Dialogs/Help/NEHelpEngine.h \
    Dialogs/Process/NEProcessDialog.h \
    Dialogs/SceneManagement/NESceneManagementDialog.h \
    Managers/NEGUIManager.h \
    Managers/NEGUIManager_p.h \
    Dialogs/Curve/NECurveScene.h \
    Dialogs/Curve/NECurveView.h \
    Dialogs/Curve/NEEnumCurveScene.h \
    Dialogs/Curve/NEKey.h \
    Utilities/ganalytics.h \
    Utilities/NEGUIStyle.h \
    Utilities/NELogBrowser.h \
    Utilities/NEUIApplication.h \
    Widgets/Dock/NEDockWidget.h \
    Widgets/Dock/NETitlebarWidget.h \
    Widgets/LineNumberArea/NELineNumberArea.h \
    Widgets/Menu/NEMenu.h \
    Widgets/MenuBar/NEMenuBar.h \
    Widgets/ShelfBar/NEShelfBar.h \
    Widgets/Shortcut/NEShortcutWidget.h \
    Widgets/Timeline/NETimeline.h \
    Widgets/Timeline/NETimelineWidget.h \
    Windows/GLMain/NEGLMainWindow.h \
    Windows/KeyframeInspector/NEKeyframeInspector.h \
    Windows/Main/NEMainWindow.h \
    Windows/Message/NEMessageWindow.h \
    Windows/PropertyManager/fileedit.h \
    Windows/PropertyManager/NEColorRampEditor.h \
    Windows/PropertyManager/NEDoubleSpinBox.h \
    Windows/PropertyManager/NEObjectController.h \
    Windows/PropertyManager/NEObjectController_p.h \
    Windows/PropertyManager/NEPropertyEditor.h \
    Windows/PropertyManager/NEPropertyEditorWindow.h \
    Windows/PropertyManager/NEVariantFactory.h \
    Windows/PropertyManager/NEVariantManager.h \
    Windows/PropertyManager/qtbuttonpropertybrowser.h \
    Windows/PropertyManager/qteditorfactory.h \
    Windows/PropertyManager/qtgroupboxpropertybrowser.h \
    Windows/PropertyManager/qtpropertybrowser.h \
    Windows/PropertyManager/qtpropertybrowserutils_p.h \
    Windows/PropertyManager/qtpropertymanager.h \
    Windows/PropertyManager/qttreepropertybrowser.h \
    Windows/PropertyManager/qtvariantproperty.h \
    Windows/SceneBuilder/NESceneBuilderWindow.h \
    Windows/SceneInspector/NESceneInspector.h \
    Windows/SceneInspector/NESceneInspectorWindow.h \
    Windows/FZoneInspector/FZoneInspector.h \
    Windows/FZoneInspector/FZoneInspectorWindow.h \
    Windows/SystemInspector/NESystemInspector.h \
    Windows/SystemInspector/NESystemInspectorWindow.h \
    globals_UI.h \
    PCH_UI.h

FORMS    += \
    Dialogs/About/NEAboutDialog.ui \
    Dialogs/Curve/NECurveDialog.ui \
    Dialogs/Process/NEProcessDialog.ui \
    Dialogs/SceneManagement/NESceneManagementDialog.ui \
    Widgets/Dock/NETitlebarWidget.ui \
    Widgets/Shortcut/NEShortcutWidget.ui \
    Widgets/Timeline/NETimeline.ui \
    Windows/GLMain/NEGLMainWindow.ui \
    Windows/KeyframeInspector/NEKeyframeInspector.ui \
    Windows/Message/NEMessageWindow.ui \
    Windows/PropertyManager/NEPropertyEditorWindow.ui \
    Windows/SceneBuilder/NESceneBuilderWindow.ui \
    Windows/SceneInspector/NESceneInspectorWindow.ui \
    Windows/FZoneInspector/FZoneInspectorWindow.ui \
    Windows/SystemInspector/NESystemInspectorWindow.ui \
    FDYN.ui


RESOURCES += \
    Windows/PropertyManager/qtpropertybrowser.qrc \
    FDYN.qrc
