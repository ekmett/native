// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native.wasm.relaxed;

constexpr auto vector_isa = native::feature_closure(native::wasm_feature::relaxed_simd);
using vector = native::simd<std::uint8_t, 16, vector_isa>;

__attribute__((target("relaxed-simd"), noinline))
void swizzle(std::uint8_t *out, std::uint8_t const *values, std::uint8_t const *indices) {
  native::i8x16_relaxed_swizzle(vector::load(values), vector::load(indices)).store(out);
}

int main(int argc, char **) {
  std::uint8_t values[16]{};
  std::uint8_t indices[16]{};
  std::uint8_t out[16]{};
  for (unsigned i = 0; i < 16; ++i) {
    values[i] = std::uint8_t(argc + i);
    indices[i] = std::uint8_t(15 - i);
  }
  swizzle(out, values, indices);
  for (unsigned i = 0; i < 16; ++i)
    if (out[i] != values[15 - i])
      return 1;
  return 0;
}
