cmake_minimum_required(VERSION 3.1.0)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


if(UNIX AND NOT APPLE)
	include_directories( ${CMAKE_CURRENT_LIST_DIR}/../External/Linux/include )
elseif(APPLE)
	include_directories( ${CMAKE_CURRENT_LIST_DIR}/../External/macosx/include )
	link_directories(${CMAKE_CURRENT_LIST_DIR}/../External/macosx/lib)
endif()

if(UNIX)
	# CONFIG(release, debug|release) {
	# 	QMAKE_CXXFLAGS += -Ofast -Wno-deprecated
	# 	QMAKE_CXXFLAGS_RELEASE -= -O2
	# }else {
	# 	QMAKE_CXXFLAGS += -O0
	# }
elseif (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od" )
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox" )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS )
	include_directories ( ${CMAKE_CURRENT_LIST_DIR}/../External/windows/include )
	link_directories ( ${CMAKE_CURRENT_LIST_DIR}/../External/windows/lib )
	remove_definitions(-DUNICODE)
endif()
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/../Build )	

set ( INSTALL_INC_DIR ${CMAKE_CURRENT_LIST_DIR}/../External/include )

