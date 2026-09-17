// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

import fixture.engine;
import simd.avx512;

extern "C" __declspec(dllexport) __declspec(noinline) int backend_avx512(
  float const * a, float const * b, float * output) {
  fixture::engine<simd::avx512,16>::run(a, b, output);
  return simd::avx512_lanes();
}
