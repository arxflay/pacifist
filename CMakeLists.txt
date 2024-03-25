cmake_minimum_required(VERSION 3.0...3.10.2)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake")
project(pacifist VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES "")
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PacifistGame.cpp
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
message(STATUS ${CMAKE_MODULE_PATH})
include("${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake")
add_executable(pacifist ${SOURCES})
target_link_libraries(pacifist PRIVATE arxengine)
target_include_directories(pacifist PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(pacifist PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17;>"
                                                        "$<${msvc_cxx}:/W3;/wd4251;/wd4996;/std:c++17>")
