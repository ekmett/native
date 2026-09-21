# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
cmake_minimum_required(VERSION 4.4)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(native_x86_integer_constexpr LANGUAGES CXX)
  find_package(native CONFIG REQUIRED COMPONENTS native)
  enable_testing()
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "^(AMD64|amd64|x86_64|X86_64)$")
  return()
endif()
foreach(family IN ITEMS pclmul gfni vpopcntdq vnni)
  add_executable(native_test_x86_integer_constexpr_${family} ${family}.cc)
  target_link_libraries(native_test_x86_integer_constexpr_${family} PRIVATE native::native)
  set_target_properties(native_test_x86_integer_constexpr_${family} PROPERTIES
    CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
  if(COMMAND native_link_clang_cl_asan)
    native_link_clang_cl_asan(native_test_x86_integer_constexpr_${family})
  endif()
  add_test(NAME native.x86.integer_constexpr.${family}
    COMMAND native_test_x86_integer_constexpr_${family})
  set_tests_properties(native.x86.integer_constexpr.${family} PROPERTIES
    SKIP_RETURN_CODE 77 LABELS "host;modules;constexpr;property;x86;${family}")
endforeach()

add_library(native_test_x86_integer_constexpr_hub OBJECT hub.cc)
target_link_libraries(native_test_x86_integer_constexpr_hub PRIVATE native::native)
set_target_properties(native_test_x86_integer_constexpr_hub PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  return()
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_library(native_test_x86_integer_constexpr_reject_runtime OBJECT EXCLUDE_FROM_ALL reject_runtime.cc)
target_link_libraries(native_test_x86_integer_constexpr_reject_runtime PRIVATE native::native)
set_target_properties(native_test_x86_integer_constexpr_reject_runtime PROPERTIES
  CXX_SCAN_FOR_MODULES ON INTERPROCEDURAL_OPTIMIZATION FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  target_compile_options(native_test_x86_integer_constexpr_reject_runtime PRIVATE /clang:-ferror-limit=0)
else()
  target_compile_options(native_test_x86_integer_constexpr_reject_runtime PRIVATE -ferror-limit=0)
endif()
add_test(NAME native.x86.integer_constexpr.reject_runtime
  COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/check_rejections.py"
    --cmake "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --config "$<CONFIG>"
    --source "${CMAKE_CURRENT_SOURCE_DIR}/reject_runtime.cc"
    --log "${CMAKE_CURRENT_BINARY_DIR}/rejections-$<CONFIG>.log")
set_tests_properties(native.x86.integer_constexpr.reject_runtime PROPERTIES
  RUN_SERIAL TRUE LABELS "host;modules;constexpr;negative;x86" TIMEOUT 180)
