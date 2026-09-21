// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <native/attributes.h>

#if defined(__GFNI__) || defined(__AVX__) || defined(__AVX2__) || defined(__AVX512F__)
#error GFNI admission must run in a baseline translation unit
#endif
