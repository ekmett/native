# SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
foreach(operation IN ITEMS add sub mul fma neg)
  # Mach-O uses leading underscores; other supported object formats do not.
  execute_process(COMMAND "${OBJDUMP}" -d --no-show-raw-insn
    "--disassemble-symbols=fp16_${operation},_fp16_${operation}" "${OBJECT}"
    RESULT_VARIABLE status OUTPUT_VARIABLE assembly ERROR_VARIABLE errors)
  if(NOT status EQUAL 0)
    message(FATAL_ERROR "objdump failed: ${errors}")
  endif()
  if(operation STREQUAL "fma")
    set(instruction fmla)
  else()
    set(instruction f${operation})
  endif()
  string(REGEX MATCHALL "(${instruction}[ \t]+v[0-9]+[.]8h|${instruction}[.]8h[ \t]+v[0-9]+)" instructions "${assembly}")
  list(LENGTH instructions count)
  if(NOT count EQUAL 1 OR assembly MATCHES "[ \t](bl|blr|fcvt[a-z]*)([.][a-z0-9]+)?[ \t]")
    message(FATAL_ERROR "Expected one native ${instruction} .8h without conversion/call: ${assembly}")
  endif()
endforeach()
message(STATUS "Native half add/sub/mul/fma/neg without widening or calls")
