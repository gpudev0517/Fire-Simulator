# Builds the NEUI module

include(../QMake/common.pri)
UIPATH = $$ROOTPATH/Code/UI
#include($$ROOTPATH/QMake/utility.pri)
#include($$ROOTPATH/QMake/install.pri)

TARGET   = NEUI
TEMPLATE = lib
QT      += core widgets network printsupport opengl testlib
CONFIG += precompile_header shared
DEFINES += NEUTRINO_NEUI_BUILD
PRECOMPILED_HEADER = PCH_UI.h

INCLUDEPATH += \
        $$ROOTPATH/Code \
        $$ROOTPATH/Code/Core \
        $$ROOTPATH/Code/Physics \
        $$ROOTPATH/Code/Geometry \
        $$ROOTPATH/Code/Render \
        $$ROOTPATH/Code/GLI \
        $$ROOTPATH/Code/PostProcessor \
        $$ROOTPATH/Code/RenderNEO \
        $$ROOTPATH/Code/UIUtilities \
        $$ROOTPATH/Code/UIProperties \
	$$ROOTPATH/External \
        $$ROOTPATH/External/include \
        $$ROOTPATH/External/PythonQt/src


INCLUDEPATH += \
        $$NDYNPATH/include/Core \
        $$NDYNPATH/include/Physics \
        $$NDYNPATH/include/PostProcessor

QMAKE_LIBDIR += $$OUT_PWD
LIBS += -lNECore \
        -lNEPhysics \
        -lNEGeometry \
        -lNERender \
        -lNEGLI \
        -lNERenderNEO \
        -lgizmo \
        -lNEUIUtilities \
        -lNEUIProperties


# Output Configuration

if($$RELEASE) {
	DESTDIR = $$ROOTPATH/Build/Release
} else {
	DESTDIR = $$ROOTPATH/Build/Debug
}

win32 {
	if($$RELEASE) {
		LIBS += -lPythonQt
	} else {
		LIBS += -lPythonQt_d
	}
        LIBS += -L$$ROOTPATH/External/Windows/lib
        LIBS += -lftgl
        LIBS += -lassimp-vc140-mt
}

unix:!macx {
	LIBS += -lftgl
	LIBS += -lfreetype
        LIBS += -lassimp
        if($$RELEASE) {
                LIBS += -lPythonQt
        } else {
                LIBS += -lPythonQt_d
        }
}

macx {
	LIBS += -lftgl
	LIBS += -lfreetype
	LIBS += -lPythonQt
        LIBS += -lassimp
}



win32 {
	INCLUDEPATH += \
		$$ROOTPATH/External/Windows/include \
                $$ROOTPATH/External/Windows/include/FFMpeg \
                $$ROOTPATH/External/Windows/include/python27 \
	INCLUDEPATH += $$ROOTPATH/External/Windows/include/Freetype2
}

macx {
	INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
}

unix:!macx {
    INCLUDEPATH += \
                $$ROOTPATH/External/Linux/include \
                $$ROOTPATH/External/Linux/FFMpeg \
                /usr/include/python2.7 \
                /usr/include/freetype2
}


