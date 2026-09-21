# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
project(native_wasm_engine_conformance NONE)
enable_testing()

find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
find_program(NATIVE_WASM_WASMTIME NAMES wasmtime REQUIRED)
set(NATIVE_WASM_ENGINE_MODULE_DIR "" CACHE PATH "Directory containing the built raw Wasm probes")
foreach(module IN ITEMS dot laneselect)
  if(NOT EXISTS "${NATIVE_WASM_ENGINE_MODULE_DIR}/${module}.wasm")
    message(FATAL_ERROR "Missing raw engine module: ${NATIVE_WASM_ENGINE_MODULE_DIR}/${module}.wasm")
  endif()
endforeach()

include("${CMAKE_CURRENT_LIST_DIR}/../engine_tests.cmake")
native_add_wasm_engine_tests("${NATIVE_WASM_ENGINE_MODULE_DIR}/dot.wasm"
  "${NATIVE_WASM_ENGINE_MODULE_DIR}/laneselect.wasm")
