# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(suite IN ITEMS header module main)
  set(target native_test_x86_bmi2_${suite})
  add_executable(${target} ${suite}.cc)
  if(suite STREQUAL "main")
    target_link_libraries(${target} PRIVATE native::native)
  else()
    target_link_libraries(${target} PRIVATE native::minimal)
  endif()
  set_target_properties(${target} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(${target})
  endif()
  add_test(NAME native.x86.bmi2.${suite} COMMAND ${target})
  set_tests_properties(native.x86.bmi2.${suite} PROPERTIES
    LABELS "host;modules;baseline;bmi2" SKIP_RETURN_CODE 77)
endforeach()

add_library(native_test_x86_bmi2_codegen OBJECT codegen.cc)
target_link_libraries(native_test_x86_bmi2_codegen PRIVATE native::headers)
target_compile_features(native_test_x86_bmi2_codegen PRIVATE cxx_std_26)
set_target_properties(native_test_x86_bmi2_codegen PROPERTIES
  CXX_SCAN_FOR_MODULES OFF INTERPROCEDURAL_OPTIMIZATION FALSE)

# Keep module consumers on the provider's baseline flags so CMake shares BMIs.
# The independent textual probe can explicitly disable optional ISA features.
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_bmi2_codegen PRIVATE
    /clang:-mno-bmi /clang:-mno-bmi2 /clang:-mno-avx /clang:-mno-avx2 /clang:-mno-fma)
else()
  target_compile_options(native_test_x86_bmi2_codegen PRIVATE
    -mno-bmi -mno-bmi2 -mno-avx -mno-avx2 -mno-fma)
endif()
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_bmi2_codegen PRIVATE /O2)
else()
  target_compile_options(native_test_x86_bmi2_codegen PRIVATE -O2)
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
get_filename_component(bmi2_compiler_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(bmi2_objdump NAMES llvm-objdump HINTS "${bmi2_compiler_dir}" REQUIRED)
add_test(NAME native.x86.bmi2.codegen COMMAND "${Python3_EXECUTABLE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/check_codegen.py" --objdump "${bmi2_objdump}"
  --output "${CMAKE_CURRENT_BINARY_DIR}/bmi2-$<CONFIG>.disassembly"
  $<TARGET_OBJECTS:native_test_x86_bmi2_codegen> COMMAND_EXPAND_LISTS)
set_tests_properties(native.x86.bmi2.codegen PROPERTIES LABELS "host;baseline;bmi2;codegen")