SOURCES += \
    Dialogs/About/NEAboutDialog.cpp \
    Dialogs/Collider/NEColliderDialog.cpp \
    Dialogs/Help/NEHelpEngine.cpp \
    Dialogs/ImageViewer/NEImageViewer.cpp \
    #Dialogs/ParticleSection/NEParticleSectionDialog.cpp \
    #Dialogs/Plot/NECustomPlot.cpp \
    Dialogs/PluginInfo/NEPluginInfoDialog.cpp \
    Dialogs/Preferences/NEPreferencesDialog.cpp \
    Dialogs/Preferences/NEShelfItemEditor.cpp \
    Dialogs/Process/NEProcessDialog.cpp \
    Windows/PropertyManager/NEPropertyEditorWindow.cpp \
    Dialogs/PropertyShortCut/NEPropertyShortCut.cpp \
    Dialogs/SceneManagement/NESceneManagementDialog.cpp \
    #Dialogs/Section/NESectionWidget.cpp \
    Managers/NEGUIManager.cpp \
    NEUIApplication.cpp \
    Widgets/Dock/NEDockWidget.cpp \
    Widgets/Dock/NETitlebarWidget.cpp \
    Widgets/GPUBufferVis/NEGPUBufferVis.cpp \
    Widgets/LineNumberArea/NELineNumberArea.cpp \
    Widgets/Menu/NEMenu.cpp \
    Widgets/MenuBar/NEMenuBar.cpp \
    Widgets/PythonInput/PythonHighlighter/PythonHighlighter.cpp \
    Widgets/PythonInput/NEPythonInput.cpp \
    Widgets/ShelfBar/NEShelfBar.cpp \
    Widgets/Shortcut/NEShortcutWidget.cpp \
    Widgets/Timeline/NETimeline.cpp \
    Widgets/Timeline/NETimelineWidget.cpp \
    Windows/BufferVis/NEBufferVisualizer.cpp \
    Windows/BufferVis/NEBufferVisWindow.cpp \
    Windows/ChartsVis/NEChartsVisWindow.cpp \
    Windows/ConnectionInspector/NEConnectionInspector.cpp \
    Windows/ExpressionEditor/NEExpressionEditor.cpp \
    Windows/ExpressionEditor/NEExpressionEditorWindow.cpp \
    Windows/ExpressionInspector/NEExpressionInspector.cpp \
    Windows/ExternalRendererSettings/NEExternalRendererSettingsWindow.cpp \
    Windows/GLMain/NEGLMainWindow.cpp \
    Windows/KeyframeInspector/NEKeyframeInspector.cpp \
    Windows/Main/NEMainWindow.cpp \
    Windows/Message/NEMessageWindow.cpp \
    Windows/NodeGraphEditor/NEBlock.cpp \
    Windows/NodeGraphEditor/NEConnection.cpp \
    Windows/NodeGraphEditor/NEConnectionGEItem.cpp \
    Windows/NodeGraphEditor/NENodeGraphEditor.cpp \
    Windows/NodeGraphEditor/NENodeGraphEditorItem.cpp \
    Windows/NodeGraphEditor/NENodeGraphEditorScene.cpp \
    Windows/NodeGraphEditor/NENodeGraphEditorWindow.cpp \
    Windows/NodeGraphEditor/NEPort.cpp \
    Windows/NodeGraphEditor/NodesEditor.cpp \
    Windows/PluginManager/NEPluginManagerWindow.cpp \
    Windows/PluginManager/NEPluginsTreeWidget.cpp \
    Windows/SceneBuilder/NESceneBuilderWindow.cpp \
    Windows/SceneInspector/NESceneInspector.cpp \
    Windows/SceneInspector/NESceneInspectorWindow.cpp \
    Windows/ScriptEditor/NEConsoleWindow.cpp \
    Windows/SystemInspector/NESystemInspector.cpp \
    Windows/SystemInspector/NESystemInspectorWindow.cpp


