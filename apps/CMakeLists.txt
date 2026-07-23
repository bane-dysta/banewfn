set(BANEWFN_APP_SOURCES banewfn/main.cpp)
set(BWPACK_APP_SOURCES bwpack/main.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND BANEWFN_APP_SOURCES
        "${CMAKE_SOURCE_DIR}/packaging/windows/resources/banewfn.rc")
    list(APPEND BWPACK_APP_SOURCES
        "${CMAKE_SOURCE_DIR}/packaging/windows/resources/bwpack.rc")
endif()

add_executable(banewfn ${BANEWFN_APP_SOURCES})
target_link_libraries(banewfn PRIVATE
    banewfn_core
    ${BANEWFN_FILESYSTEM_LIB}
)

add_executable(bwpack ${BWPACK_APP_SOURCES})
target_link_libraries(bwpack PRIVATE
    bwpack_core
    ${BANEWFN_FILESYSTEM_LIB}
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_include_directories(banewfn PRIVATE
        "${CMAKE_SOURCE_DIR}/packaging/windows/resources")
    target_include_directories(bwpack PRIVATE
        "${CMAKE_SOURCE_DIR}/packaging/windows/resources")
endif()

set_target_properties(banewfn bwpack PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
