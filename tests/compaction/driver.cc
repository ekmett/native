// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdio>
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
import native.x86.features;
#elif NATIVE_TEST_PROFILE == 128 && (NATIVE_TEST_FP16 || NATIVE_TEST_BF16)
import native.arm.features;
#endif
#if (!NATIVE_MINIMAL_HAS_AVX512 && (defined(__AVX512F__) || defined(__AVX512DQ__) || defined(__AVX512BW__) || defined(__AVX512VL__)))
#error Common consumer must not inherit AVX-512 ISA flags
#endif
int compaction_entry(int,char **);
int main(int argc,char ** argv) {
#if NATIVE_TEST_PROFILE == 256 || NATIVE_TEST_PROFILE == 512
#if NATIVE_TEST_AVX512_FP16
  constexpr auto profile = native::avx512_fp16;
#elif NATIVE_TEST_BF16
  constexpr auto profile = native::avx512_bf16;
#else
  constexpr auto profile = NATIVE_TEST_PROFILE == 512 ? native::avx512 : native::avx2;
#endif
  auto admission=native::classify_isa(native::observe_x86_capabilities(),profile);
  if(!admission.admitted()) { std::puts(admission.reason());return admission.invalid_features ? 1 : 77; }
#endif
#if NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_BF16
  auto admission=native::classify_isa(native::observe_arm_capabilities(),native::neon_bf16);
  if(!admission.admitted()) {std::puts(admission.reason());return 77;}
#elif NATIVE_TEST_PROFILE == 128 && NATIVE_TEST_FP16
  auto admission=native::classify_isa(native::observe_arm_capabilities(),native::neon_fp16);
  if(!admission.admitted()) { std::puts(admission.reason());return 77; }
#endif
  return compaction_entry(argc,argv);
}
