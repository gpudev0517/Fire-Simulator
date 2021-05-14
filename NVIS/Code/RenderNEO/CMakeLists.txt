cmake_minimum_required (VERSION 3.1.0)

set (PRODUCT_RENDERING "NERendering" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

include(../cmake/common.cmake)
include(utility)

find_package( Qt5Core REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )
find_package( Qt5Network REQUIRED )
find_package( Qt5OpenGL REQUIRED )

add_definitions ( -DNEUTRINO_NERENDERING_BUILD )

set ( PRECOMPILED_SOURCE ${ROOTPATH}/Code/Rendering/PCH_Rendering.cpp )
set ( PRECOMPILED_HEADER PCH_Rendering.h )

set ( RENDERINGPATH ${ROOTPATH}/Code/Rendering )
set ( USE_3DELIGHT OFF)

# Common Includes #
include_directories(
	${ROOTPATH}
	${ROOTPATH}/Code
	${ROOTPATH}/Code/Core
	${ROOTPATH}/External
	${ROOTPATH}/External/include
	${ROOTPATH}/External/OpenVDB
)

# Platform Specific Configurations #

if(WIN32 AND MSVC)
	include_directories(
		${ROOTPATH}/External/Windows/include
		${ROOTPATH}/External/Windows/include/python27
	)
endif()

include_directories(
	$$RENDERINGPATH
	${ROOTPATH}/Code/Coupling
	${ROOTPATH}/Code/ExtraSources
	${ROOTPATH}/Code/Plugins
)

if( WIN32 AND MSVC)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NECore.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NECore.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEGeometry.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEGeometry.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEPostProcessor.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEPostProcessor.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NERigids.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NERigids.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/openVDB.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/openVDB.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/GFSDK_SSAO_GL.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/GFSDK_SSAO_GL.lib)

	link_libraries(Half.lib tbb.lib)

	if(${USE_ADDSOURCES})
		link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEFlowDaemons.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEFlowDaemons.lib)
		link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/NEPeriodicBoundaries.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/NEPeriodicBoundaries.lib)
	endif()

	if(${CUDA_INSTALLED}) 
		add_definitions( -DCUDA_AVAILABLE )
		link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/GVDB.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/GVDB.lib)
	endif()

	if(${USE_3DELIGHT}) 
		include_directories( ${ROOTPATH}/External/Windows/include/3DeLight )
		add_definitions( -DTHREEDELIGHT_AVAILABLE )
		link_libraries(3DeLight.lib)
	endif()

	link_libraries ( GLU32.lib OpenGL32.lib  glew32.lib ftgl.lib )
	link_libraries(optimized python27.lib debug python27_d.lib)
	link_libraries(optimized ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/PythonQt.lib debug ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/PythonQt.lib)
else()
	# insert for other platform
endif()

if( UNIX AND NOT APPLE)
	# LIBS += -lGLU
	# LIBS += -lGLEW
	# LIBS += -lftgl
	# LIBS += -lfreetype
endif()

if( APPLE )
	# LIBS += -lGLU
	# LIBS += -lglew
	# LIBS += -lftgl
	# LIBS += -lfreetype
endif()

if(WIN32 AND MSVC)
	set ( VTK_VERSION 7.1 )
	include_directories( 
		${ROOTPATH}/External/Windows/include/VTK/ 
		${ROOTPATH}/External/Windows/include/Freetype2
	)
endif()

if(APPLE)
	set ( VTK_VERSION 7.1 )
	include_directories(
		${ROOTPATH}/External/macosx/include/vtk-${VTK_VERSION}
		${ROOTPATH}/External/macosx/include/freetype2
	)
endif()

if(UNIX AND NOT APPLE)
	set ( VTK_VERSION 7.1 )
	include_directories(
		${ROOTPATH}/External/Linux/include/vtk-${VTK_VERSION}
		${ROOTPATH}/External/Linux/include/freetype2
	)
endif()

if(${USE_VTK})
	if( WIN32 AND MSVC)
		link_libraries(
			vtkCommonTransforms-${VTK_VERSION}.lib
			vtkIOXML-${VTK_VERSION}.lib
			vtkFiltersModeling-${VTK_VERSION}.lib
			vtkFiltersGeneral-${VTK_VERSION}.lib
			vtkFiltersSources-${VTK_VERSION}.lib
			vtkFiltersCore-${VTK_VERSION}.lib
			vtkCommonColor-${VTK_VERSION}.lib
			vtkCommonExecutionModel-${VTK_VERSION}.lib
			vtkCommonDataModel-${VTK_VERSION}.lib
			vtkCommonMisc-${VTK_VERSION}.lib
			vtkCommonCore-${VTK_VERSION}.lib
			vtkCommonMath-${VTK_VERSION}.lib
			vtkCommonSystem-${VTK_VERSION}.lib
			vtksys-${VTK_VERSION}.lib
			vtkIOCore-${VTK_VERSION}.lib
			vtkIOXMLParser-${VTK_VERSION}.lib
			vtkzlib-${VTK_VERSION}.lib
			vtkexpat-${VTK_VERSION}.lib
		)
	else()
		# insert for other platform
	endif()
