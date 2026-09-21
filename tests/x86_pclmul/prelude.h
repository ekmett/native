// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>
#include <immintrin.h>
#include <native/attributes.h>
#if defined(__PCLMUL__) || defined(__VPCLMULQDQ__) || defined(__AVX__) || defined(__AVX512F__)
#error Carry-less multiplication admission must run at the baseline
#endif
