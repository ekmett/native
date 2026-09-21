// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <native/targets.h>
import native.isa;

// An extra deployment requirement must not change the compiler's permissions.
#undef NATIVE_TARGET_EXTRA_MINIMUM
#define NATIVE_TARGET_EXTRA_MINIMUM (::native::isa(static_cast<::native::x86_feature>(-1)))

namespace {
  constexpr auto baseline=NATIVE_BASELINE;
  static_assert(baseline<=native::detail::known_features);
  static_assert(!(NATIVE_TARGET_MINIMUM<=native::detail::known_features));
  template<native::isa A> struct snapshot { static constexpr auto value=A; };
  static_assert(snapshot<baseline>::value==baseline);

#if NATIVE_BASELINE_CASE == 1
  // -msse4.2 -mno-popcnt: explicit negatives survive implied-feature bundles.
  static_assert(baseline.has(native::x86_feature::sse42));
  static_assert(baseline.has(native::x86_feature::crc32));
  static_assert(!baseline.has(native::x86_feature::popcnt));
  static_assert(!baseline.has(native::x86_feature::avx));
#elif NATIVE_BASELINE_CASE == 2
  // CRC32 has a compiler switch independent of the SSE4.2 vector instructions.
  static_assert(baseline.has(native::x86_feature::crc32));
  static_assert(!baseline.has(native::x86_feature::sse42));
  static_assert(!baseline.has(native::x86_feature::popcnt));
#elif NATIVE_BASELINE_CASE == 3
  // AVX2 enables predecessors but does not promise fused multiply-add.
  static_assert(baseline.has(native::x86_feature::avx2));
  static_assert(baseline.has(native::x86_feature::sse42));
  static_assert(baseline.has(native::x86_feature::popcnt));
  static_assert(!baseline.has(native::x86_feature::fma));
#elif NATIVE_BASELINE_CASE == 4
  // An unregistered enabled extension leaves registered features usable.
  static_assert(baseline.has(native::x86_feature::avx512f));
  static_assert(baseline.has(native::x86_feature::avx512bw));
#elif NATIVE_BASELINE_CASE == 5
  // An ARM scalar extension does not require additional SIMD instructions.
  static_assert(baseline.has(native::arm_feature::neon));
  static_assert(baseline.has(native::arm_feature::crc));
  static_assert(!baseline.has(native::arm_feature::aes));
  static_assert(!baseline.has(native::arm_feature::neon_fp16));
#elif NATIVE_BASELINE_CASE == 6
  // Clang's AES target macro establishes the independent AES and PMULL bits.
  static_assert(baseline.has(native::arm_feature::aes));
  static_assert(baseline.has(native::arm_feature::pmull));
  static_assert(!baseline.has(native::arm_feature::sha2));
#elif NATIVE_BASELINE_CASE == 7
  static_assert(baseline.has(native::arm_feature::sha1));
  static_assert(baseline.has(native::arm_feature::sha2));
  static_assert(baseline.has(native::arm_feature::sha3));
  static_assert(baseline.has(native::arm_feature::sha512));
#elif NATIVE_BASELINE_CASE == 8
  static_assert(!baseline.has(native::arm_feature::neon));
  static_assert(!baseline.has(native::arm_feature::neon_fp16));
#elif NATIVE_BASELINE_CASE == 9
  // Architecture-version macros can survive removal of FP/vector registers.
  static_assert(!baseline.has(native::arm_feature::neon));
  static_assert(!baseline.has(native::arm_feature::rdm));
  static_assert(!baseline.has(native::arm_feature::complxnum));
  static_assert(!baseline.has(native::arm_feature::jsconv));
#endif
}

#if NATIVE_BASELINE_CASE == 0
int main() { return 0; }
#endif