endif()

set ( RENDERING_SOURCES
	${RENDERINGPATH}/Rendering/Filter/NEFilterFXAA.cpp 
	${RENDERINGPATH}/Rendering/Manager/NEGLManager.cpp 
	${RENDERINGPATH}/Rendering/Manager/NERenderManager.cpp 
	${RENDERINGPATH}/Rendering/NEOsd3d.cpp 
	${RENDERINGPATH}/Rendering/NEGLRenderer.cpp 
	${RENDERINGPATH}/Rendering/NEGLRenderableMesh.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEClearWaterMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEParticleWaterMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEOceanMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEPBRMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEPhongMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEStructureMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NETerrainMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEToneMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEToonMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NETransparentMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/Material/NEVolumeMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/NEIndexBuffer.cpp 
	${RENDERINGPATH}/Rendering/Resource/NEMaterial.cpp 
	${RENDERINGPATH}/Rendering/Resource/NESurface.cpp 
	${RENDERINGPATH}/Rendering/Resource/NEVertexBuffer.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEAreaLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEDirectionalLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEImageBasedLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEPointLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NESkyLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/Light/NESpotLight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NECamera.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NELight.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEParticleKillerExtentRenderer.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEObjectRenderer.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDBase.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDGlobalInformation.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDMeasurement.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDParticleFluid.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDParticleRigid.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDRigidTopography.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEClipPlane.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NENullGizmo.cpp 
	${RENDERINGPATH}/Rendering/Spatial/NEInteractiveMeasurementGizmo.cpp 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEPovrayExport.cpp 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEPovRaySettings.cpp 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NERenderSetting.cpp 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NESpecificRenderSetting.cpp 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKSettings.cpp 
	${RENDERINGPATH}/Utility/glutils.cpp 
	${RENDERINGPATH}/Utility/NEOsd.cpp
)
	

