# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(polyfill_float_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(polyfill_float_objdump NAMES llvm-objdump HINTS "${polyfill_float_compiler_dir}" REQUIRED)
add_executable(native_test_polyfill_x86_f16c f16c.cc)
target_link_libraries(native_test_polyfill_x86_f16c PRIVATE native::native)
add_test(NAME native.polyfill.x86.f16c COMMAND native_test_polyfill_x86_f16c)
set_tests_properties(native.polyfill.x86.f16c PROPERTIES LABELS "host;polyfill;x86;f16c")

add_executable(native_test_polyfill_x86_avxneconvert ../x86_avxneconvert/runtime.cc)
target_link_libraries(native_test_polyfill_x86_avxneconvert PRIVATE native::native)
target_compile_definitions(native_test_polyfill_x86_avxneconvert PRIVATE NATIVE_TEST_POLYFILL=1)
add_test(NAME native.polyfill.x86.avxneconvert COMMAND native_test_polyfill_x86_avxneconvert)
set_tests_properties(native.polyfill.x86.avxneconvert PROPERTIES LABELS "host;polyfill;x86;avxneconvert")

foreach(family IN ITEMS f16c avxneconvert_0 avxneconvert_1)
  set(source ../x86_f16c/codegen_public.cc)
  set(reference native_test_x86_f16c_codegen_public)
  if(family MATCHES "avxneconvert_([01])")
    set(profile ${CMAKE_MATCH_1})
    set(source ../x86_avxneconvert/codegen.cc)
    set(reference native_test_x86_avxneconvert_public_${profile})
  endif()
  set(target native_polyfill_float_codegen_${family})
  add_library(${target} OBJECT ${source})
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_options(${target} PRIVATE "$<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/O2,-O2>")
  target_compile_definitions(${target} PRIVATE NATIVE_TEST_CODEGEN_POLYFILL=1)
  if(family MATCHES "avxneconvert")
    target_compile_definitions(${target} PRIVATE NATIVE_NE_PROFILE=${profile})
  endif()
  add_test(NAME native.polyfill.x86.codegen_${family} COMMAND "${Python3_EXECUTABLE}"
    "${PROJECT_SOURCE_DIR}/tests/x86_codegen_pairs.py" --objdump "${polyfill_float_objdump}"
    --raw "$<TARGET_OBJECTS:${reference}>" --public "$<TARGET_OBJECTS:${target}>"
    --output "${CMAKE_CURRENT_BINARY_DIR}/${family}-native-first-$<CONFIG>.txt")
  set_tests_properties(native.polyfill.x86.codegen_${family} PROPERTIES LABELS "host;polyfill;x86;codegen")
endforeach()

foreach(mode IN ITEMS native polyfill)
  set(target native_polyfill_float_storage_${mode})
  add_library(${target} OBJECT instruction_storage.cc)
  target_link_libraries(${target} PRIVATE native::native)
  target_compile_options(${target} PRIVATE "$<IF:$<CXX_COMPILER_FRONTEND_VARIANT:MSVC>,/O2,-O2>")
  if(mode STREQUAL "polyfill")
    target_compile_definitions(${target} PRIVATE NATIVE_TEST_CODEGEN_POLYFILL=1)
  endif()
endforeach()
add_test(NAME native.polyfill.x86.codegen_float_storage COMMAND "${Python3_EXECUTABLE}"
  "${PROJECT_SOURCE_DIR}/tests/x86_codegen_pairs.py" --objdump "${polyfill_float_objdump}"
  --raw "$<TARGET_OBJECTS:native_polyfill_float_storage_native>"
  --public "$<TARGET_OBJECTS:native_polyfill_float_storage_polyfill>"
  --operation-pattern "^vcvt(ph2ps|ps2ph)\\b"
  --output "${CMAKE_CURRENT_BINARY_DIR}/float-storage-native-first-$<CONFIG>.txt")
set_tests_properties(native.polyfill.x86.codegen_float_storage PROPERTIES LABELS "host;polyfill;x86;codegen")
