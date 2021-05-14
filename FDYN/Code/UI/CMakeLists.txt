cmake_minimum_required (VERSION 3.1.0)

set (PRODUCT_UI "NEUI" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

include(../cmake/common.cmake)
include(utility)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set ( CMAKE_AUTORCC ON )

set ( UIPATH  ${ROOTPATH}/Code/UI )

set ( PRECOMPILED_SOURCE ${ROOTPATH}/Code/UI/PCH_UI.cpp )
set ( PRECOMPILED_HEADER PCH_UI.h )

add_definitions ( -DNEUTRINO_NEUI_BUILD  )

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5PrintSupport REQUIRED )
find_package( Qt5OpenGL REQUIRED )
find_package( Qt5Test REQUIRED )

include_directories ( 
	${UIPATH} 
	${ROOTPATH}/Code 
	${ROOTPATH}/Code/ExtraSources 
	${ROOTPATH}/Code/Core 
	${ROOTPATH}/External 
	${ROOTPATH}/External/include
)
if( WIN32 AND MSVC )
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NECore.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NECore.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEGeometry.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEGeometry.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NERendering.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NERendering.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NERigids.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NERigids.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/gizmo.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/gizmo.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/PythonQt.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/PythonQt.lib)

	link_libraries(GLU32.lib OpenGL32.lib glew32.lib ftgl.lib assimp-vc140-mt.lib)
else()
	# insert for other platform
endif()

if( UNIX AND NOT APPLE ) 
	# LIBS += -lGLU
	# LIBS += -lGLEW
	# LIBS += -lftgl
	# LIBS += -lfreetype
	# LIBS += -lPythonQt
    #     LIBS += -lassimp
endif()

if(APPLE )
	# LIBS += -lGLU
	# LIBS += -lglew
	# LIBS += -lftgl
	# LIBS += -lfreetype
	# LIBS += -lPythonQt
    #     LIBS += -lassimp
endif()

if( WIN32 AND MSVC )
	include_directories(
		${ROOTPATH}/External/Windows/include 
		${ROOTPATH}/External/Windows/include/assimp 
		${ROOTPATH}/External/Windows/include/python27 
		${ROOTPATH}/External/Windows/include/Freetype2
	)
endif()

if(APPLE)
	# INCLUDEPATH += $$ROOTPATH/External/macosx/include/freetype2
endif()

if( UNIX AND NOT APPLE )
	# INCLUDEPATH += $$ROOTPATH/External/Linux/include/freetype2
endif()



