// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#if TEST_AVX2
import simd.avx2;
using Arch=simd::avx2;
#elif TEST_AVX512
import simd.avx512;
using Arch=simd::avx512;
#elif TEST_NEON_FP16
import simd.neon_fp16;
using Arch=simd::neon_fp16;
#else
import simd.neon;
using Arch=simd::neon;
#endif
extern "C" void granular_kernel(float const *input,float *output) {
  using V=simd::vec<float,4,Arch>;
  simd::store_simd(output,fma(simd::load_simd<V>(input),V(2.f),V(1.f)));
}
