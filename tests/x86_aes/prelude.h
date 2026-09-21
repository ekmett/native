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
#if defined(__AES__) || defined(__VAES__) || defined(__AVX__)
#error AES admission must run at the provider baseline
#endif
