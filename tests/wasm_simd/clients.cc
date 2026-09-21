// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <cstdint>
#include <type_traits>
import native;
import native.math;

constexpr auto architecture = native::feature_closure(native::wasm_feature::simd128);
using vector = native::simd<float, 4, architecture>;
using registers = native::wide<vector, 2>;
template <class T>
concept has_exp = requires(T value) { native::math::exp(value); };
// The range-reduced exp kernel requires backend FMA/scaling operations. SIMD128
// does not expose it until a numerically qualified implementation is available.
static_assert(!has_exp<vector>);
static_assert(!has_exp<registers>);

__attribute__((target("simd128"))) constexpr bool arithmetic(float x) {
  registers a{vector(x), vector(x + 1.f)};
  registers b{vector(2.f), vector(3.f)};
  auto result = a * b + a - b;
  static_assert(std::same_as<decltype(result), registers>);
  std::array<float, 4> lane{};
  result.registers[0].store(lane.data());
  for (float value : lane)
    if (value != x * 2.f + x - 2.f)
      return false;
  result.registers[1].store(lane.data());
  for (float value : lane)
    if (value != (x + 1.f) * 3.f + (x + 1.f) - 3.f)
      return false;
  native::wide<vector, 0> empty{};
  auto sum = empty + empty;
  static_assert(std::same_as<decltype(sum), native::wide<vector, 0>>);
  return true;
}

static_assert(arithmetic(4.f));
volatile float input = 4.f;

__attribute__((target("simd128"), noinline)) bool runtime() {
  return arithmetic(input);
}

int main() {
  return runtime() ? 0 : 1;
}
