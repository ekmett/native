# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
find_program(NATIVE_WASM_WASMTIME NAMES wasmtime REQUIRED)
add_executable(native_test_wasm_simd main.cc)
target_link_libraries(native_test_wasm_simd PRIVATE native::native)
set_target_properties(native_test_wasm_simd PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd COMMAND "${NATIVE_WASM_NODE}" "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd>)
set_tests_properties(native.wasm.simd PROPERTIES SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128")

add_library(native_test_wasm_simd_codegen OBJECT codegen.cc)
target_link_libraries(native_test_wasm_simd_codegen PRIVATE native::native)
target_compile_options(native_test_wasm_simd_codegen PRIVATE -O2)
set_target_properties(native_test_wasm_simd_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(wasm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(wasm_objdump NAMES llvm-objdump HINTS "${wasm_compiler_dir}" REQUIRED)
add_test(NAME native.wasm.simd.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${wasm_objdump}"
  --source "${CMAKE_CURRENT_SOURCE_DIR}/codegen.cc"
  --output "${CMAKE_CURRENT_BINARY_DIR}/codegen-$<CONFIG>.json"
  $<TARGET_OBJECTS:native_test_wasm_simd_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.wasm.simd.codegen PROPERTIES LABELS "wasm;simd128;codegen")

add_executable(native_test_wasm_simd_clients clients.cc)
target_link_libraries(native_test_wasm_simd_clients PRIVATE native::native)
set_target_properties(native_test_wasm_simd_clients PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd.clients COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd_clients>)
set_tests_properties(native.wasm.simd.clients PROPERTIES
  SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128;wide")

add_executable(native_test_wasm_simd_api api.cc)
target_link_libraries(native_test_wasm_simd_api PRIVATE native::native)
set_target_properties(native_test_wasm_simd_api PROPERTIES CXX_SCAN_FOR_MODULES ON)
add_test(NAME native.wasm.simd.api COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_simd_api>)
set_tests_properties(native.wasm.simd.api PROPERTIES
  SKIP_RETURN_CODE 77 LABELS "wasm;modules;simd128;api")

# Permission-only consumers run even when the engine rejects every SIMD opcode.
foreach(kind IN ITEMS header module)
  foreach(mode IN ITEMS scalar mixed)
    add_executable(native_test_wasm_polyfill_${mode}_${kind} polyfill.cc)
    target_link_libraries(native_test_wasm_polyfill_${mode}_${kind} PRIVATE native::native)
    if(kind STREQUAL "header")
      target_compile_definitions(native_test_wasm_polyfill_${mode}_${kind} PRIVATE NATIVE_POLYFILL_HEADERS=1)
    endif()
    if(mode STREQUAL "scalar")
      target_compile_definitions(native_test_wasm_polyfill_${mode}_${kind} PRIVATE NATIVE_POLYFILL_SCALAR_ONLY=1)
      set(engine_simd n)
    else()
      target_compile_options(native_test_wasm_polyfill_${mode}_${kind} PRIVATE -msimd128 -mno-relaxed-simd)
      set(engine_simd y)
    endif()
    add_test(NAME native.wasm.polyfill.${mode}_${kind} COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=${engine_simd} -Wrelaxed-simd=n $<TARGET_FILE:native_test_wasm_polyfill_${mode}_${kind}>)
    set_tests_properties(native.wasm.polyfill.${mode}_${kind} PROPERTIES LABELS "wasm;polyfill;${mode};${kind}" TIMEOUT 30)
  endforeach()
endforeach()

# The shared SIMD corpus exercises tails, masks, custom storage, memory and math.
foreach(mode IN ITEMS scalar mixed)
  add_executable(native_test_wasm_polyfill_core_${mode} ../polyfill/main.cc)
  target_link_libraries(native_test_wasm_polyfill_core_${mode} PRIVATE native::native)
  target_compile_definitions(native_test_wasm_polyfill_core_${mode} PRIVATE NATIVE_POLYFILL_HEADERS=1)
  if(mode STREQUAL "scalar")
    target_compile_definitions(native_test_wasm_polyfill_core_${mode} PRIVATE NATIVE_POLYFILL_SCALAR_ONLY=1)
    set(engine_simd n)
  else()
    target_compile_options(native_test_wasm_polyfill_core_${mode} PRIVATE -msimd128 -mno-relaxed-simd)
    set(engine_simd y)
  endif()
  add_test(NAME native.wasm.polyfill.core_${mode} COMMAND "${NATIVE_WASM_WASMTIME}"
    run -Ccache=n -Wsimd=${engine_simd} -Wrelaxed-simd=n $<TARGET_FILE:native_test_wasm_polyfill_core_${mode}>)
  set_tests_properties(native.wasm.polyfill.core_${mode} PROPERTIES LABELS "wasm;polyfill;${mode};memory;math" TIMEOUT 30)
endforeach()