set ( UI_SOURCES
	${UIPATH}/NEUIApplication.cpp 
	${UIPATH}/Dialogs/NEAboutDialog.cpp 
	${UIPATH}/Dialogs/NEAbstractDialog.cpp 
	${UIPATH}/Dialogs/NEColliderDialog.cpp 
	${UIPATH}/Dialogs/NECurveDialog.cpp 
	${UIPATH}/Dialogs/NEColorPresetDialog.cpp 
	${UIPATH}/Dialogs/NEFileDialog.cpp 
	${UIPATH}/Dialogs/NEGPUBufferVis.cpp 
	${UIPATH}/Dialogs/NEHelpEngine.cpp 
	${UIPATH}/Dialogs/NEImageViewer.cpp 
	${UIPATH}/Dialogs/NEParticleSectionDialog.cpp 
	${UIPATH}/Dialogs/NEPluginInfoDialog.cpp 
	${UIPATH}/Dialogs/NEProcessDialog.cpp 
	${UIPATH}/Dialogs/NESceneManagementDialog.cpp 
	${UIPATH}/Dialogs/NESectionWidget.cpp 
	${UIPATH}/Dialogs/Plot/NECustomPlot.cpp 
	${UIPATH}/Manager/NEGUIManager.cpp 
	${UIPATH}/PropertyManager/fileedit.cpp 
	${UIPATH}/PropertyManager/NEDoubleSpinBox.cpp 
	${UIPATH}/PropertyManager/NEObjectController.cpp 
	${UIPATH}/PropertyManager/qtbuttonpropertybrowser.cpp 
	${UIPATH}/PropertyManager/qtgroupboxpropertybrowser.cpp 
	${UIPATH}/PropertyManager/qtpropertybrowserutils.cpp 
	${UIPATH}/PropertyManager/qttreepropertybrowser.cpp 
	${UIPATH}/PropertyManager/NEVariantFactory.cpp 
	${UIPATH}/PropertyManager/NEVariantManager.cpp 
	${UIPATH}/PropertyManager/qteditorfactory.cpp 
	${UIPATH}/PropertyManager/qtpropertybrowser.cpp 
	${UIPATH}/PropertyManager/qtpropertymanager.cpp 
	${UIPATH}/PropertyManager/qtvariantproperty.cpp 
	${UIPATH}/NEBufferVisualizer.cpp 
	${UIPATH}/NEBufferVisWindow.cpp 
	${UIPATH}/NEChartsVisWindow.cpp 
	${UIPATH}/NEConnectionGEItem.cpp 
	${UIPATH}/NEConnectionInspector.cpp 
	${UIPATH}/NEConsoleWindow.cpp 
	${UIPATH}/NEDockWidget.cpp 
	${UIPATH}/NEColorRampEditor.cpp 
	${UIPATH}/NEExpressionEditor.cpp 
	${UIPATH}/NEExpressionEditorWindow.cpp 
	${UIPATH}/NEExpressionInspector.cpp 
	${UIPATH}/NEExternalRendererSettingsWindow.cpp 
	${UIPATH}/NEGLMainWindow.cpp 
	${UIPATH}/NEGLWidget.cpp 
	${UIPATH}/NEGUIStyle.cpp 
	${UIPATH}/NEKeyframeInspector.cpp 
	${UIPATH}/NELineNumberArea.cpp 
	${UIPATH}/NELogBrowser.cpp 
	${UIPATH}/NEMainWindow.cpp 
	${UIPATH}/NEMenu.cpp 
	${UIPATH}/NEMenuBar.cpp 
	${UIPATH}/NEMessageWindow.cpp 
	${UIPATH}/NENodeGraphEditor.cpp 
	${UIPATH}/NENodeGraphEditorItem.cpp 
	${UIPATH}/NENodeGraphEditorScene.cpp 
	${UIPATH}/NENodeGraphEditorWindow.cpp 
	${UIPATH}/NEPluginManagerWindow.cpp 
	${UIPATH}/NEPluginsTreeWidget.cpp 
	${UIPATH}/NEPreferencesDialog.cpp 
	${UIPATH}/NEPropertyEditor.cpp 
	${UIPATH}/NEPropertyEditorWindow.cpp 
	${UIPATH}/NEPythonInput.cpp 
	${UIPATH}/NESceneBuilderWindow.cpp 
	${UIPATH}/NESceneInspector.cpp 
	${UIPATH}/NESceneInspectorWindow.cpp 
	${UIPATH}/NEShelfBar.cpp 
	${UIPATH}/NEShelfItemEditor.cpp 
	${UIPATH}/NESystemInspector.cpp 
	${UIPATH}/NESystemInspectorWindow.cpp 
	${UIPATH}/NETimeline.cpp 
	${UIPATH}/NETimelineWidget.cpp 
	${UIPATH}/NETitlebarWidget.cpp 
	${UIPATH}/PythonHighlighter/PythonHighlighter.cpp 
        ${UIPATH}/Utility/ganalytics.cpp 
	${UIPATH}/Utility/Curves/NECurveView.cpp 
	${UIPATH}/Utility/Curves/NECurveScene.cpp 
	${UIPATH}/Utility/Curves/NEEnumCurveScene.cpp 
        ${UIPATH}/Utility/Curves/NEKey.cpp 
        ${UIPATH}/NEBlock.cpp 
        ${UIPATH}/NEConnection.cpp 
        ${UIPATH}/NEPort.cpp 
		${UIPATH}/NodesEditor.cpp
		Dialogs/NEColorPresetDialog.cpp 
    Utility/ganalytics.cpp

)

include_directories ( ${CMAKE_CURRENT_BINARY_DIR})
include_directories ( ${CMAKE_CURRENT_BINARY_DIR}/Dialogs)
include_directories ( ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager)
include_directories ( ${CMAKE_CURRENT_BINARY_DIR}/Utility)

