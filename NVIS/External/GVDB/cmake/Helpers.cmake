#----- copy_cuda_bins
# This macro copies all binaries for the CUDA library to the target executale location. 
#
macro(_copy_cuda_bins projname )	
	add_custom_command(
		TARGET ${projname} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy ${CUDA_DLL} $<TARGET_FILE_DIR:${PROJNAME}>   
	)
endmacro()

#------------------------------------ FIND LOCAL PACKAGES

macro ( _FIND_LOCALPACKAGE_CUDA ) 

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../LocalPackages/CUDA)
    Message(STATUS "Local CUDA detected. Using it")
    set(CUDA_LOCALPACK_VER "8.0" CACHE STRING "CUDA Version")
    set(CUDA_TOOLKIT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../LocalPackages/CUDA/v${CUDA_LOCALPACK_VER}_win")
  endif()

endmacro()


macro ( _FIND_LOCALPACKAGE_OPTIX )

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../LocalPackages/Optix)
    Message(STATUS "Local Optix detected. Using it")
    set(OPTIX_LOCALPACK_VER "4.0.1" CACHE STRING "OptiX Version")
    set(OPTIX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../LocalPackages/Optix/Optix_${OPTIX_LOCALPACK_VER}_win64")
  endif()

endmacro ()



#------------------------------------ CROSS-PLATFORM PTX COMPILE 
#
# _COMPILEPTX( SOURCES file1.cu file2.cu TARGET_PATH <path where ptxs should be stored> GENERATED_FILES ptx_sources NVCC_OPTIONS -arch=sm_20)
# Generates ptx files for the given source files. ptx_sources will contain the list of generated files.
#
FUNCTION( _COMPILEPTX )
  set(options "")
  set(oneValueArgs TARGET_PATH GENERATED INCLUDE )
  set(multiValueArgs OPTIONS SOURCES)
  CMAKE_PARSE_ARGUMENTS( _COMPILEPTX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  # Match the bitness of the ptx to the bitness of the application
  set( MACHINE "--machine=32" )
  if( CMAKE_SIZEOF_VOID_P EQUAL 8)
    set( MACHINE "--machine=64" )
  endif()
  unset ( PTX_FILES CACHE )
  
  
  if ( WIN32 ) 
		# Windows - PTX compile
		file ( MAKE_DIRECTORY "${_COMPILEPTX_TARGET_PATH}/Debug" )
		file ( MAKE_DIRECTORY "${_COMPILEPTX_TARGET_PATH}/Release" )
		string (REPLACE ";" " " _COMPILEPTX_OPTIONS "${_COMPILEPTX_OPTIONS}")  
		separate_arguments( _OPTS WINDOWS_COMMAND "${_COMPILEPTX_OPTIONS}" )
		message ( STATUS "NVCC Options: ${_COMPILEPTX_OPTIONS}" )  
		message ( STATUS "NVCC Include: ${_COMPILEPTX_INCLUDE}" )

                set ( INCL "-I\"${_COMPILEPTX_INCLUDE}\"" )

		# Custom build rule to generate ptx files from cuda files
		FOREACH( input ${_COMPILEPTX_SOURCES} )
			get_filename_component( input_ext ${input} EXT )									# Input extension
			get_filename_component( input_without_ext ${input} NAME_WE )						# Input base
			if ( ${input_ext} STREQUAL ".cu" )			
				
				# Set output names
				set( output "${input_without_ext}.ptx" )							# Output name
				set( output_with_path "${_COMPILEPTX_TARGET_PATH}/$(Configuration)/${input_without_ext}.ptx" )	# Output with path
				set( output_with_quote "\"${output_with_path}\"" )
				LIST( APPEND PTX_FILES ${output} )												# Append to output list
    
				message( STATUS "NVCC Compile: ${CUDA_NVCC_EXECUTABLE} ${MACHINE} --ptx ${_COMPILEPTX_OPTIONS} ${input} ${INCL} -o ${output_with_path} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}")
    
				add_custom_command(
					OUTPUT  ${output_with_path}
					MAIN_DEPENDENCY ${input}
					COMMAND ${CUDA_NVCC_EXECUTABLE} ${MACHINE} --ptx ${_OPTS} ${input} ${INCL} -o ${output_with_quote} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
				)			
			endif()
		ENDFOREACH( )
  else ()
		# Linux - PTX compile
		file ( MAKE_DIRECTORY "${_COMPILEPTX_TARGET_PATH}" )
		cuda_compile_ptx(PTX_FILES ${_COMPILEPTX_SOURCES} )
  endif()

  set( ${_COMPILEPTX_GENERATED} ${PTX_FILES} PARENT_SCOPE)

  set( PTX_FILES ${PTX_FILES} CACHE PATH "path" FORCE )

ENDFUNCTION()

#------------------------------------ CROSS-PLATFORM INSTALL
function( _INSTALL )   
  set (options "")
  set (oneValueArgs DESTINATION SOURCE OUTPUT )
  set (multiValueArgs FILES )
  CMAKE_PARSE_ARGUMENTS(_INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if ( _INSTALL_SOURCE ) 
     set ( _INSTALL_SOURCE "${_INSTALL_SOURCE}/" )	  
  endif()
  set ( OUT_LIST ${${_INSTALL_OUTPUT}} )

  if ( WIN32 )      
      # Windows - copy to desintation at post-build
      file ( MAKE_DIRECTORY "${_INSTALL_DESTINATION}/" )
      foreach (_file ${_INSTALL_FILES} )	     
          message ( STATUS "Install: ${_INSTALL_SOURCE}${_file} -> ${_INSTALL_DESTINATION}" )
          add_custom_command(
            TARGET ${PROJNAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${_INSTALL_SOURCE}${_file} ${_INSTALL_DESTINATION}
          )          
 	  list ( APPEND OUT_LIST "${_INSTALL_SOURCE}${_file}" )
      endforeach()    
  else ()
      # Linux 
      if ( _INSTALL_SOURCE )	   
	  foreach ( _file ${_INSTALL_FILES} )
             list ( APPEND OUT_LIST "${_INSTALL_SOURCE}${_file}" )
    	  endforeach()
      else()
	     list ( APPEND OUT_LIST ${_INSTALL_FILES} )
      endif() 
  endif( )
  set ( ${_INSTALL_OUTPUT} ${OUT_LIST} PARENT_SCOPE )
   
endfunction()

#------------------------------------------------- CROSS-PLATFORM INSTALL PTX
#
function( _INSTALL_PTX )   
  set (options "")
  set (oneValueArgs DESTINATION OUTPUT )
  set (multiValueArgs FILES )
  CMAKE_PARSE_ARGUMENTS(_INSTALL_PTX "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set ( OUT_LIST ${${_INSTALL_OUTPUT}} )

  unset ( PTX_FIXED )

  if ( WIN32 )   
    set ( PTX_FIXED ${_INSTALL_PTX_FILES} )     # NOTE: Input of FILES must be list of ptx *with paths*	
    list ( APPEND OUT_LIST ${PTX_FIXED} )
  else()
    foreach ( _file IN ITEMS ${_INSTALL_PTX_FILES} )
      get_filename_component ( _ptxpath ${_file} DIRECTORY )
      get_filename_component ( _ptxbase ${_file} NAME_WE )
      string ( SUBSTRING ${_ptxbase} 27 -1 _ptxname )
      set ( _fixed "${_ptxpath}/${_ptxname}.ptx" )
      add_custom_command ( TARGET ${PROJNAME} PRE_LINK
        COMMAND ${CMAKE_COMMAND} -E copy  ${_file} ${_fixed}
        )      
      list ( APPEND PTX_FIXED ${_fixed} )
      list ( APPEND OUT_LIST ${_fixed} )
    endforeach()
  endif()
  
  # Install PTX
  message ( STATUS "PTX files: ${PTX_FIXED}" )
  _INSTALL ( FILES ${PTX_FIXED} DESTINATION ${_INSTALL_PTX_DESTINATION} )

  set ( ${_INSTALL_OUTPUT} ${OUT_LIST} PARENT_SCOPE )

endfunction()


#----------------------------------------------- CROSS-PLATFORM FIND FILES
# Find one or more of a specific file in the given folder
# Returns the file name w/o path

macro(_FIND_FILE targetVar searchDir nameWin64 nameLnx cnt)
  unset ( fileList )  
  unset ( nameFind )  
  unset ( targetVar )  
  # set ( ${cnt} "0" )
  if ( WIN32 ) 
     SET ( nameFind ${nameWin64} )
  else()
     SET ( nameFind ${nameLnx} )
  endif()
  file(GLOB fileList "${${searchDir}}/${nameFind}")  
  list(LENGTH fileList NUMLIST)  
  if (NUMLIST GREATER 0)	
     MATH (EXPR ${cnt} "${${cnt}}+1" )	
 	 list(APPEND ${targetVar} ${nameFind} )
  endif() 
endmacro()

#----------------------------------------------- CROSS-PLATFORM FIND MULTIPLE
# Find all files in specified folder with the given extension.
# This creates a file list, where each entry is only the filename w/o path
# Return the count of files
macro(_FIND_MULTIPLE targetVar searchDir extWin64 extLnx cnt)    
  unset ( fileList )    
  unset ( targetVar ) 
  unset ( ${cnt} )
  set ( ${cnt} "0" )
  if ( WIN32 ) 
     SET ( extFind ${extWin64} )
  else()
     SET ( extFind ${extLnx} )
  endif()
  file( GLOB fileList "${${searchDir}}/*.${extFind}")  
  list( LENGTH fileList NUMLIST)
  math( EXPR ${cnt} "${${cnt}}+${NUMLIST}" )  
  foreach ( _file ${fileList} )
     get_filename_component ( fname ${_file} NAME )
     list( APPEND ${targetVar} ${fname} )
  endforeach()
endmacro()