set ( RENDERING_HEADERS
	globals_Rendering.h 
	${RENDERINGPATH}/Rendering/Filter/NEFilterBase.h 
	${RENDERINGPATH}/Rendering/Filter/NEFilterFXAA.h 
	${RENDERINGPATH}/Rendering/Manager/NEGLManager.h 
	${RENDERINGPATH}/Rendering/Manager/NEGLManager_p.h
	${RENDERINGPATH}/Rendering/Manager/NERenderManager.h 
	${RENDERINGPATH}/Rendering/NEOsd3d.h 
	${RENDERINGPATH}/Rendering/NEGLRenderer.h 
	${RENDERINGPATH}/Rendering/NEGLRenderableMesh.h 
	${RENDERINGPATH}/Rendering/NEGLRenderer_p.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEClearWaterMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEParticleWaterMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEOceanMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEPBRMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEPhongMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEStructureMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NETerrainMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEToneMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEToonMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NETransparentMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/Material/NEVolumeMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/NEIndexBuffer.h 
	${RENDERINGPATH}/Rendering/Resource/NEMaterial.h 
	${RENDERINGPATH}/Rendering/Resource/NESurface.h 
	${RENDERINGPATH}/Rendering/Resource/NEVertexBuffer.h 
	${RENDERINGPATH}/Rendering/Resource/NEVertexFormats.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEAreaLight.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEDirectionalLight.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEImageBasedLight.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NEPointLight.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NESkyLight.h 
	${RENDERINGPATH}/Rendering/Spatial/Light/NESpotLight.h 
	${RENDERINGPATH}/Rendering/Spatial/NECamera.h 
	${RENDERINGPATH}/Rendering/Spatial/NECamera_p.h 
	${RENDERINGPATH}/Rendering/Spatial/NELight.h 
	${RENDERINGPATH}/Rendering/Spatial/NEParticleKillerExtentRenderer.h 
	${RENDERINGPATH}/Rendering/Spatial/NEObjectRenderer.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDBase.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDGlobalInformation.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDMeasurement.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDParticleFluid.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDParticleRigid.h 
	${RENDERINGPATH}/Rendering/Spatial/NEOSDRigidTopography.h 
	${RENDERINGPATH}/Rendering/Spatial/NEClipPlane.h 
	${RENDERINGPATH}/Rendering/Spatial/NENullGizmo.h 
	${RENDERINGPATH}/Rendering/Spatial/NEInteractiveMeasurementGizmo.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEPovrayExport.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEPovRaySettings.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NERenderSetting.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NESpecificRenderSetting.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKExport.h 
	${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKSettings.h 
	${RENDERINGPATH}/Utility/glutils.h 
	${RENDERINGPATH}/Utility/NEOsd.h
	# PCH_Rendering.h
)


if(${USE_VTK})
	list ( APPEND RENDERING_SOURCES 
		${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKExport.cpp 
		${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKSettings.cpp
	)
	list ( APPEND RENDERING_HEADERS
		${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKExport.h 
		${RENDERINGPATH}/Rendering/Utility/RenderSettings/NEVTKSettings.h
	)
endif()


if(${USE_GRIDFLUID} ) 
	list ( APPEND RENDERING_SOURCES ${RENDERINGPATH}/Rendering/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridRenderer.cpp )
	list ( APPEND RENDERING_HEADERS ${RENDERINGPATH}/Rendering/Spatial/Physical/Fluid/GridFluid/MacGrid/NEMacGridRenderer.h	)
endif()

if(${USE_NIISPH} ) 
	list ( APPEND RENDERING_SOURCES ${RENDERINGPATH}/Rendering/Spatial/NENIISPHRenderer.cpp )
	list ( APPEND RENDERING_HEADERS ${RENDERINGPATH}/Rendering/Spatial/NENIISPHRenderer.h )
endif()

if(${USE_NWCSPH} )
	
	list  ( APPEND RENDERING_SOURCES ${RENDERINGPATH}/Rendering/Spatial/NENWCSPHRenderer.cpp )
	list ( APPEND RENDERING_HEADERS  ${RENDERINGPATH}/Rendering/Spatial/NENWCSPHRenderer.h )
endif()

if(${USE_ADDSOURCES} )
	list ( APPEND RENDERING_SOURCES
		${RENDERINGPATH}/Rendering/Spatial/NESourceRenderer.cpp 
		#${RENDERINGPATH}/Rendering/Spatial/NEParticleConstrainerRenderer.cpp 
		${RENDERINGPATH}/Rendering/Spatial/NEPeriodicBoundariesRenderer.cpp 
		${RENDERINGPATH}/Rendering/Spatial/NEVolumeKillerRenderer.cpp 
		${RENDERINGPATH}/Rendering/Spatial/NEFlowKillerRenderer.cpp
	)

	list ( APPEND RENDERING_HEADERS 
		${RENDERINGPATH}/Rendering/Spatial/NESourceRenderer.h 
		#${RENDERINGPATH}/Rendering/Spatial/NEParticleConstrainerRenderer.h 
		${RENDERINGPATH}/Rendering/Spatial/NEPeriodicBoundariesRenderer.h 
		${RENDERINGPATH}/Rendering/Spatial/NEVolumeKillerRenderer.h 
		${RENDERINGPATH}/Rendering/Spatial/NEFlowKillerRenderer.h
	)
endif()

if(${USE_COUPLING}) 

        list ( APPEND RENDERING_SOURCES 
            ${RENDERINGPATH}/Rendering/Spatial/NECouplerRenderer.cpp 
            #${RENDERINGPATH}/Rendering/Spatial/NEHeightFieldSourceRenderer.cpp 
            ${RENDERINGPATH}/Rendering/Spatial/NELinkRenderer.cpp
		)

        list ( APPEND RENDERING_HEADERS
            ${RENDERINGPATH}/Rendering/Spatial/NECouplerRenderer.h 
            #${RENDERINGPATH}/Rendering/Spatial/NEHeightFieldSourceRenderer.h 
            ${RENDERINGPATH}/Rendering/Spatial/NELinkRenderer.h
		)
endif()


if(${USE_GEOM})
  if(${USE_VOLUME} )
	list ( APPEND RENDERING_SOURCES
		${RENDERINGPATH}/Rendering/Spatial/NEOSDVolume.cpp 
		${RENDERINGPATH}/Rendering/Spatial/NEVolumeRenderer.cpp
	)

	list ( APPEND RENDERING_HEADERS
		${RENDERINGPATH}/Rendering/Spatial/NEOSDVolume.h 
		${RENDERINGPATH}/Rendering/Spatial/NEVolumeRenderer.h
	)
  endif()
  list ( APPEND RENDERING_SOURCES ${RENDERINGPATH}/Rendering/Spatial/NEYuTurkMCRenderer.cpp )
  list ( APPEND RENDERING_HEADERS ${RENDERINGPATH}/Rendering/Spatial/NEYuTurkMCRenderer.h )
endif()


if(${CUDA_INSTALLED})
    if(${USE_FIRE} )
        list ( APPEND RENDERING_SOURCES
			${RENDERINGPATH}/Rendering/Spatial/NEFireRenderer.cpp 
			${RENDERINGPATH}/Rendering/Spatial/NEFireCombineRenderer.cpp
		)
        list ( APPEND RENDERING_HEADERS
			${RENDERINGPATH}/Rendering/Spatial/NEFireRenderer.h 
			${RENDERINGPATH}/Rendering/Spatial/NEFireCombineRenderer.h
		)
    endif()

    list ( APPEND RENDERING_SOURCES 
		${RENDERINGPATH}/Rendering/Spatial/NEGVDBRenderer.cpp 
		${RENDERINGPATH}/Rendering/Spatial/NEGVDBNode.cpp
	)
    list ( APPEND RENDERING_HEADERS 
		${RENDERINGPATH}/Rendering/Spatial/NEGVDBRenderer.h 
		${RENDERINGPATH}/Rendering/Spatial/NEGVDBNode.h
	)
    if(${USE_GEOM} )
            list ( APPEND RENDERING_SOURCES  ${RENDERINGPATH}/Rendering/Spatial/NEGVDBSurfaceRenderer.cpp )
            list ( APPEND RENDERING_HEADERS  ${RENDERINGPATH}/Rendering/Spatial/NEGVDBSurfaceRenderer.h )
    endif()

    # CUDA_PTX_SRC_DIR = ${ROOTPATH}/Build$$QT_VERSION
    # CUDA_PTX_DEST_DIR = ${ROOTPATH}/../../Data

    # CONFIG(debug, debug|release) {
        # CUDA_PTX_SRC_DIR = $$join(CUDA_PTX_SRC_DIR,"","","/Debug")
    # }
    # else {
        # CUDA_PTX_SRC_DIR = $$join(CUDA_PTX_SRC_DIR,"","","/Release")
    # }

    # PTX_FILES += 
        # cuda_gvdb_copydata.ptx 
        # cuda_gvdb_module.ptx

    # for(file, PTX_FILES) {
        # eval($${file}.depends = $$CUDA_PTX_SRC_DIR/$$file)
        # eval($${file}.target = $$CUDA_PTX_DEST_DIR/$$file)
        # COPY_COMMAND = copy $$CUDA_PTX_SRC_DIR/$$file $$CUDA_PTX_DEST_DIR
        # win32 {
            # COPY_COMMAND = $$replace(COPY_COMMAND, "/", "")
            # COPY_COMMAND += " /Y"
        # }
        # eval($${file}.commands = $$COPY_COMMAND)

        # QMAKE_EXTRA_TARGETS += $${file}
        # PRE_TARGETDEPS += $$CUDA_PTX_DEST_DIR/$$file
    # }

else()
    message("No CUDA is installed, skip using gvdb.")
endif()


file (GLOB MOC_FILES 
	${RENDERING_HEADERS}
)

QT5_WRAP_CPP(MOC_SOURCES ${MOC_FILES})


list ( APPEND ALL_SOURCES 
	${RENDERING_SOURCES}
	${MOC_SOURCES}
)
ADD_MSVC_PRECOMPILED_HEADER(${PRECOMPILED_HEADER} ${PRECOMPILED_SOURCE} ALL_SOURCES)

if( CUDA_INSTALLED)
	cuda_add_library( ${PRODUCT_RENDERING} SHARED
		${RENDERING_HEADERS}
		${ALL_SOURCES}
	)
else()
	add_library( ${PRODUCT_RENDERING} SHARED
		${RENDERING_HEADERS}
		${ALL_SOURCES}
	)
endif()

SET_TARGET_PROPERTIES(${PRODUCT_RENDERING} PROPERTIES FOLDER ${CODE_FILTER_NAME})

set_target_properties(${PRODUCT_RENDERING} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
)

add_dependencies( ${PRODUCT_RENDERING} ${PRODUCT_GFSDK} ${PRODUCT_CORE} ${PRODUCT_GEOMETRY} ${PRODUCT_POSTPROCESSOR} ${PRODUCT_RIGIDS})
qt5_use_modules(${PRODUCT_RENDERING} Core Xml Network Widgets OpenGL)
