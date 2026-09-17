#pragma once

#ifdef __cplusplus
// Host profiles own namespace selection and CPU feature admission.
#include "simd/simd/config.h"
#else
// Shader compilation has no host ISA profile or CPU feature requirements.
#ifndef SIMD_NAMESPACE
#define SIMD_NAMESPACE simd
#endif
#endif

// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
