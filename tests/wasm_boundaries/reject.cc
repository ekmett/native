// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native;

constexpr auto simd_isa = native::feature_closure(native::wasm_feature::simd128);
#if NATIVE_WASM_CASE == 2 && !NATIVE_WASM_POSITIVE
constexpr native::isa<native::wasm> arch{};
#else
constexpr auto arch = simd_isa;
#endif
#if NATIVE_WASM_CASE == 3 && !NATIVE_WASM_POSITIVE
using vector = native::simd<std::uint32_t, 3, arch>;
#elif NATIVE_WASM_CASE == 6 && !NATIVE_WASM_POSITIVE
using vector = native::simd<std::uint32_t, 4>;
#else
using vector = native::simd<std::uint32_t, 4, arch>;
#endif

#if NATIVE_WASM_CASE == 2 || NATIVE_WASM_CASE == 3 || NATIVE_WASM_CASE == 6
static_assert(sizeof(vector) == 16);
#elif NATIVE_WASM_CASE == 1
#if NATIVE_WASM_POSITIVE
__attribute__((target("simd128")))
#endif
void caller(std::uint32_t *out, std::uint32_t const *left, std::uint32_t const *right) {
  (vector::load(left) + vector::load(right)).store(out);
}
#elif NATIVE_WASM_CASE == 4
__attribute__((target("simd128")))
std::uint32_t caller(vector value) {
  return value.get<NATIVE_WASM_POSITIVE ? 3 : 4>();
}
#elif NATIVE_WASM_CASE == 5
__attribute__((target("simd128")))
vector caller(vector value) {
  return shuffle<0, 1, 2, NATIVE_WASM_POSITIVE ? 7 : 8>(value, value);
}
#endif
