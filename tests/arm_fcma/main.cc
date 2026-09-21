// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native;
#include "checks.h"

using F = native::simd<float, 4, native::neon>;
static_assert(requires(F x) { native::fcmla<fcma_fixture::arch, 0>(x, x, x); });
using H = native::simd<native::fp16, 8, native::neon_fp16>;
static_assert(requires(H x) { native::fcmla<fcma_fixture::half_arch, 0>(x, x, x); });
__attribute__((target("complxnum"), noinline)) bool float_bridge() {
  F a = float32x4_t{1, 2, 3, 4};
  F b = float32x4_t{5, 6, 7, 8};
  F acc = float32x4_t{0, 0, 0, 0};
  F partial = native::fcmla<fcma_fixture::arch, 0>(acc, a, b);
  F complete = native::fcmla<fcma_fixture::arch, 90>(partial, a, b);
  return fcma_fixture::equal(complete.to_native(), float32x4_t{-7, 16, -11, 52});
}
__attribute__((target("complxnum,fullfp16"), noinline)) bool half_bridge() {
  H a = float16x8_t{1, 2, 3, 4, 5, 6, 7, 8};
  H b = float16x8_t{5, 6, 7, 8, 1, 2, 3, 4};
  H acc = float16x8_t{};
  H partial = native::fcmla<fcma_fixture::half_arch, 0>(acc, a, b);
  H complete = native::fcmla<fcma_fixture::half_arch, 90>(partial, a, b);
  return fcma_fixture::equal(complete.to_native(), float16x8_t{-7, 16, -11, 52, -7, 16, -11, 52});
}
int main(int argc, char**) {
  int result = fcma_fixture::run(argc);
  return result ? result : float_bridge() && half_bridge() ? 0 : 1;
}
