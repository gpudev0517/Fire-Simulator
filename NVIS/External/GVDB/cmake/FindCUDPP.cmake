
#####################################################################################
# Find CUDPP
#
unset(CUDPP_FOUND CACHE)
unset(CUDPP_INCLUDE_DIR CACHE)

if ( NOT DEFINED CUDPP_ROOT_DIR )
  if (WIN32)
    get_filename_component ( BASEDIR "${CMAKE_MODULE_PATH}/../../shared_cudpp" REALPATH )
  else()
    get_filename_component ( BASEDIR "/usr/local/cudpp/" REALPATH )
  endif()
  set ( CUDPP_ROOT_DIR ${BASEDIR} CACHE PATH "Location of cuDPP library" FORCE)
endif()
message ( STATUS "Searching for cuDPP at.. ${CUDPP_ROOT_DIR}")
set( CUDPP_FOUND "YES" )

if ( CUDPP_ROOT_DIR )

    #-- Paths to CUDPP Library (cached so user can modify)
	set ( CUDPP_INCLUDE_DIR "${CUDPP_ROOT_DIR}/include" CACHE PATH "Path to include files" FORCE)
	set ( CUDPP_LIB_DIR "${CUDPP_ROOT_DIR}/lib" CACHE PATH "Path to libraries" FORCE)		

	#-------- Locate Header files
    set ( OK_H "0" )
	_FIND_FILE ( CUDPP_HEADERS CUDPP_INCLUDE_DIR "cudpp.h" "cudpp.h" OK_H )	
	if ( OK_H EQUAL 1 ) 
	    message ( STATUS "  Found. CUDPP Header files. ${CUDPP_INCLUDE_DIR}" )
	else()
	    message ( "  NOT FOUND. CUDPP Header files" )
		set ( CUDPP_FOUND "NO" )
	endif ()

    #-------- Locate Library	
     set ( OK_LIB "0" )		
  	_FIND_FILE ( LIST_LIB CUDPP_LIB_DIR "cudpp_${MSVC_YEAR}x64${CUDA_SUFFIX}.lib" "cudpp.so" OK_LIB )	        
  	_FIND_FILE ( LIST_LIB CUDPP_LIB_DIR "cudpp_hash_${MSVC_YEAR}x64${CUDA_SUFFIX}.lib" "cudpp_hash.so" OK_LIB )	
	_FIND_FILE ( LIST_LIB CUDPP_LIB_DIR "cudpp_${MSVC_YEAR}x64d${CUDA_SUFFIX}.lib" "cudpp.so" OK_LIB )	        	
	_FIND_FILE ( LIST_LIB CUDPP_LIB_DIR "cudpp_hash_${MSVC_YEAR}x64d${CUDA_SUFFIX}.lib" "cudpp_hashd.so" OK_LIB )
	message ( STATUS "  Searching for CUDPP LIBRARY: cudpp_${MSVC_YEAR}x64${CUDA_SUFFIX}.lib" )
	if ( OK_LIB EQUAL 4 ) 
	   message ( STATUS "  Found. CUDPP Library. ${CUDPP_LIB_DIR}" )	   
	else()
	   message ( "Some CUDPP Libraries are not there. Built and install 'shared_cudpp' prior to gvdb" )	   
	   message ( "Libraries found: ${LIST_LIB}" )	   
	endif()

endif()
 
if ( ${CUDPP_FOUND} STREQUAL "NO" )
   message( FATAL_ERROR "
      Please set CUDPP_ROOT_DIR to the root location 
      of installed CUDPP library containing /include and /lib.
      Not found at CUDPP_ROOT_DIR: ${CUDPP_ROOT_DIR}\n"
   )
endif()

set ( CUDPP_LIB_DIR ${CUDPP_LIB_DIR} CACHE INTERNAL "" FORCE)

#-- We do not want user to modified these vars, but helpful to show them
message ( STATUS "  CUDPP_ROOT_DIR:    ${CUDPP_ROOT_DIR}" )
message ( STATUS "  CUDPP_INCLUDE_DIR: ${CUDPP_INCLUDE_DIR}" )
message ( STATUS "  CUDPP_LIB_DIR:     ${CUDPP_LIB_DIR}" )

mark_as_advanced(CUDPP_FOUND)






