add_executable(banewfn_tests
    test_main.cpp
    test_utils.cpp
    test_citation.cpp
    test_bwpack.cpp
    test_config.cpp
    test_grep.cpp
    test_input.cpp
    test_inline_conf.cpp
)

target_include_directories(banewfn_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(banewfn_tests PRIVATE
    banewfn_core
    bwpack_core
    banewfn_common
)

set_target_properties(banewfn_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_test(NAME banewfn_unit_tests COMMAND $<TARGET_FILE:banewfn_tests>)

add_test(
    NAME banewfn_auto_citations_cli
    COMMAND ${CMAKE_COMMAND}
        -DBANEWFN_EXE=$<TARGET_FILE:banewfn>
        -P ${CMAKE_CURRENT_SOURCE_DIR}/test_auto_citations.cmake
)
