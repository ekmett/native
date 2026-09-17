// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#if PROFILE_AVX2
import simd.avx2;
using V=simd::vec<float,8,simd::avx2>;
#define PAIR_ENTRY pair_avx2
#else
import simd.avx512;
using V=simd::vec<float,16,simd::avx512>;
#define PAIR_ENTRY pair_avx512
#endif
// Import-only generic wide and raw math consumer.
extern "C" int PAIR_ENTRY(float input) {
  auto [a,b]=simd::wide{V(input),V(input)};
  auto [raw]=simd::exp(simd::wide{a});
  auto empty=simd::exp(simd::wide<V,0>{});
  (void)b;(void)empty;
  float lane[V::lanes];raw.store(lane);
  for(float x:lane) if(x!=1.f)return 1;
  return 0;
}