set ( UI_HEADERS
	NEUIApplication.h 
	globals_UI.h 
	Dialogs/NEAboutDialog.h 
	Dialogs/NEAbstractDialog.h 
	Dialogs/NEColliderDialog.h 
	Dialogs/NECurveDialog.h 
	Dialogs/NEColorPresetDialog.h 
	Dialogs/NEFileDialog.h 
	Dialogs/NEGPUBufferVis.h 
	Dialogs/NEHelpEngine.h 
	Dialogs/NEImageViewer.h 
	Dialogs/NEParticleSectionDialog.h 
	Dialogs/NEPluginInfoDialog.h 
	Dialogs/NEProcessDialog.h 
	Dialogs/NESceneManagementDialog.h 
	Dialogs/NESectionWidget.h 
	Dialogs/Plot/NECustomPlot.h 
	Manager/NEGUIManager.h 
	Manager/NEGUIManager_p.h 
	PropertyManager/fileedit.h 
	PropertyManager/NEDoubleSpinBox.h 
	PropertyManager/NEObjectController.h 
	PropertyManager/NEObjectController_p.h 
	PropertyManager/qtbuttonpropertybrowser.h 
	PropertyManager/qtgroupboxpropertybrowser.h 
	PropertyManager/qtpropertybrowserutils_p.h 
	PropertyManager/qttreepropertybrowser.h 
	PropertyManager/NEVariantFactory.h 
	PropertyManager/NEVariantManager.h 
	PropertyManager/qteditorfactory.h 
	PropertyManager/qtpropertybrowser.h 
	PropertyManager/qtpropertymanager.h 
	PropertyManager/qtvariantproperty.h 
	NEBufferVisualizer.h 
	NEBufferVisWindow.h 
	NEChartsVisWindow.h 
	NEConnectionGEItem.h 
	NEConnectionInspector.h 
	NEConsoleWindow.h 
	NEDockWidget.h 
	NEColorRampEditor.h 
	NEExpressionEditor.h 
	NEExpressionEditorWindow.h 
	NEExpressionInspector.h 
	NEExternalRendererSettingsWindow.h 
	NEGLMainWindow.h 
	NEGLWidget.h 
	NEGLWidget_p.h 
   	AreaTex.h 
	SearchTex.h 
	NEGUIStyle.h 
	NEKeyframeInspector.h 
	NELineNumberArea.h 
	NELogBrowser.h 
	NEMainWindow.h 
	NEMenu.h 
	NEMenuBar.h 
	NEMessageWindow.h 
	NENodeGraphEditor.h 
	NENodeGraphEditorItem.h 
	NENodeGraphEditorScene.h 
	NENodeGraphEditorWindow.h 
	NEPluginManagerWindow.h 
	NEPluginsTreeWidget.h 
	NEPreferencesDialog.h 
	NEPropertyEditor.h 
	NEPropertyEditorWindow.h 
	NEPropertyObject.h 
	NEPythonInput.h 
	NESceneBuilderWindow.h 
	NESceneInspector.h 
	NESceneInspectorWindow.h 
	NEShelfBar.h 
	NEShelfItemEditor.h 
	NESystemInspector.h 
	NESystemInspectorWindow.h 
	NETimeline.h 
	NETimelineWidget.h 
	NETitlebarWidget.h 
	PythonHighlighter/PythonHighlighter.h 
	Utility/ganalytics.h 
	Utility/Curves/NECurveView.h 
	Utility/Curves/NECurveScene.h 
	Utility/Curves/NEEnumCurveScene.h 
	Utility/Curves/NEKey.h 
	NEBlock.h 
	NEConnection.h 
	NEPort.h 
	NodesEditor.h
	# PCH_UI.h 
    Dialogs/NEColorPresetDialog.h 
    Utility/ganalytics.h
)

set ( UI_FORMS
	${UIPATH}/neutrino.ui 
	${UIPATH}/NEExpressionEditorWindow.ui 
	${UIPATH}/NEExpressionInspector.ui 
	${UIPATH}/NEKeyframeInspector.ui 
	${UIPATH}/NEConnectionInspector.ui 
	${UIPATH}/NEGLMainWindow.ui 
	${UIPATH}/NEConsoleWindow.ui 
	${UIPATH}/NETimeline.ui 
	${UIPATH}/NETitlebarWidget.ui 
	${UIPATH}/NECurveDialog.ui 
	${UIPATH}/NEParticleSectionDialog.ui 
	${UIPATH}/NESceneInspectorWindow.ui 
	${UIPATH}/NESystemInspectorWindow.ui 
	${UIPATH}/NEPropertyEditorWindow.ui 
	${UIPATH}/NEMessageWindow.ui 
	${UIPATH}/NESceneBuilderWindow.ui 
	${UIPATH}/NENodeGraphEditorWindow.ui 
	${UIPATH}/NEBufferVisWindow.ui 
	${UIPATH}/NEPreferencesDialog.ui 
	${UIPATH}/Dialogs/NESceneManagementDialog.ui 
	${UIPATH}/Dialogs/NEAboutDialog.ui 
	${UIPATH}/NEChartsVisWindow.ui 
	${UIPATH}/NEExternalRendererSettingsWindow.ui 
	${UIPATH}/Dialogs/NEProcessDialog.ui 
	${UIPATH}/NEPluginManagerWindow.ui 
	${UIPATH}/Dialogs/NEPluginInfoDialog.ui 
	${UIPATH}/NEShelfItemEditor.ui
)

