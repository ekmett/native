// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/isa.h>
#if NATIVE_INVALID_FAMILY == 0
constexpr auto mixed=native::wasm_feature::simd128&native::x86_feature::sse;
#else
constexpr auto mixed=native::wasm_feature::simd128&native::arm_feature::neon;
#endif
constexpr auto invalid=native::detail::source_isa<mixed>;
