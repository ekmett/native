// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

import fixture.engine;
import simd.avx2;

extern "C" __declspec(dllexport) __declspec(noinline) int backend_avx2(
  float const * a, float const * b, float * output) {
  fixture::engine<simd::avx2,8>::run(a, b, output);
  return simd::avx2_lanes();
}
