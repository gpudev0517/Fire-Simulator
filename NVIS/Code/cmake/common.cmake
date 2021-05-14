cmake_minimum_required (VERSION 3.1.0)
#set ( ParentDirectory ${ROOTPATH} )

add_definitions(-D_UNICODE -DUNICODE)

set ( ROOTPATH  ${CMAKE_CURRENT_LIST_DIR}/../../ )
set ( ParentDirectory ${ROOTPATH} )


set ( USE_OPENCL OFF CACHE BOOL "Use OpenCL")
set ( USE_CRYPTLIB OFF CACHE BOOL "Use CryptLib" )
set ( USE_GRIDFLUID OFF CACHE BOOL "Use Grid Fluid" )
set ( USE_COMM OFF CACHE BOOL "Use Communcations" )
set ( USE_CHRONO OFF CACHE BOOL "Use Chrono" )
set ( USE_BULLET ON CACHE BOOL "Use Bullet" )
set ( USE_POST OFF CACHE BOOL "Use Post" )
set ( USE_GEOM ON CACHE BOOL "Use Geom" )
set ( USE_FIRE OFF CACHE BOOL "Use Fire" )
set ( USE_VTK OFF CACHE BOOL "Use VTK" )

set ( USE_MEASURE ON CACHE BOOL "Use Measurement" )

set ( USE_IISPH ON CACHE BOOL "Use IISPH" )
set ( USE_NIISPH ON CACHE BOOL "Use NIISPH" )
set ( USE_NWCSPH ON CACHE BOOL "Use NWCSPH" )

set ( USE_VOLUME ON CACHE BOOL "Use Volumes" )
set ( USE_COUPLING ON CACHE BOOL "Use Coupling" )

set ( USE_FIELDS OFF CACHE BOOL "Use Fields" )
set ( USE_ADDSOURCES OFF CACHE BOOL "Use Additional Sources" )

set ( USE_SPRAY OFF CACHE BOOL "Use Spray" )

set ( USE_COMPILER "Intel Compiler 18.0" CACHE STRING "ICC" )
set_property(CACHE USE_COMPILER PROPERTY STRINGS "Intel Compiler 18.0" "Visual Studio C++ Compiler 17.0" )

if( WIN32 AND MSVC )
	if(${USE_COMPILER} STREQUAL "Intel Compiler 18.0")
		set ( CMAKE_GENERATOR_TOOLSET "Intel C++ Compiler 18.0" CACHE  INTERNAL  "Platform Toolset" FORCE)
	elseif(${USE_COMPILER} STREQUAL "Visual Studio C++ Compiler 17.0")
		set ( CMAKE_GENERATOR_TOOLSET "v141" CACHE  INTERNAL  "Platform Toolset" FORCE)
	endif()
endif()



if(${USE_CHRONO})
add_definitions ( -DNE_CHRONO )
endif()

if(${USE_BULLET} ) 
add_definitions ( -DNE_BULLET )
endif()

if(${USE_GRIDFLUID} ) 
add_definitions ( -DNE_GRIDFLUID)
endif()


if(${USE_VTK} ) 
add_definitions( -DNE_VTK)
endif()


if(${USE_COMM}) 
add_definitions( -DNE_COMM)
endif()

if(${USE_MEASURE}) 
add_definitions( -DNE_MEASURE)
endif()

if(${USE_IISPH} ) 
add_definitions( -DNE_IISPH)
endif()

if(${USE_NIISPH} ) 
add_definitions( -DNE_NIISPH)
endif()

if(${USE_NWCSPH} ) 
add_definitions( -DNE_NWCSPH)
endif()

if(${USE_VOLUME} ) 
add_definitions( -DNE_VOLUME )
endif()


if(${USE_COUPLING} ) 
add_definitions( -DNE_COUPLING )
endif()

if(${USE_FIELDS} ) 
add_definitions( -DNE_FIELDS )
endif()

if(${USE_ADDSOURCES} ) 
add_definitions( -DNE_ADDSOURCES )
endif()

if(${USE_SPRAY} ) 
add_definitions( -DNE_SPRAY )
endif()

if(${USE_GEOM}) 
add_definitions( -DNE_GEOM )
endif()

if(${USE_POST} ) 
add_definitions( -DNE_POST )
endif()

if(${USE_FIRE} ) 
add_definitions( -DNE_FIRE)
endif()


# Platform Specific Configurations #

if( WIN32 AND MSVC )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -openmp")
    if(${USE_COMPILER} STREQUAL "Intel Compiler 18.0")
        set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qopenmp")
    endif()
    set ( CMAKE_CXX_COMPILE_FEATURES "${CMAKE_CXX_COMPILE_FEATURES} _WIN32 _OPENMP")

    link_directories(${ROOTPATH}/External/Windows/lib)
	include_directories (${ROOTPATH}/External/Windows/include )
    add_definitions( -DNOMINMAX -D_OPENMP)
    add_definitions(/wd4100 /wd4661 /wd4503 /wd4005 /wd4267 /wd4273 /wd4133 /wd4146 /wd4002 /wd4138 /wd4189 /wd4521)

    if(${USE_COMPILER} STREQUAL "Intel Compiler 18.0")
		set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /bigobj /Wpch-messages-")
		set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /bigobj /Wpch-messages")
	else()
		set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /bigobj")
		set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /bigobj")
	endif()

    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD /bigobj" )
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /bigobj $$DISABLED_WARNINGS")

    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:LIBMT.lib" )
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:MSVCRT.lib" )

    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:LIBMT.lib" )
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:LIBCMT.lib /NODEFAULTLIB:MSVCRT.lib" )
    
endif()


if(UNIX)
    # DISABLED_WARNINGS = -Wno-conversion-null \
    #                   -Wno-return-type \
    #                   -Wno-maybe-unitialized \
    #                   -Wno-overloaded-virtual \
    #                   -Wno-unused-variable \
    #                   -Wno-reorder \
    #                   -Wno-sign-compare \
    #                   -Wno-address \
    #                   -Wno-switch \
    #                   -Wno-narrowing \
    #                   -Wno-strict-aliasing \
    #                   -Wno-unused-but-set-variable
endif()

if( UNIX AND NOT APPLE)
    # QMAKE_CXXFLAGS += -fopenmp \
    #                   -fPIC \
    #                   $$DISABLED_WARNINGS \
    #                   -isystem ../neutrino/External

    # if($$RELEASE) {
    #     QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
    #     QMAKE_CXXFLAGS_RELEASE -= -O2
    # } else {
    #     QMAKE_CXXFLAGS += -O0
    #     DEFINES += _DEBUG
    # }

    # QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/Libs\''
    # QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
    # LIBS += -L$$ROOTPATH/External/Linux/lib

    # if( $$USE_OPENCL ) {
    #     LIBS += -lcl
    # }
    # LIBS += -lgomp
endif()

if(APPLE)
    # QMAKE_CXXFLAGS += $$DISABLED_WARNINGS \
    #                  -isystem ../neutrino/External

    # if($$RELEASE) {
    #     QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
    #     QMAKE_CXXFLAGS_RELEASE -= -O2
    # } else {
    #     QMAKE_CXXFLAGS += -O0
    # }

    # QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/Libs\''
    # QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN/\''
    # LIBS += -L$$ROOTPATH/External/macosx/lib

    # LIBS += -lz \
    #         -framework OpenCL

endif()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/../Build )	


