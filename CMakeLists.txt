cmake_minimum_required(VERSION 3.10)
project(banewfn VERSION 1.5.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
)

option(BUILD_STATIC "Build with static linking" ON)
if(BUILD_STATIC)
    if(MINGW)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()

option(BANEWFN_BUILD_TESTS "Build unit tests" OFF)

set(BANEWFN_PACKAGE_CONF_SOURCE "${CMAKE_SOURCE_DIR}/conf" CACHE PATH
    "Directory bundled as conf/ in release packages")
set(BANEWFN_PACKAGE_SCRIPT_SOURCE "${CMAKE_SOURCE_DIR}/scripts" CACHE PATH
    "Directory bundled as scripts/ in release packages")
set(BANEWFN_PACKAGE_DOC_SOURCE "${CMAKE_SOURCE_DIR}/docs/out" CACHE PATH
    "Directory bundled as docs/out/ in release packages")
set(BANEWFN_PACKAGE_DOC_PATTERN "*.pdf" CACHE STRING
    "Glob pattern copied from BANEWFN_PACKAGE_DOC_SOURCE into docs/out/")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    enable_language(RC)
endif()

# GCC 8 and older keep std::filesystem in a separate support library.
# Newer libstdc++/Clang/MSVC provide it through the normal C++ runtime.
set(BANEWFN_FILESYSTEM_LIB)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    list(APPEND BANEWFN_FILESYSTEM_LIB stdc++fs)
endif()

include(GNUInstallDirs)

add_subdirectory(src)
add_subdirectory(apps)

if(BANEWFN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include("${CMAKE_SOURCE_DIR}/cmake/BaneWfnInstall.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/BaneWfnPackaging.cmake")

message(STATUS "banewfn: Building with C++${CMAKE_CXX_STANDARD}")
if(BUILD_STATIC)
    message(STATUS "banewfn: Static linking enabled")
endif()
message(STATUS "banewfn: Release conf source = ${BANEWFN_PACKAGE_CONF_SOURCE}")
message(STATUS "banewfn: Release scripts source = ${BANEWFN_PACKAGE_SCRIPT_SOURCE}")
message(STATUS "banewfn: Release docs source = ${BANEWFN_PACKAGE_DOC_SOURCE}")
