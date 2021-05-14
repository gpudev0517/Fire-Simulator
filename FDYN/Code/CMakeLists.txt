cmake_minimum_required(VERSION 3.1.0)

project(FDYN)


add_subdirectory(UI)
add_subdirectory(AppMain)

set( VS_STARTUP_PROJECT AppMain )

