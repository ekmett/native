// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
#include <native/attributes.h>

#if defined(__ADX__) || defined(__AVXIFMA__) || defined(__AVX512IFMA__) || defined(__AVX__)
#error IFMA/ADX admission must run at the provider baseline
#endif
