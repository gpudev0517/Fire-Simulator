cmake_minimum_required(VERSION 3.1.0)

set (PRODUCT_GVDB "GVDB" CACHE STRING "")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake )

include (utility)
include (external_lib_config)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)

if( CUDA_INSTALLED )
  add_definitions(  -DNOMINMAX -DGLEW_NO_GLU -DUSECUDA -DGVDB_EXPORTS -DBUILD_OPENGL )

  if(UNIX)
  endif()

  if(MSVC)
    link_libraries( OpenGL32.lib glew32.lib)
  endif()

  if(APPLE)
  endif()

  include_directories( ${CMAKE_CURRENT_LIST_DIR}/src )

  set ( GVDB_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/app_perf.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_allocator.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_camera.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_model.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_node.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_render_opengl.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_scene.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_types.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_vec.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_3D.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_base.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_gvdb.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_ObjarReader.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_OBJReader.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_Parser.cpp 
      ${CMAKE_CURRENT_LIST_DIR}/src/string_helper.cpp
  )
  set ( GVDB_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/app_perf.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_allocator.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_camera.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_model.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_node.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_render.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_scene.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_types.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_vec.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_3D.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_base.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/gvdb_volume_gvdb.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_ObjarReader.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_OBJReader.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/loader_Parser.h 
      ${CMAKE_CURRENT_LIST_DIR}/src/string_helper.h
  )

  set ( CUDA_PTX_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_copydata.cu 
      ${CMAKE_CURRENT_LIST_DIR}/kernels/cuda_gvdb_module.cu

  )

  if(${DEBUG_BUILD})
  set ( CUDA_PTX_DEST
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/cuda_gvdb_copydata.ptx 
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug/cuda_gvdb_module.ptx

  )
  else()
  set ( CUDA_PTX_DEST
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/cuda_gvdb_copydata.ptx 
      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release/cuda_gvdb_module.ptx

  )
  endif()

  cuda_compile_ptx(
    cuda_ptx_files
    ${CUDA_PTX_SOURCES}
  )

  list( GET cuda_ptx_files 0 ptx0 )
  list( GET cuda_ptx_files 1 ptx1 )

  list (GET CUDA_PTX_DEST 0 dest_ptx0)
  list (GET CUDA_PTX_DEST 1 dest_ptx1)

  source_group ( "Sources" FILES ${GVDB_SOURCES} )
  source_group ( "Headers" FILES ${GVDB_HEADERS} )
  source_group ( "Kernels" FILES ${CUDA_PTX_SOURCES} )
  
  find_package(CUDA REQUIRED)
  
  include_directories ( ${CMAKE_CURRENT_LIST_DIR}/../shared_cudpp/include)
  cuda_add_library( ${PRODUCT_GVDB} SHARED
    ${GVDB_SOURCES} ${GVDB_HEADERS}
    ${CUDA_PTX_SOURCES}
    # ${cuda_ptx_files}

  )

  SET_TARGET_PROPERTIES(${PRODUCT_GVDB} PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

  if( WIN32 AND MSVC )
    target_link_libraries( ${PRODUCT_GVDB}
      optimized ${CMAKE_CURRENT_LIST_DIR}/../shared_cudpp/lib/cudpp_2015x64.lib
      debug ${CMAKE_CURRENT_LIST_DIR}/../shared_cudpp/lib/cudpp_2015x64d.lib
    )
  else()
    # insert for other platform
  endif()

  set_target_properties(${PRODUCT_GVDB} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
  )

  add_custom_command(TARGET ${PRODUCT_GVDB}  POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${ptx0} ${dest_ptx0}
      COMMENT "Copying ${ptx0} to ${dest_ptx0}."
  )

  add_custom_command(TARGET ${PRODUCT_GVDB}  POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${ptx1} ${dest_ptx1}
      COMMENT "Copying ${ptx1} to ${dest_ptx1}."
  )

  
  # add_custom_command(OUTPUT ${dest_ptx0} COMMAND ${CMAKE_COMMAND} -E copy ${ptx0} ${dest_ptx0} DEPENDS ${ptx0})
  # add_custom_command(OUTPUT ${dest_ptx1} COMMAND ${CMAKE_COMMAND} -E copy ${ptx1} ${dest_ptx1} DEPENDS ${ptx1})

  # add_custom_target(ptx ALL
  #     DEPENDS ${CUDA_PTX_DEST} ${CUDA_PTX_SOURCES}
  #     SOURCES ${CUDA_PTX_SOURCES}
  # )
  
  # SET_TARGET_PROPERTIES(ptx PROPERTIES FOLDER ${EXTERNAL_FILTER_NAME})

else()
message("No CUDA compiler found: Can't build gvdb.")
endif()