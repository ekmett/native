# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
execute_process(COMMAND "${OBJDUMP}" -d --no-show-raw-insn
  "--disassemble-symbols=bf16_dot2,_bf16_dot2" "${OBJECT}"
  RESULT_VARIABLE status OUTPUT_VARIABLE assembly ERROR_VARIABLE errors)
if(NOT status EQUAL 0)
  message(FATAL_ERROR "objdump failed: ${errors}")
endif()
string(REGEX MATCHALL "(bfdot[ \t]+v[0-9]+[.]4s|bfdot[.]4s[ \t]+v[0-9]+)" instructions "${assembly}")
list(LENGTH instructions count)
if(NOT count EQUAL 1 OR assembly MATCHES "[ \t](bl|blr|fcvt[a-z]*|bfcvt[a-z]*|fmul|fadd|fmla)([.][a-z0-9]+)?[ \t]")
  message(FATAL_ERROR "Expected one native BFDOT .4s without widening/scalarization/calls: ${assembly}")
endif()
message(STATUS "One native BFDOT .4s without conversion, arithmetic emulation or calls")
