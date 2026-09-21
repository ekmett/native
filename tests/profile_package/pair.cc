// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
import native.static_string;
import native.math;
using namespace native;
static_assert("abi"_ss.size() == 3);
#if PROFILE_AVX2
import native;
using V=native::simd<float,8,native::avx2>;
#define PAIR_ENTRY pair_avx2
#else
import native;
using V=native::simd<float,16,native::avx512>;
#define PAIR_ENTRY pair_avx512
#endif
// Import-only generic wide and raw math consumer.
extern "C" int PAIR_ENTRY(float input) {
  auto [a,b]=native::wide{V(input),V(input)};
  auto [raw]=native::exp(native::wide{a});
  auto empty=native::exp(native::wide<V,0>{});
  (void)b;(void)empty;
  float lane[V::lanes];raw.store(lane);
  for(float x:lane) if(x!=1.f)return 1;
  return 0;
}
