// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native;

constexpr auto vector_isa = native::feature_closure(native::wasm_feature::simd128);
using vector = native::simd<std::uint32_t, 4, vector_isa>;

__attribute__((target("simd128"), noinline))
void add(std::uint32_t *out, std::uint32_t const *left, std::uint32_t const *right) {
  (vector::load(left) + vector::load(right)).store(out);
}

int main(int argc, char **) {
  auto x = std::uint32_t(argc);
  std::uint32_t left[]{x, x + 1, x + 2, x + 3};
  std::uint32_t right[]{3, 5, 7, 11};
  std::uint32_t out[4]{};
  add(out, left, right);
  for (unsigned i = 0; i < 4; ++i)
    if (out[i] != left[i] + right[i])
      return 1;
  return 0;
}
