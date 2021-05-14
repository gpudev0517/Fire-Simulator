cmake_minimum_required(VERSION 3.1.0)

project(Neutrino)

add_subdirectory(Core)
add_subdirectory(Geometry)
add_subdirectory(NetCmd)
add_subdirectory(Plugins)
add_subdirectory(PostProcessor)
add_subdirectory(Rendering)
add_subdirectory(UI)
add_subdirectory(AppMain)

#set( VS_STARTUP_PROJECT AppMain )

