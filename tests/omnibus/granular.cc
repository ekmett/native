// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#if TEST_AVX2
import native;
constexpr auto Arch = native::avx2;
#elif TEST_AVX512
import native;
constexpr auto Arch = native::avx512;
#elif TEST_NEON_BF16
import native;
constexpr auto Arch = native::neon_bf16;
#elif TEST_NEON_FP16
import native;
constexpr auto Arch = native::neon_fp16;
#else
import native;
constexpr auto Arch = native::neon;
#endif
extern "C" void granular_kernel(float const *input,float *output) {
  using V=native::vec<float,4,Arch>;
  native::store_simd(output,fma(native::load_simd<V>(input),V(2.f),V(1.f)));
}
