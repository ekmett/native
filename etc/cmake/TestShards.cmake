# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

set(NATIVE_TEST_SHARD 0 CACHE STRING "Test build shard (0 builds the complete suite)")
set(NATIVE_TEST_SHARD_COUNT 6 CACHE STRING "Number of test build shards (2 or 6)")
if(NOT NATIVE_TEST_SHARD MATCHES "^[0-6]$" OR
    NOT NATIVE_TEST_SHARD_COUNT MATCHES "^(2|6)$" OR
    NATIVE_TEST_SHARD GREATER NATIVE_TEST_SHARD_COUNT)
  message(FATAL_ERROR "Select shard 0 (all), or 1..NATIVE_TEST_SHARD_COUNT (2 or 6)")
endif()
file(READ "${CMAKE_CURRENT_LIST_DIR}/test-shards.json" native_test_shards)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_LIST_DIR}/test-shards.json")

function(native_test_group group result)
  string(JSON bucket ERROR_VARIABLE error GET "${native_test_shards}" "${group}")
  if(error)
    message(FATAL_ERROR "Assign new test group '${group}' in etc/cmake/test-shards.json")
  endif()
  math(EXPR shard "(${bucket} - 1) % ${NATIVE_TEST_SHARD_COUNT} + 1")
  if(NATIVE_TEST_SHARD EQUAL 0 OR NATIVE_TEST_SHARD EQUAL shard)
    set(${result} TRUE PARENT_SCOPE)
    set_property(GLOBAL APPEND PROPERTY NATIVE_SELECTED_TEST_GROUPS "${group}")
  else()
    set(${result} FALSE PARENT_SCOPE)
  endif()
endfunction()

function(native_test_directory directory)
  # Core has two expensive independent pairs, selected inside its CMakeLists.
  if(directory STREQUAL "core_regression")
    add_subdirectory("tests/${directory}")
    return()
  endif()
  native_test_group("${directory}" selected)
  if(selected)
    add_subdirectory("tests/${directory}")
  endif()
endfunction()
