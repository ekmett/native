// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include "prelude.h"
import native;
#include "checks.h"

native_target("avx2,fma,f16c") native_noinline bool bridges() {
  constexpr auto A = native::avx2 & native::x86_feature::f16c;
  using V = native::simd<float, 8, A>;
  V input(1.5f);
  auto half = native::cvtps_ph<A, 0>(input);
  auto output = native::cvtph_ps<A, 8>(half);
  std::array<float, 8> lanes{};
  output.store(lanes.data());
  for (auto x : lanes) if (x != 1.5f) return false;
  auto scalar = native::fp16::from_bits(native::cvtss_sh<A, 0>(1.5f));
  return scalar.to_bits() == 0x3e00 && native::cvtsh_ss<A>(scalar.to_bits()) == 1.5f;
}
int main() {
  int result = f16c_fixture::run();
  if (result) return result;
  auto cpu = native::observe_x86_capabilities();
  if (native::classify_isa(cpu, native::avx2 & native::x86_feature::f16c).admitted()) {
    f16c_fixture::mxcsr_guard guard;
    if (!bridges()) return 1;
    std::puts("fp16 storage and typed simd conversions passed");
  } else std::puts("SKIP: AVX2 simd bridge unavailable");
  return 0;
}
