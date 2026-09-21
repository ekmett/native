// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native;
#include "checks.h"

using F = native::simd<float, 4, native::feature_closure(fp16fml_fixture::arch)>;
using H = native::simd<native::fp16, 8, native::feature_closure(fp16fml_fixture::arch)>;
static_assert(requires(F x, H y) { native::fmlal<native::feature_closure(fp16fml_fixture::arch)>(x, y, y); });
__attribute__((target("fp16fml"), noinline)) bool bridge() {
  F acc = float32x4_t{1, 2, 3, 4};
  H a = float16x8_t{1, 2, 3, 4, 5, 6, 7, 8};
  H b = float16x8_t{2, 3, 4, 5, 6, 7, 8, 9};
  F low = native::fmlal<native::feature_closure(fp16fml_fixture::arch)>(acc, a, b);
  F high = native::fmlal2_lane<native::feature_closure(fp16fml_fixture::arch), 7>(acc, a, b);
  auto l = low.to_native(), h = high.to_native();
  return l[0] == 3 && l[1] == 8 && l[2] == 15 && l[3] == 24 &&
    h[0] == 46 && h[1] == 56 && h[2] == 66 && h[3] == 76;
}
int main(int argc, char**) {
  int result = fp16fml_fixture::run(argc);
  return result ? result : bridge() ? 0 : 1;
}
