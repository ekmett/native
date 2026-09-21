// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.simd;
constexpr auto partial=native::feature_closure(native::avx2 & native::x86_feature::avx512f);
using V=native::simd<float,16,partial>;
static_assert(sizeof(V)==64);
void reject_storage_arithmetic(V value) {
#if NATIVE_ADAPTER_REJECT == 1
  (void)native::floor(value);
#elif NATIVE_ADAPTER_REJECT == 2
  (void)native::convert<std::int32_t>(value);
#elif NATIVE_ADAPTER_REJECT == 3
  (void)native::compress(V::mask::from_bits(3),value);
#endif
}
