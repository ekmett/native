// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

#pragma once
#include <immintrin.h>
#include <array>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
#include "engine.h"
namespace native {
#define PROBE_LANES 8
#define PROBE_ARCH avx2
#define PROBE_ENTRY avx2_lanes
#include "backend.inc"
#undef PROBE_ENTRY
#undef PROBE_ARCH
#undef PROBE_LANES
}
