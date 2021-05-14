cmake_minimum_required (VERSION 3.1.0)


MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
	
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
	
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/TP /Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

find_package(CUDA)
if(CUDA_FOUND)
	set(CUDA_INSTALLED ON CACHE BOOL "Use CUDA")
else()
	set(CUDA_INSTALLED OFF CACHE BOOL "Use CUDA")
endif()

if(CUDA_INSTALLED)
	if(WIN32 AND MSVC )
		set (SYSTEM_TYPE "64")
		set (CUDA_ARCH "sm_50")
		set (SYSTEM_NAME "Win64")
		set (NVCC_OPTIONS "${NVCC_OPTIONS} -MD  --use_fast_math -Xcompiler")
		include_directories( ${CUDA_TOOLKIT_ROOT_DIR}/include )
		set( CUDA_LIBRARY ${CUDA_TOOLKIT_ROOT_DIR}/lib/x64/cuda.lib )
		set( CUDART_LIBRARY ${CUDA_TOOLKIT_ROOT_DIR}/lib/x64/cudart.lib )
		link_libraries (${CUDA_LIBRARY} ${CUDART_LIBRARY} )

	elseif(UNIX AND NOT APPLE)
		# SYSTEM_TYPE = 64            # '32' or '64', depending on your system
		# CUDA_ARCH = sm_50            # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
		# NVCC_OPTIONS = --use_fast_math -Xcompiler
		# SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'

		# NVCC_OPTIONS += -fpic

		# QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
		# 				/usr/lib/nvidia-319
		# LIBS += $$CUDA_LIBS
	elseif(APPLE)
		# SYSTEM_TYPE = 64
		# CUDA_ARCH = sm_50           # Type of CUDA architecture, for example 'compute_10', 'compute_11', 'sm_10'
		# SYSTEM_NAME = unix         # Depending on your system either 'Win32', 'x64', or 'Win64'

		# #NVCC_OPTIONS += -fpic

		# QMAKE_LIBDIR += /usr/lib/x86_64-linux-gnu \
		# 				/usr/lib/nvidia-319
		# LIBS += $$CUDA_LIBS
	endif()
endif()




