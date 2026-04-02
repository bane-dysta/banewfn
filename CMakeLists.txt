cmake_minimum_required(VERSION 3.10)
project(banewfn VERSION 1.3.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings (only for C++)
add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
)

# Static build option
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
set(BANEWFN_PACKAGE_DOC_SOURCE "${CMAKE_SOURCE_DIR}/docs/out" CACHE PATH
    "Directory bundled as docs/out/ in release packages")
set(BANEWFN_PACKAGE_DOC_PATTERN "*.pdf" CACHE STRING
    "Glob pattern copied from BANEWFN_PACKAGE_DOC_SOURCE into docs/out/")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    enable_language(RC)
endif()

# Core sources shared by tools
set(CORE_SOURCES
    src/config.cpp
    src/input.cpp
    src/utils.cpp
    src/inline_conf.cpp
)

set(CORE_HEADERS
    src/config.h
    src/input.h
    src/utils.h
    src/inline_conf.h
)

add_library(banewfn_core OBJECT ${CORE_SOURCES} ${CORE_HEADERS})

set(BANEWFN_WINDOWS_RESOURCE)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND BANEWFN_WINDOWS_RESOURCE src/banewfn.rc)
endif()

# banewfn (main runner)
add_executable(banewfn
    $<TARGET_OBJECTS:banewfn_core>
    src/banewfn.cpp
    src/ui.cpp
    src/ui.h
    ${BANEWFN_WINDOWS_RESOURCE}
)

# bwpack (bundler)
add_executable(bwpack
    $<TARGET_OBJECTS:banewfn_core>
    src/bwpack.cpp
)

# Output directory
set_target_properties(banewfn bwpack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(BANEWFN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install runtime files directly at package root.
install(TARGETS banewfn bwpack
    RUNTIME DESTINATION .
)

if(IS_DIRECTORY "${BANEWFN_PACKAGE_CONF_SOURCE}")
    install(DIRECTORY "${BANEWFN_PACKAGE_CONF_SOURCE}/"
        DESTINATION conf
    )
else()
    message(WARNING "banewfn: conf source directory does not exist and will not be packaged: ${BANEWFN_PACKAGE_CONF_SOURCE}")
endif()

if(IS_DIRECTORY "${BANEWFN_PACKAGE_DOC_SOURCE}")
    file(GLOB BANEWFN_PACKAGE_MANUALS "${BANEWFN_PACKAGE_DOC_SOURCE}/${BANEWFN_PACKAGE_DOC_PATTERN}")
    if(BANEWFN_PACKAGE_MANUALS)
        install(DIRECTORY "${BANEWFN_PACKAGE_DOC_SOURCE}/"
            DESTINATION docs/out
            FILES_MATCHING
            PATTERN "${BANEWFN_PACKAGE_DOC_PATTERN}"
        )
    else()
        message(WARNING "banewfn: no manuals matched ${BANEWFN_PACKAGE_DOC_PATTERN} under ${BANEWFN_PACKAGE_DOC_SOURCE}; docs/out will be empty in packages")
    endif()
else()
    message(WARNING "banewfn: docs source directory does not exist and will not be packaged: ${BANEWFN_PACKAGE_DOC_SOURCE}")
endif()

# Packaging (CPack)
set(CPACK_PACKAGE_NAME "banewfn")
set(CPACK_PACKAGE_VENDOR "Bane Dysta")
set(CPACK_PACKAGE_CONTACT "https://github.com/bane-dysta/banewfn")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "BaneWfn release bundle")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_VERBATIM_VARIABLES YES)
set(CPACK_MONOLITHIC_INSTALL ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(BANEWFN_PACKAGE_PLATFORM "windows-x86_64")
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(BANEWFN_PACKAGE_PLATFORM "linux-x86_64")
    set(CPACK_GENERATOR "STGZ")
else()
    string(TOLOWER "${CMAKE_SYSTEM_NAME}" BANEWFN_SYSTEM_NAME_LOWER)
    set(BANEWFN_PACKAGE_PLATFORM "${BANEWFN_SYSTEM_NAME_LOWER}-${CMAKE_SYSTEM_PROCESSOR}")
    set(CPACK_GENERATOR "TGZ")
endif()

set(CPACK_PACKAGE_FILE_NAME "banewfn-${PROJECT_VERSION}-${BANEWFN_PACKAGE_PLATFORM}")

include(CPack)

message(STATUS "banewfn: Building with C++${CMAKE_CXX_STANDARD}")
if(BUILD_STATIC)
    message(STATUS "banewfn: Static linking enabled")
endif()
message(STATUS "banewfn: Release conf source = ${BANEWFN_PACKAGE_CONF_SOURCE}")
message(STATUS "banewfn: Release docs source = ${BANEWFN_PACKAGE_DOC_SOURCE}")
