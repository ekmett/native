# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

if(NOT NATIVE_BUILD_TESTS OR NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang"
    OR CMAKE_CROSSCOMPILING)
  message(FATAL_ERROR "Native coverage requires a native Clang test build.")
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
find_program(NATIVE_GRCOV grcov REQUIRED)
get_filename_component(native_llvm_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
find_program(NATIVE_LLVM_PROFDATA llvm-profdata HINTS "${native_llvm_bin}" REQUIRED)
find_program(NATIVE_LLVM_COV llvm-cov HINTS "${native_llvm_bin}" REQUIRED)
get_filename_component(native_coverage_llvm_bin "${NATIVE_LLVM_COV}" DIRECTORY)
set(native_coverage_dir "${CMAKE_CURRENT_BINARY_DIR}/coverage")
file(MAKE_DIRECTORY "${native_coverage_dir}/raw")
# Binary signature and process ID keep parallel tests from overwriting profiles.
# Directory options instrument providers and importers without changing the
# installed package's usage requirements.
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
  add_compile_options(
    "/clang:-fprofile-instr-generate=${native_coverage_dir}/raw/%m-%p.profraw"
    /clang:-fcoverage-mapping)
else()
  add_compile_options(
    "-fprofile-instr-generate=${native_coverage_dir}/raw/%m-%p.profraw" -fcoverage-mapping)
  add_link_options(-fprofile-instr-generate)
endif()

# Assembly probes follow the *_codegen.cc/codegen_*.cc convention. Disable
# counters in those consumers, reusing the instrumented module dependencies.
# Mixed runtime/assembly targets can explicitly set NATIVE_COVERAGE to OFF.
function(native_coverage_probes directory)
  get_property(targets DIRECTORY "${directory}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach(target IN LISTS targets)
    get_target_property(type "${target}" TYPE)
    if(type STREQUAL "EXECUTABLE")
      set_property(GLOBAL APPEND PROPERTY NATIVE_COVERAGE_BINARIES "$<TARGET_FILE:${target}>")
    endif()
    get_target_property(sources "${target}" SOURCES)
    get_property(explicit TARGET "${target}" PROPERTY NATIVE_COVERAGE SET)
    get_target_property(coverage "${target}" NATIVE_COVERAGE)
    if((explicit AND NOT coverage) OR sources MATCHES "(^|[/;])[^/;]*codegen[^/;]*\\.cc($|;)")
      if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        target_compile_options("${target}" PRIVATE /clang:-fno-profile-instr-generate /clang:-fno-coverage-mapping)
      else()
        target_compile_options("${target}" PRIVATE -fno-profile-instr-generate -fno-coverage-mapping)
      endif()
    endif()
  endforeach()
  get_property(children DIRECTORY "${directory}" PROPERTY SUBDIRECTORIES)
  foreach(child IN LISTS children)
    native_coverage_probes("${child}")
  endforeach()
endfunction()
function(native_coverage_configure)
  native_coverage_probes("${CMAKE_CURRENT_SOURCE_DIR}")
  get_property(binaries GLOBAL PROPERTY NATIVE_COVERAGE_BINARIES)
  file(GENERATE OUTPUT "${native_coverage_dir}/binaries-$<CONFIG>.txt"
    CONTENT "$<JOIN:${binaries},\n>\n")
endfunction()
cmake_language(DEFER CALL native_coverage_configure)

# Run after CTest: reporting never rebuilds or re-executes the test suite.
add_custom_target(native_coverage
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/etc/cmake/coverage_report.py"
    --source "${CMAKE_CURRENT_SOURCE_DIR}"
    --coverage "${native_coverage_dir}" --binaries "${native_coverage_dir}/binaries-$<CONFIG>.txt"
    --llvm "${native_coverage_llvm_bin}" --grcov "${NATIVE_GRCOV}"
  COMMENT "Generating Native runtime coverage with grcov"
  VERBATIM)
