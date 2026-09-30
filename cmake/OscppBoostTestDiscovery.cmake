# Register each Boost.Test test case of an executable as its own CTest test, so `ctest -R <suite>` or
# `ctest -R <suite>/<case>` runs just those tests. Like gtest_discover_tests(), discovery runs after the executable is
# built, by listing its contents (`--list_content=HRF`), so new test cases need no CMake change.
#
#   oscpp_boost_test_discover_tests(<target> [LABELS <label>...])
#
# Tests are named <suite>/<case> and run `<target> --run_test=<suite>/<case>`.

set(_OSCPP_BOOST_TEST_DISCOVERY_SCRIPT "${CMAKE_CURRENT_LIST_FILE}")

function(oscpp_boost_test_discover_tests TARGET)
    cmake_parse_arguments(ARG "" "" "LABELS" ${ARGN})
    set(tests_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_tests.cmake")
    set(include_file "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_include.cmake")

    add_custom_command(TARGET ${TARGET} POST_BUILD
            BYPRODUCTS "${tests_file}"
            COMMAND "${CMAKE_COMMAND}"
                -D "OSCPP_DISCOVER=ON"
                -D "TEST_EXECUTABLE=$<TARGET_FILE:${TARGET}>"
                -D "TESTS_FILE=${tests_file}"
                -D "TEST_LABELS=${ARG_LABELS}"
                -P "${_OSCPP_BOOST_TEST_DISCOVERY_SCRIPT}"
            COMMENT "Discovering Boost.Test test cases in ${TARGET}"
            VERBATIM)

    file(WRITE "${include_file}" "\
if(EXISTS \"${tests_file}\")
    include(\"${tests_file}\")
else()
    add_test(${TARGET}_NOT_BUILT ${TARGET}_NOT_BUILT)
endif()
")
    set_property(DIRECTORY APPEND PROPERTY TEST_INCLUDE_FILES "${include_file}")
endfunction()

# Script mode: list the test cases and write the add_test() commands.
if(OSCPP_DISCOVER)
    execute_process(COMMAND "${TEST_EXECUTABLE}" --list_content=HRF
            RESULT_VARIABLE result
            OUTPUT_VARIABLE standard_output
            ERROR_VARIABLE standard_error)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Listing the tests of ${TEST_EXECUTABLE} failed (${result}):\n${standard_output}${standard_error}")
    endif()

    # Boost.Test prints the listing on stderr. Suites and cases are indented four spaces per nesting level, and an
    # enabled item is followed by '*'.
    string(REPLACE "\n" ";" lines "${standard_output}${standard_error}")
    set(path "")
    set(script "")
    foreach(line IN LISTS lines)
        if(NOT line MATCHES "^( *)([^ *:]+)\\*?$")
            continue()
        endif()
        string(LENGTH "${CMAKE_MATCH_1}" indent)
        math(EXPR depth "${indent} / 4")
        set(name "${CMAKE_MATCH_2}")
        # Keep only the ancestors of this item, then add it.
        set(kept "")
        set(index 0)
        foreach(ancestor IN LISTS path)
            if(index LESS depth)
                list(APPEND kept "${ancestor}")
            endif()
            math(EXPR index "${index} + 1")
        endforeach()
        set(path "${kept}")
        list(APPEND path "${name}")
        # A line is a test case if the next non-empty line is not indented deeper; decide after the loop by
        # remembering every candidate and dropping those that turn out to be suites.
        list(JOIN path "/" full_name)
        list(APPEND candidates "${depth}|${full_name}")
    endforeach()

    list(LENGTH candidates count)
    math(EXPR last "${count} - 1")
    foreach(i RANGE ${last})
        list(GET candidates ${i} candidate)
        string(REGEX MATCH "^([0-9]+)\\|(.*)$" ignored "${candidate}")
        set(depth "${CMAKE_MATCH_1}")
        set(full_name "${CMAKE_MATCH_2}")
        set(is_suite FALSE)
        math(EXPR next "${i} + 1")
        if(next LESS count)
            list(GET candidates ${next} next_candidate)
            string(REGEX MATCH "^([0-9]+)\\|" ignored "${next_candidate}")
            if(CMAKE_MATCH_1 GREATER depth)
                set(is_suite TRUE)
            endif()
        endif()
        if(NOT is_suite)
            string(APPEND script "add_test([=[${full_name}]=] [=[${TEST_EXECUTABLE}]=] [=[--run_test=${full_name}]=])\n")
            if(TEST_LABELS)
                string(APPEND script "set_tests_properties([=[${full_name}]=] PROPERTIES LABELS [=[${TEST_LABELS}]=])\n")
            endif()
        endif()
    endforeach()
    file(WRITE "${TESTS_FILE}" "${script}")
endif()
