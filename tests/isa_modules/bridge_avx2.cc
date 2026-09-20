// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0

import fixture.engine;
import native.avx2;

extern "C" __declspec(dllexport) __declspec(noinline) int backend_avx2(
  float const * a, float const * b, float * output) {
  fixture::engine<native::avx2,8>::run(a, b, output);
  return native::avx2_lanes();
}
