# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_wasm_relaxed_consumer LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
find_program(NATIVE_WASM_NODE NAMES node REQUIRED)
foreach(interface IN ITEMS module main)
  add_executable(native_test_wasm_relaxed_${interface} ${interface}.cc)
  target_link_libraries(native_test_wasm_relaxed_${interface} PRIVATE native::native)
  target_compile_options(native_test_wasm_relaxed_${interface} PRIVATE -ffp-contract=off)
  set_target_properties(native_test_wasm_relaxed_${interface} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  add_test(NAME native.wasm.relaxed.${interface} COMMAND "${NATIVE_WASM_NODE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_relaxed_${interface}>)
  set_tests_properties(native.wasm.relaxed.${interface} PROPERTIES LABELS "wasm;modules;relaxed_simd")
endforeach()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(native_wasm_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_WASM_OBJDUMP NAMES llvm-objdump HINTS "${native_wasm_compiler_dir}" REQUIRED)
foreach(interface IN ITEMS raw public)
  add_library(native_test_wasm_relaxed_${interface} OBJECT ${interface}.cc)
  target_link_libraries(native_test_wasm_relaxed_${interface} PRIVATE native::native)
  target_compile_options(native_test_wasm_relaxed_${interface} PRIVATE -O2)
  set_target_properties(native_test_wasm_relaxed_${interface} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
endforeach()
add_test(NAME native.wasm.relaxed.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${NATIVE_WASM_OBJDUMP}"
  --raw $<TARGET_OBJECTS:native_test_wasm_relaxed_raw>
  --public $<TARGET_OBJECTS:native_test_wasm_relaxed_public>
  --engine $<TARGET_FILE:native_test_wasm_relaxed_dot_engine>
  --output "${CMAKE_CURRENT_BINARY_DIR}/codegen.log")
set_tests_properties(native.wasm.relaxed.codegen PROPERTIES LABELS "wasm;codegen;relaxed_simd")

foreach(operation RANGE 0 19)
  set(positive native_test_wasm_relaxed_positive_${operation})
  add_library(${positive} OBJECT EXCLUDE_FROM_ALL reject.cc)
  target_link_libraries(${positive} PRIVATE native::native)
  target_compile_definitions(${positive} PRIVATE NATIVE_RELAXED_OP=${operation})
  set_target_properties(${positive} PROPERTIES CXX_SCAN_FOR_MODULES ON)
  foreach(boundary IN ITEMS TARGET WEAK PRIVATE SHAPE)
    string(TOLOWER "${boundary}" name)
    set(negative native_test_wasm_relaxed_reject_${name}_${operation})
    add_library(${negative} OBJECT EXCLUDE_FROM_ALL reject.cc)
    target_link_libraries(${negative} PRIVATE native::native)
    target_compile_definitions(${negative} PRIVATE NATIVE_RELAXED_OP=${operation} NATIVE_REJECT_${boundary}=1)
    set_target_properties(${negative} PROPERTIES CXX_SCAN_FOR_MODULES ON)
    if(boundary STREQUAL "PRIVATE" AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      # Private intrinsic helpers are not an installed API.
      set_target_properties(${negative} PROPERTIES EXCLUDE_FROM_ALL TRUE)
      continue()
    endif()
    set(control "${positive}")
    if(boundary STREQUAL "PRIVATE")
      set(control native_test_wasm_relaxed_private_positive_${operation})
      add_library(${control} OBJECT EXCLUDE_FROM_ALL reject.cc)
      target_link_libraries(${control} PRIVATE native::native)
      target_compile_definitions(${control} PRIVATE
        NATIVE_RELAXED_OP=${operation} NATIVE_REJECT_PRIVATE=1 NATIVE_POSITIVE=1)
      set_target_properties(${control} PROPERTIES CXX_SCAN_FOR_MODULES ON)
    endif()
    if(boundary STREQUAL "TARGET")
      set(diagnostic "requires target feature 'relaxed-simd'")
    elseif(boundary STREQUAL "WEAK")
      set(diagnostic "consteval|immediate function")
    else()
      set(diagnostic "no matching function|constraints not satisfied")
    endif()
    add_test(NAME native.wasm.relaxed.reject.${name}.${operation} COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/check_negative.py" --cmake "${CMAKE_COMMAND}"
      --build "${CMAKE_BINARY_DIR}" --positive "${control}" --negative "${negative}"
      --config "$<CONFIG>" --diagnostic "${diagnostic}"
      --log "${CMAKE_CURRENT_BINARY_DIR}/reject-${name}-${operation}.log")
    set_tests_properties(native.wasm.relaxed.reject.${name}.${operation} PROPERTIES
      RUN_SERIAL TRUE LABELS "wasm;negative;relaxed_simd")
  endforeach()
endforeach()

add_executable(native_test_wasm_relaxed_dot_engine dot_engine.cc)
target_compile_features(native_test_wasm_relaxed_dot_engine PRIVATE cxx_std_26)
add_test(NAME native.wasm.relaxed.engine.dot COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_relaxed_dot_engine>)
set_tests_properties(native.wasm.relaxed.engine.dot PROPERTIES LABELS "wasm;engine-conformance;relaxed_simd")

find_program(NATIVE_WASM_WASMTIME NAMES wasmtime)
if(NATIVE_WASM_WASMTIME)
  foreach(interface IN ITEMS main dot_engine)
    add_test(NAME native.wasm.relaxed.wasmtime.${interface} COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=y -Wrelaxed-simd=y $<TARGET_FILE:native_test_wasm_relaxed_${interface}>)
    set_tests_properties(native.wasm.relaxed.wasmtime.${interface} PROPERTIES LABELS "wasm;wasmtime;relaxed_simd")
  endforeach()
  add_test(NAME native.wasm.relaxed.wasmtime.dot_deterministic COMMAND "${NATIVE_WASM_WASMTIME}"
    run -Ccache=n -Wsimd=y -Wrelaxed-simd=y -Wrelaxed-simd-deterministic=y
    $<TARGET_FILE:native_test_wasm_relaxed_dot_engine> --deterministic)
  set_tests_properties(native.wasm.relaxed.wasmtime.dot_engine
    native.wasm.relaxed.wasmtime.dot_deterministic PROPERTIES LABELS "wasm;wasmtime;engine-conformance;relaxed_simd")
endif()

add_executable(native_test_wasm_relaxed_laneselect_engine laneselect_engine.cc)
target_compile_features(native_test_wasm_relaxed_laneselect_engine PRIVATE cxx_std_26)
add_test(NAME native.wasm.relaxed.engine.laneselect COMMAND "${NATIVE_WASM_NODE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/run.mjs" $<TARGET_FILE:native_test_wasm_relaxed_laneselect_engine>)
set_tests_properties(native.wasm.relaxed.engine.laneselect PROPERTIES
  LABELS "wasm;engine-conformance;relaxed_simd")
if(NATIVE_WASM_WASMTIME)
  add_test(NAME native.wasm.relaxed.wasmtime.laneselect_engine COMMAND "${NATIVE_WASM_WASMTIME}"
    run -Ccache=n -Wsimd=y -Wrelaxed-simd=y $<TARGET_FILE:native_test_wasm_relaxed_laneselect_engine>)
  add_test(NAME native.wasm.relaxed.wasmtime.laneselect_deterministic COMMAND "${NATIVE_WASM_WASMTIME}"
    run -Ccache=n -Wsimd=y -Wrelaxed-simd=y -Wrelaxed-simd-deterministic=y
    $<TARGET_FILE:native_test_wasm_relaxed_laneselect_engine> --deterministic)
  set_tests_properties(native.wasm.relaxed.wasmtime.laneselect_engine
    native.wasm.relaxed.wasmtime.laneselect_deterministic
    PROPERTIES LABELS "wasm;wasmtime;engine-conformance;relaxed_simd")
endif()
