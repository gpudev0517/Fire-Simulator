# Builds the NEUI module

include(../QMake/common.pri)
UIPATH = $$ROOTPATH/Code/FDYNUI

TARGET   = FDYNUI
TEMPLATE = lib

QT += core widgets network printsupport opengl testlib
QT += quick
QT += quickcontrols2
QT += qml
QT += widgets
QT += quickwidgets

CONFIG += c++11
CONFIG += precompile_header shared

#DEFINES += OLD_PROPERTY
#DEFINES += OLD_PROPERTY_MENU

DEFINES += FDYN_FDYNUI_BUILD
PRECOMPILED_HEADER = PCH_UI.h


INCLUDEPATH += \
	$$UIPATH \
        $$NDYNPATH/include \
        $$NDYNPATH/include/Geometry \
        $$NDYNPATH/include/Physics \
        $$NDYNPATH/include/Render \
        $$NDYNPATH/include/GLI \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/include/UIUtilities \
        $$NDYNPATH/include/UIProperties \
        $$NDYNPATH/External/include \
        $$NDYNPATH/External/include\python27 \
        $$NDYNPATH/External/include/PythonQt/src


INCLUDEPATH += \
        $$ROOTPATH/Code/Plugins



QMAKE_LIBDIR += $$OUT_PWD
QMAKE_LIBDIR += $$NDYNPATH/lib

LIBS += -lNECore \
        -lNEGeometry \
        -lNEPhysics \
        -lNERender \
        -lNEGLI \
        -lNEUIUtilities \
        -lNEUIProperties \
        -lFEngine \
        -lgizmo


# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}