set ( UI_RESOURCES
	${UIPATH}/PropertyManager/qtpropertybrowser.qrc 
	${UIPATH}/neutrino.qrc
)

source_group ( "Forms" FILES ${UI_FORMS} )
source_group ( "Resources" FILES ${UI_RESOURCES} )




qt5_wrap_ui(UI_GENERATED_HEADERS ${UI_FORMS})

# PropertyManager/qtpropertymanager.cpp 
# Utility/ganalytics.cpp 
# PropertyManager/qttreepropertybrowser.cpp
# PropertyManager/qteditorfactory.cpp 

file (GLOB MOC_FILES 
	${UI_HEADERS}
)

qt5_wrap_cpp(MOC_SOURCES ${MOC_FILES} OPTIONS ${MOC_OPTION})

set ( EXCLUDE_SOURCES )

foreach(value ${MOC_SOURCES})
	if(${value} MATCHES "moc_NEObjectController.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qtbuttonpropertybrowser.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qteditorfactory.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qtgroupboxpropertybrowser.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()
		
	if(${value} MATCHES "moc_qtpropertybrowser.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qtpropertymanager.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qttreepropertybrowser.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qtvariantproperty.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

	if(${value} MATCHES "moc_qtvariantproperty.cpp")
		# list ( REMOVE_ITEM MOC_SOURCES ${value} )
		list ( APPEND EXCLUDE_SOURCES ${value})
	endif()

endforeach()




qt5_generate_moc(PropertyManager/qtpropertymanager.cpp ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qtpropertymanager.moc )
qt5_generate_moc(Utility/ganalytics.cpp  ${CMAKE_CURRENT_BINARY_DIR}/Utility/ganalytics.moc )
qt5_generate_moc(PropertyManager/qttreepropertybrowser.cpp ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qttreepropertybrowser.moc )
qt5_generate_moc(PropertyManager/qteditorfactory.cpp ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qteditorfactory.moc  )

list ( APPEND EXCLUDE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qtpropertymanager.moc)
list ( APPEND EXCLUDE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/Utility/ganalytics.moc)
list ( APPEND EXCLUDE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qttreepropertybrowser.moc)
list ( APPEND EXCLUDE_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qteditorfactory.moc)

list ( APPEND ALL_SOURCES 
	${UI_SOURCES}
	${MOC_SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qtpropertymanager.moc
	${CMAKE_CURRENT_BINARY_DIR}/Utility/ganalytics.moc
	${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qttreepropertybrowser.moc
	${CMAKE_CURRENT_BINARY_DIR}/PropertyManager/qteditorfactory.moc
)

ADD_MSVC_PRECOMPILED_HEADER(${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE} ALL_SOURCES)

set_source_files_properties(${EXCLUDE_SOURCES} PROPERTIES
  	HEADER_FILE_ONLY TRUE)
# set_source_files_properties(${EXCLUDE_SOURCES} PROPERTIES
# 	HEADER_FILE_ONLY TRUE)
# set_source_files_properties(${EXCLUDE_SOURCES} PROPERTIES
# 	HEADER_FILE_ONLY TRUE)
# set_source_files_properties(${EXCLUDE_SOURCES} PROPERTIES
# 	HEADER_FILE_ONLY TRUE)
# set_source_files_properties(${EXCLUDE_SOURCES} PROPERTIES
# 	HEADER_FILE_ONLY TRUE)

add_library( ${PRODUCT_UI} SHARED
	${UI_HEADERS}
	${UI_GENERATED_HEADERS}
	${UI_RESOURCES}
	${ALL_SOURCES}
)

SET_TARGET_PROPERTIES(${PRODUCT_UI} PROPERTIES FOLDER ${CODE_FILTER_NAME})

set_target_properties(${PRODUCT_UI} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

add_dependencies( ${PRODUCT_UI} ${PRODUCT_GIZMO} ${PRODUCT_CORE} ${PRODUCT_GEOMETRY} ${PRODUCT_RENDERING} ${PRODUCT_RIGIDS})
qt5_use_modules(${PRODUCT_UI} Gui Core Xml Network Widgets OpenGL PrintSupport Test)