# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
execute_process(COMMAND "${OBJDUMP}" -d --no-show-raw-insn --disassemble-symbols=bf16_dot2 "${OBJECT}"
  RESULT_VARIABLE status OUTPUT_VARIABLE assembly ERROR_VARIABLE errors)
if(NOT status EQUAL 0)
  message(FATAL_ERROR "objdump failed: ${errors}")
endif()
string(REGEX MATCHALL "vdpbf16ps" instructions "${assembly}")
list(LENGTH instructions count)
if(NOT count EQUAL 1)
  message(FATAL_ERROR "Expected one native BF16 dot instruction: ${assembly}")
endif()
if(assembly MATCHES "[ 	]call[q]?[ 	]")
  message(FATAL_ERROR "Unexpected out-of-line call: ${assembly}")
endif()
message(STATUS "bf16_dot2 uses one VDPBF16PS without calls")
