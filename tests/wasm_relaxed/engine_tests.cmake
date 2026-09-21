# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
# Shared by source/package tests and the artifact-only CI conformance job.
function(native_add_wasm_engine_tests dot_module laneselect_module)
  add_test(NAME native.wasm.relaxed.engine.dot COMMAND "${NATIVE_WASM_NODE}"
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/run.mjs" "${dot_module}")
  add_test(NAME native.wasm.relaxed.engine.laneselect COMMAND "${NATIVE_WASM_NODE}"
    "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/run.mjs" "${laneselect_module}")
  set_tests_properties(native.wasm.relaxed.engine.dot
    native.wasm.relaxed.engine.laneselect PROPERTIES
    LABELS "wasm;engine-conformance;relaxed_simd")

  if(NATIVE_WASM_WASMTIME)
    add_test(NAME native.wasm.relaxed.wasmtime.dot_engine COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=y -Wrelaxed-simd=y "${dot_module}")
    add_test(NAME native.wasm.relaxed.wasmtime.dot_deterministic COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=y -Wrelaxed-simd=y -Wrelaxed-simd-deterministic=y
      "${dot_module}" --deterministic)
    add_test(NAME native.wasm.relaxed.wasmtime.laneselect_engine COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=y -Wrelaxed-simd=y "${laneselect_module}")
    add_test(NAME native.wasm.relaxed.wasmtime.laneselect_deterministic COMMAND "${NATIVE_WASM_WASMTIME}"
      run -Ccache=n -Wsimd=y -Wrelaxed-simd=y -Wrelaxed-simd-deterministic=y
      "${laneselect_module}" --deterministic)
    set_tests_properties(native.wasm.relaxed.wasmtime.dot_engine
      native.wasm.relaxed.wasmtime.dot_deterministic
      native.wasm.relaxed.wasmtime.laneselect_engine
      native.wasm.relaxed.wasmtime.laneselect_deterministic PROPERTIES
      LABELS "wasm;wasmtime;engine-conformance;relaxed_simd")
  endif()
endfunction()
