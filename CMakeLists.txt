cmake_minimum_required(VERSION 3.10)
project(banewfn LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings
add_compile_options(-Wall -Wextra)

# Static build option
option(BUILD_STATIC "Build with static linking" ON)
if(BUILD_STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
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

# banewfn (main runner)
add_executable(banewfn
    $<TARGET_OBJECTS:banewfn_core>
    src/banewfn.cpp
    src/ui.cpp
    src/ui.h
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

# Install
install(TARGETS banewfn bwpack
    RUNTIME DESTINATION bin
)

message(STATUS "banewfn: Building with C++${CMAKE_CXX_STANDARD}")
if(BUILD_STATIC)
    message(STATUS "banewfn: Static linking enabled")
endif()
