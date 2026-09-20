// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#ifdef NATIVE_TARGETS_METADATA_ONLY
import native_target_metadata;
#else
import native;
#endif
#include <native/targets.h>
#define NATIVE_TARGET_invalid "avx2,not-a-feature"
constexpr auto reject=NATIVE_TARGET_ISA(invalid);