HEADERS  += \
    Dialogs/About/NEAboutDialog.h \
    Dialogs/Collider/NEColliderDialog.h \
    Dialogs/Help/NEHelpEngine.h \
    Dialogs/ImageViewer/NEImageViewer.h \
    #Dialogs/ParticleSection/NEParticleSectionDialog.h \
    #Dialogs/Plot/NECustomPlot.h \
    Dialogs/PluginInfo/NEPluginInfoDialog.h \
    Dialogs/Preferences/NEPreferencesDialog.h \
    Dialogs/Preferences/NEShelfItemEditor.h \
    Dialogs/Process/NEProcessDialog.h \
    Windows/PropertyManager/NEPropertyEditorWindow.h \
    Dialogs/PropertyShortCut/NEPropertyShortCut.h \
    Dialogs/SceneManagement/NESceneManagementDialog.h \
    #Dialogs/Section/NESectionWidget.h \
    Managers/NEGUIManager.h \
    Managers/NEGUIManager_p.h \
    NEUIApplication.h \
    Widgets/Dock/NEDockWidget.h \
    Widgets/Dock/NETitlebarWidget.h \
    Widgets/GPUBufferVis/NEGPUBufferVis.h \
    Widgets/LineNumberArea/NELineNumberArea.h \
    Widgets/Menu/NEMenu.h \
    Widgets/MenuBar/NEMenuBar.h \
    Widgets/PythonInput/PythonHighlighter/PythonHighlighter.h \
    Widgets/PythonInput/NEPythonInput.h \
    Widgets/ShelfBar/NEShelfBar.h \
    Widgets/Shortcut/NEShortcutWidget.h \
    Widgets/Timeline/NETimeline.h \
    Widgets/Timeline/NETimelineWidget.h \
    Windows/BufferVis/NEBufferVisualizer.h \
    Windows/BufferVis/NEBufferVisWindow.h \
    Windows/ChartsVis/NEChartsVisWindow.h \
    Windows/ConnectionInspector/NEConnectionInspector.h \
    Windows/ExpressionEditor/NEExpressionEditor.h \
    Windows/ExpressionEditor/NEExpressionEditorWindow.h \
    Windows/ExpressionInspector/NEExpressionInspector.h \
    Windows/ExternalRendererSettings/NEExternalRendererSettingsWindow.h \
    Windows/GLMain/NEGLMainWindow.h \
    Windows/KeyframeInspector/NEKeyframeInspector.h \
    Windows/Main/NEMainWindow.h \
    Windows/Message/NEMessageWindow.h \
    Windows/NodeGraphEditor/NEBlock.h \
    Windows/NodeGraphEditor/NEConnection.h \
    Windows/NodeGraphEditor/NEConnectionGEItem.h \
    Windows/NodeGraphEditor/NENodeGraphEditor.h \
    Windows/NodeGraphEditor/NENodeGraphEditorItem.h \
    Windows/NodeGraphEditor/NENodeGraphEditorScene.h \
    Windows/NodeGraphEditor/NENodeGraphEditorWindow.h \
    Windows/NodeGraphEditor/NEPort.h \
    Windows/NodeGraphEditor/NodesEditor.h \
    Windows/PluginManager/NEPluginManagerWindow.h \
    Windows/PluginManager/NEPluginsTreeWidget.h \
    Windows/SceneBuilder/NESceneBuilderWindow.h \
    Windows/SceneInspector/NESceneInspector.h \
    Windows/SceneInspector/NESceneInspectorWindow.h \
    Windows/ScriptEditor/NEConsoleWindow.h \
    Windows/SystemInspector/NESystemInspector.h \
    Windows/SystemInspector/NESystemInspectorWindow.h \
    globals_UI.h \
    PCH_UI.h

FORMS    += \
    Dialogs/About/NEAboutDialog.ui \
    Dialogs/ParticleSection/NEParticleSectionDialog.ui \
    Dialogs/PluginInfo/NEPluginInfoDialog.ui \
    Dialogs/Preferences/NEPreferencesDialog.ui \
    Dialogs/Preferences/NEShelfItemEditor.ui \
    Dialogs/Process/NEProcessDialog.ui \
    Dialogs/SceneManagement/NESceneManagementDialog.ui \
    Widgets/Dock/NETitlebarWidget.ui \
    Widgets/Shortcut/NEShortcutWidget.ui \
    Widgets/Timeline/NETimeline.ui \
    Windows/BufferVis/NEBufferVisWindow.ui \
    Windows/ChartsVis/NEChartsVisWindow.ui \
    Windows/ConnectionInspector/NEConnectionInspector.ui \
    Windows/ExpressionEditor/NEExpressionEditorWindow.ui \
    Windows/ExpressionInspector/NEExpressionInspector.ui \
    Windows/ExternalRendererSettings/NEExternalRendererSettingsWindow.ui \
    Windows/GLMain/NEGLMainWindow.ui \
    Windows/KeyframeInspector/NEKeyframeInspector.ui \
    Windows/Message/NEMessageWindow.ui \
    Windows/NodeGraphEditor/NENodeGraphEditorWindow.ui \
    Windows/PluginManager/NEPluginManagerWindow.ui \
    Windows/SceneBuilder/NESceneBuilderWindow.ui \
    Windows/SceneInspector/NESceneInspectorWindow.ui \
    Windows/ScriptEditor/NEConsoleWindow.ui \
    Windows/SystemInspector/NESystemInspectorWindow.ui \
    neutrino.ui \
    Dialogs/PropertyShortCut/NEPropertyShortCut.ui

FORMS    += \
    Windows/PropertyManager/NEPropertyEditorWindow.ui

RESOURCES += \
    neutrino.qrc
