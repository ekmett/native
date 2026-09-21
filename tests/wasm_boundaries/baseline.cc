// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <cstdint>
import native;
import native.wasm.relaxed;

constexpr auto vector_isa = native::feature_closure(native::wasm_feature::simd128);
using vector = native::simd<std::uint32_t, 4, vector_isa>;
static_assert(sizeof(vector) == 16);
static_assert(native::target_arch == native::wasm);

// Importing the hub and declaring vector types must not require their execution
// features. A real engine with both SIMD extensions disabled checks the result.
int main() {
  auto capabilities = native::observe_wasm_capabilities();
  return capabilities.observed == native::isa<native::wasm>{} ? 0 : 1;
}
