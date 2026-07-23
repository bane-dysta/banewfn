if(NOT DEFINED BANEWFN_EXE)
    message(FATAL_ERROR "BANEWFN_EXE is required")
endif()

set(TEST_ROOT "${CMAKE_CURRENT_BINARY_DIR}/auto_citations_cli")
file(REMOVE_RECURSE "${TEST_ROOT}")
file(MAKE_DIRECTORY "${TEST_ROOT}")

function(run_case NAME INPUT_TEXT EXPECTED_FILE FORBIDDEN_FILE)
    set(CASE_DIR "${TEST_ROOT}/${NAME}")
    file(MAKE_DIRECTORY "${CASE_DIR}/conf")
    file(WRITE "${CASE_DIR}/banewfn.rc"
        "confpath=${CASE_DIR}/conf\n"
        "citations_output=default.bib\n")
    file(WRITE "${CASE_DIR}/workflow.bw" "${INPUT_TEXT}")

    execute_process(
        COMMAND "${BANEWFN_EXE}" workflow.bw
        WORKING_DIRECTORY "${CASE_DIR}"
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE STDOUT
        ERROR_VARIABLE STDERR
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR
            "${NAME}: banewfn failed with ${RESULT}\nstdout:\n${STDOUT}\nstderr:\n${STDERR}")
    endif()

    if(NOT "${EXPECTED_FILE}" STREQUAL "")
        if(NOT EXISTS "${CASE_DIR}/${EXPECTED_FILE}")
            message(FATAL_ERROR "${NAME}: expected ${EXPECTED_FILE} was not created")
        endif()
        file(READ "${CASE_DIR}/${EXPECTED_FILE}" BIB_CONTENT)
        if(NOT BIB_CONTENT MATCHES "@misc\\{demo")
            message(FATAL_ERROR "${NAME}: generated file is not the expected BibTeX record\n${BIB_CONTENT}")
        endif()
    endif()

    if(NOT "${FORBIDDEN_FILE}" STREQUAL "" AND EXISTS "${CASE_DIR}/${FORBIDDEN_FILE}")
        message(FATAL_ERROR "${NAME}: unexpected ${FORBIDDEN_FILE} was created")
    endif()
endfunction()

set(CITATION_BLOCK [=[bane.cite demo {
    authors = "Alpha One"
    title = "Automatic Citation"
    year = 2026
}
]=])

run_case(default_output "${CITATION_BLOCK}" "default.bib" "")
run_case(file_override "citations_output=custom.bib\n${CITATION_BLOCK}" "custom.bib" "default.bib")
run_case(file_disable "citations_output=off\n${CITATION_BLOCK}" "" "default.bib")
run_case(explicit_output "${CITATION_BLOCK}\nbane.citations.write manual {\n    output = manual.bib\n    format = bibtex\n}\n" "manual.bib" "default.bib")

set(MERGE_DIR "${TEST_ROOT}/merge_across_invocations")
file(MAKE_DIRECTORY "${MERGE_DIR}/conf")
file(WRITE "${MERGE_DIR}/banewfn.rc"
    "confpath=${MERGE_DIR}/conf\n"
    "citations_output=references.bib\n")
file(WRITE "${MERGE_DIR}/first.bw" [=[bane.cite multiwfn {
    title = "Multiwfn"
    doi = "10.1002/jcc.22885"
    reason = "AAA"
}
]=])
file(WRITE "${MERGE_DIR}/second.bw" [=[bane.cite lu_igmh_2022 {
    title = "IGMH"
    doi = "10.1002/jcc.26812"
    reason = "IGMH"
}
]=])

foreach(INPUT_FILE IN ITEMS first.bw second.bw)
    execute_process(
        COMMAND "${BANEWFN_EXE}" "${INPUT_FILE}"
        WORKING_DIRECTORY "${MERGE_DIR}"
        RESULT_VARIABLE RESULT
        OUTPUT_VARIABLE STDOUT
        ERROR_VARIABLE STDERR
    )
    if(NOT RESULT EQUAL 0)
        message(FATAL_ERROR
            "merge_across_invocations/${INPUT_FILE}: banewfn failed with ${RESULT}\nstdout:\n${STDOUT}\nstderr:\n${STDERR}")
    endif()
endforeach()

file(READ "${MERGE_DIR}/references.bib" MERGED_BIB)
if(EXISTS "${MERGE_DIR}/reference.bib")
    message(FATAL_ERROR "merge_across_invocations: unexpected misspelled output reference.bib")
endif()
foreach(EXPECTED IN ITEMS
        "baneid = {multiwfn}"
        "banereason1 = {AAA}"
        "@misc{lu_igmh_2022"
        "banereason1 = {IGMH}")
    string(FIND "${MERGED_BIB}" "${EXPECTED}" FOUND_AT)
    if(FOUND_AT EQUAL -1)
        message(FATAL_ERROR "merge_across_invocations: missing '${EXPECTED}'\n${MERGED_BIB}")
    endif()
endforeach()
