// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <concepts>
#include <cstdint>
#include <native/targets.h>
import native;

constexpr auto expected = [] {
  native::isa<native::wasm> result{};
  if constexpr (NATIVE_EXPECTED_WASM_BASELINE >= 1)
    result.set(native::wasm_feature::simd128, true);
  if constexpr (NATIVE_EXPECTED_WASM_BASELINE >= 2)
    result.set(native::wasm_feature::relaxed_simd, true);
  return result;
}();
static_assert(NATIVE_BASELINE == expected);
using vector = native::simd<float, 4>;
static_assert(std::same_as<vector, native::simd<float, 4, expected>>);
template<class T> concept complete = requires { sizeof(T); };
static_assert(complete<vector> == (NATIVE_EXPECTED_WASM_BASELINE != 0));

int main(int argc, char **) {
#if NATIVE_EXPECTED_WASM_BASELINE
  float input[]{float(argc), 2, 3, 4};
  float result[4]{};
  (vector::load(input) + vector(1)).store(result);
  for (unsigned i = 0; i < 4; ++i)
    if (result[i] != input[i] + 1)
      return 1;
#endif
  return 0;
}