win32 {
    QMAKE_LIBDIR += $$NDYNPATH/External/windows/lib


    if($$RELEASE) {
            LIBS += -lPythonQt
    } else {
            LIBS += -lPythonQt_d
    }
#    LIBS += -lPythonQt
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


#SOURCES += \
#    Managers/FDYNGUIManager.cpp \
#    Utilities/FDYNUIApplication.cpp \
#    Utilities/FDYNTreeModel.cpp \
#    Windows/Main/FDYNMainWindow.cpp \
#    Widgets/Timeline/FDYNTimeline.cpp \
#    Widgets/Timeline/FDYNTimelineWidget.cpp \
#    Windows/SceneInspector/FDYNSceneInspector.cpp \
#    Windows/SceneInspector/FDYNSceneInspectorWindow.cpp \
#    Windows/ZoneInspector/FDYNZoneInspector.cpp \
#    Windows/ZoneInspector/FDYNZoneInspectorWindow.cpp


#HEADERS  += \
#    Managers/FDYNGUIManager.h \
#    Managers/FDYNGUIManager_p.h \
#    Utilities/FDYNUIApplication.h \
#    Utilities/FDYNTreeModel.h \
#    Utilities/FDYNTreeItem.h \
#    Windows/Main/FDYNMainWindow.h \
#    Widgets/Timeline/FDYNTimeline.h \
#    Widgets/Timeline/FDYNTimelineWidget.h \
#    Windows/ZoneInspector/FDYNZoneInspector.h \
#    Windows/ZoneInspector/FDYNZoneInspectorWindow.h \
#    Windows/SystemInspector/FDYNSystemInspector.h \
#    Windows/SystemInspector/FDYNSystemInspectorWindow.h \
#    globals_UI.h \
#    PCH_UI.h


SOURCES += \
    Dialogs/About/FDYNAboutDialog.cpp \
    Dialogs/File/FDYNFileDialog.cpp \
    Dialogs/MaterialSelector/FDYNHRRCurveWidget.cpp \
    Dialogs/Process/FDYNProcessDialog.cpp \
    Dialogs/Project/FDYNCreateProjectDialog.cpp \
    Dialogs/Settings/FDYNSettingsDialog.cpp \
    Dialogs/FRANXCalc/FDYNFRANXCalcDialog.cpp \
    Dialogs/PropertyShortCut/FDYNPropertyShortCut.cpp \
    Dialogs/MaterialSelector/FDYNSurfaceMaterialDialog.cpp \
    Dialogs/MaterialSelector/FDYNFireMaterialDialog.cpp \
    Dialogs/MaterialSelector/FDYNFireAddT2Dialog.cpp \
    Dialogs/CriteriaSelector/FDYNFailureCriteriaDialog.cpp \
    Dialogs/VentCriteriaSelector/FDYNVentCriteriaDialog.cpp \
    Dialogs/VentCriteriaSelector/FDYNAddVentValuesDialog.cpp \
    Dialogs/VentCriteriaSelector/FDYNVentValueCurveWidget.cpp \
    Dialogs/CableSelector/FDYNCableDialog.cpp \
    Dialogs/SimulateScenario/FDYNSimulateScenarioDialog.cpp \
    Managers/FDYNGUIManager.cpp \
    Utilities/FDYNUIApplication.cpp \
    Widgets/Dock/FDYNDockWidget.cpp \
    Widgets/Dock/FDYNTitleBarWidget.cpp \
    Widgets/MenuBar/FDYNMenuBar.cpp \
    Widgets/Menu/FDYNMenu.cpp \
    Widgets/Timeline/FDYNTimeline.cpp \
    Widgets/CADModeling/FDYNCADModelingTool.cpp \
    Windows/ExternalRendererSettings/FDYNExternalRendererSettingsWindow.cpp \
    Windows/PropertyManager/fileedit.cpp \
    Windows/PropertyManager/FDYNObjectController.cpp \
    Windows/PropertyManager/FDYNPropertyEditorWindow.cpp \
    Windows/PropertyManager/FDYNVariantFactory.cpp \
    Windows/PropertyManager/FDYNVariantManager.cpp \
    Windows/PropertyManager/qttreepropertybrowser.cpp \
    Windows/SceneInspector/FDYNSceneInspector.cpp \
    Windows/SceneInspector/FDYNSceneInspectorWindow.cpp \
    Windows/ZoneInspector/FDYNZoneInspector.cpp \
    Windows/ZoneInspector/FDYNZoneInspectorWindow.cpp \
    Windows/ScenarioInspector/FDYNScenarioInspector.cpp \
    Windows/ScenarioInspector/FDYNScenarioInspectorWindow.cpp \
    Windows/FRANXInspector/FDYNFRANXInspector.cpp \
    Windows/FRANXInspector/FDYNFRANXInspectorWindow.cpp \
    Windows/SystemInspector/FDYNSystemInspector.cpp \
    Windows/SystemInspector/FDYNSystemInspectorWindow.cpp \
    Windows/Main/FDYNMainWindow.cpp \
    Windows/GLMain/FDYNGLMainWindow.cpp \
    Windows/Message/FDYNMessageWindow.cpp \
    Utilities/FDYNTreeModel.cpp \
    Utilities/FDYNTreeCustomType.cpp \
    Utilities/FDYNTreeItem.cpp \
    Utilities/FDYNTreeWidget.cpp \
    Utilities/FDYNPropertyWidget.cpp \
    Utilities/FDYNPropertyItem.cpp \
    Utilities/FDYNPropertyModel.cpp \
    Utilities/FDYNPropertyCustomType.cpp


HEADERS  += \
    Dialogs/About/FDYNAboutDialog.h \
    Dialogs/File/FDYNFileDialog.h \
    Dialogs/MaterialSelector/FDYNHRRCurveWidget.h \
    Dialogs/Process/FDYNProcessDialog.h \
    Dialogs/Project/FDYNCreateProjectDialog.h \
    Dialogs/Settings/FDYNSettingsDialog.h \
    Dialogs/FRANXCalc/FDYNFRANXCalcDialog.h \
    Dialogs/PropertyShortCut/FDYNPropertyShortCut.h \
    Dialogs/MaterialSelector/FDYNSurfaceMaterialDialog.h \
    Dialogs/CriteriaSelector/FDYNFailureCriteriaDialog.h \
    Dialogs/VentCriteriaSelector/FDYNVentCriteriaDialog.h \
    Dialogs/VentCriteriaSelector/FDYNAddVentValuesDialog.h \
    Dialogs/VentCriteriaSelector/FDYNVentValueCurveWidget.h \
    Dialogs/CableSelector/FDYNCableDialog.h \
    Dialogs/MaterialSelector/FDYNFireMaterialDialog.h \
    Dialogs/MaterialSelector/FDYNFireAddT2Dialog.h \
    Dialogs/SimulateScenario/FDYNSimulateScenarioDialog.h \
    Managers/FDYNGUIManager.h \
    Managers/FDYNGUIManager_p.h \
    Utilities/FDYNUIApplication.h \
    Widgets/Dock/FDYNDockWidget.h \
    Widgets/Dock/FDYNTitleBarWidget.h \
    Widgets/MenuBar/FDYNMenuBar.h \
    Widgets/Menu/FDYNMenu.h \
    Widgets/Timeline/FDYNTimeline.h \
    Widgets/CADModeling/FDYNCADModelingTool.h \
    Windows/ExternalRendererSettings/FDYNExternalRendererSettingsWindow.h \
    Windows/PropertyManager/fileedit.h \
    Windows/PropertyManager/FDYNObjectController.h \
    Windows/PropertyManager/FDYNObjectController_p.h \
    Windows/PropertyManager/FDYNPropertyEditorWindow.h \
    Windows/PropertyManager/FDYNVariantFactory.h \
    Windows/PropertyManager/FDYNVariantManager.h \
    Windows/PropertyManager/qttreepropertybrowser.h \
    Windows/SceneInspector/FDYNSceneInspector.h \
    Windows/SceneInspector/FDYNSceneInspectorItem.h \
    Windows/SceneInspector/FDYNSceneInspectorWindow.h \
    Windows/ZoneInspector/FDYNZoneInspector.h \
    Windows/ZoneInspector/FDYNZoneInspectorWindow.h \
    Windows/ZoneInspector/FDYNZoneInspectorItem.h \
    Windows/ScenarioInspector/FDYNScenarioInspector.h \
    Windows/ScenarioInspector/FDYNScenarioInspectorWindow.h \
    Windows/ScenarioInspector/FDYNScenarioInspectorItem.h \
    Windows/FRANXInspector/FDYNFRANXInspector.h \
    Windows/FRANXInspector/FDYNFRANXInspectorWindow.h \
    Windows/FRANXInspector/FDYNFRANXInspectorItem.h \
    Windows/SystemInspector/FDYNSystemInspector.h \
    Windows/SystemInspector/FDYNSystemInspectorItem.h \
    Windows/SystemInspector/FDYNSystemInspectorWindow.h \
    Windows/Main/FDYNMainWindow.h \
    Windows/GLMain/FDYNGLMainWindow.h \
    Windows/Message/FDYNMessageWindow.h \
    globals_UI.h \
    PCH_UI.h \
    Utilities/FDYNTreeModel.h \
    Utilities/FDYNTreeItem.h \
    Utilities/FDYNTreeCustomType.h \
    Utilities/FDYNTreeWidget.h \
    Utilities/FDYNPropertyWidget.h \
    Utilities/FDYNPropertyItem.h \
    Utilities/FDYNPropertyModel.h \
    Utilities/FDYNPropertyCustomType.h

FORMS += \
    Dialogs/About/FDYNAboutDialog.ui \
    Dialogs/Project/FDYNCreateProjectDialog.ui \
    Dialogs/Settings/FDYNSettingsDialog.ui \
    Dialogs/FRANXCalc/FDYNFRANXCalcDialog.ui \
    Widgets/Dock/FDYNTitleBarWidget.ui \
    Windows/ExternalRendererSettings/FDYNExternalRendererSettingsWindow.ui \
    Windows/PropertyManager/FDYNPropertyEditorWindow.ui \
    Windows/Message/FDYNMessageWindow.ui \
    Dialogs/Process/FDYNProcessDialog.ui \
    Dialogs/PropertyShortCut/FDYNPropertyShortCut.ui \
    Dialogs/MaterialSelector/FDYNSurfaceMaterialDialog.ui \
    Dialogs/CriteriaSelector/FDYNFailureCriteriaDialog.ui \
    Dialogs/VentCriteriaSelector/FDYNVentCriteriaDialog.ui \
    Dialogs/VentCriteriaSelector/FDYNAddVentValuesDialog.ui \
    Dialogs/CableSelector/FDYNCableDialog.ui \
    Dialogs/MaterialSelector/FDYNFireMaterialDialog.ui \
    Dialogs/MaterialSelector/FDYNFireAddT2Dialog.ui \
    Dialogs/SimulateScenario/FDYNSimulateScenarioDialog.ui \

RESOURCES += \
    FDYNUI.qrc
