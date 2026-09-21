// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <utility>
#include <immintrin.h>
#include <native/attributes.h>

#if defined(__F16C__) || defined(__AVX__) || defined(__AVX2__) || defined(__AVX512FP16__)
#error F16C admission must run in a baseline translation unit
#endif
