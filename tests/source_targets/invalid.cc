// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifdef SIMD_TARGETS_METADATA_ONLY
import simd_target_metadata;
#else
import simd;
#endif
#include <simd/targets.h>
#define SIMD_TARGET_invalid "avx2,not-a-feature"
constexpr auto reject=SIMD_TARGET_ISA(invalid);
