# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

# Preprocessing retains the real NATIVE_BASELINE expression and its assertions,
# while leaving `import native.isa;` for a normal-ABI compilation. No C++ standard
# library header is parsed with FP/vector registers disabled on Windows ARM64.
set(options --driver-mode=g++ -E -P -x c++ -std=c++26
  -DNATIVE_BASELINE_CASE=9 -march=armv8.3-a -mgeneral-regs-only "-I${INCLUDE}")
if(COMPILER_TARGET)
  list(APPEND options "--target=${COMPILER_TARGET}")
endif()
execute_process(COMMAND "${COMPILER}" ${options} "${SOURCE}"
  OUTPUT_FILE "${OUTPUT}" ERROR_VARIABLE diagnostic RESULT_VARIABLE result)
if(NOT result EQUAL 0)
  file(REMOVE "${OUTPUT}")
  message(FATAL_ERROR "General-register baseline preprocessing failed:\n${diagnostic}")
endif()
