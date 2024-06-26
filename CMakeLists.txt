cmake_minimum_required(VERSION 3.0...3.10.2)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake")
project(pacifist VERSION ${VER_MAJOR}.${VER_MINOR} LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SOURCES "")
list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PacifistGameWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/PacifistGame.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FontManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CheckBox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Player.cpp
)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/libs/ProjectLibs.cmake")
if(WIN32)
	add_executable(pacifist WIN32 ${SOURCES})
    target_sources(pacifist PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/rc/pacifist.rc"
    "${CMAKE_CURRENT_SOURCE_DIR}/rc/resource.h")
else()
    add_executable(pacifist ${SOURCES})
endif()
target_link_libraries(pacifist PUBLIC arxengine)
target_include_directories(pacifist PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(pacifist PRIVATE "$<${gcc_like_cxx}:-Wall;-Wextra;-Wconversion;-std=c++17;>"
                                                        "$<${msvc_cxx}:/W3;/wd4251;/wd4996;/std:c++17>")

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CPACK_PACKAGE_NAME pacifist-dev)
    set(CPACK_DEBIAN_PACKAGE_CONFLICTS pacifist)
else()
    set(CPACK_PACKAGE_NAME pacifist)
    set(CPACK_DEBIAN_PACKAGE_CONFLICTS pacifist-dev)
endif()                                       
set(CPACK_PACKAGE_VENDOR arxflay)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CPACK_PACKAGE_VERSION_MAJOR ${VER_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VER_MINOR})
set(CPACK_PACKAGE_VERSION ${VER_MAJOR}.${VER_MINOR})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${PKG_VER_NAME}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_SECTION games)
set(CPACK_DEBIAN_PACKAGE_SOURCE "${CPACK_PACKAGE_NAME} (${CMAKE_PACKAGE_VERSION})")
set(CPACK_DEBIAN_PACKAGE_PROVIDES ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Pacifist - 2D platformer")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexej Fedorenko(arxflay)")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set_target_properties(pacifist PROPERTIES DEBUG_POSTFIX "d")
if (NOT PACIFIST_DISABLE_INSTALL)
    install(TARGETS pacifist)
    if (UNIX)
        set(CMAKE_INSTALL_PREFIX "/usr")
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "share/${CMAKE_PROJECT_NAME}")
    else()
        install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${CMAKE_INSTALL_BINDIR}/..")
    endif()
endif()

if (PAC_USE_RELATIVE_PATH)
    target_compile_definitions(pacifist PRIVATE -DPAC_USE_RELATIVE_PATH)
endif()
include(InstallRequiredSystemLibraries)

#this must be a last entry in this file
include(CPack)
