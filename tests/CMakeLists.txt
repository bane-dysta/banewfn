add_executable(banewfn_tests
    test_main.cpp
    test_utils.cpp
    test_config.cpp
    test_input.cpp
    test_inline_conf.cpp
    $<TARGET_OBJECTS:banewfn_core>
)

target_include_directories(banewfn_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(banewfn_tests PRIVATE ${BANEWFN_FILESYSTEM_LIB})

set_target_properties(banewfn_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(NAME banewfn_unit_tests COMMAND $<TARGET_FILE:banewfn_tests>)
