// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>

#if defined(__BMI__) || defined(__BMI2__) || defined(__AVX__) || defined(__AVX2__) || defined(__FMA__)
#error BMI2 admission must run in a baseline translation unit
#endif
