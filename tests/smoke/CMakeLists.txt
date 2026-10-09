# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# The documented examples already exercise values, masks, tails, swizzles,
# compaction, wide packs, math and utility imports with small known results.
set(API_PROFILES "${NATIVE_TEST_ISA}")
add_subdirectory("../api" "api")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|ARM64)$")
  set(instructions arm.cc)
else()
  set(instructions x86.cc)
endif()
add_executable(native_instruction_smoke main.cc ${instructions})
target_link_libraries(native_instruction_smoke PRIVATE native::native)
add_test(NAME native.instructions.smoke COMMAND native_instruction_smoke)
set_tests_properties(native.instructions.smoke PROPERTIES TIMEOUT 30 LABELS "smoke;instructions")

add_executable(native_half_smoke half.cc)
target_link_libraries(native_half_smoke PRIVATE native::native)
add_test(NAME native.half.smoke COMMAND native_half_smoke)
set_tests_properties(native.half.smoke PROPERTIES TIMEOUT 30 LABELS "smoke;numerics")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|aarch64|ARM64)$")
  add_executable(native_arm_sm3_semantics arm_sm3.cc)
  target_link_libraries(native_arm_sm3_semantics PRIVATE native::native)
  add_test(NAME native.arm.sm3.semantics COMMAND native_arm_sm3_semantics)
  set_tests_properties(native.arm.sm3.semantics PROPERTIES TIMEOUT 30 LABELS "smoke;arm;sm3")
endif()
