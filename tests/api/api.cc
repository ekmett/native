// SPDX-FileCopyrightText: 2026 Edward Kmett <ekmett@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <span>
#include <string_view>
#include <tuple>
#include <utility>
#if API_AVX512
import native;
constexpr auto selected_arch = native::avx512;
#elif API_NEON
import native;
constexpr auto selected_arch = native::neon;
#else
import native;
constexpr auto selected_arch = native::avx2;
#endif
import native.wide;
import native.math;
import native.memory;
import native.types;
import native.static_string;
import native.numerics;
#if API_NEON
static_assert((selected_arch & native::arm_feature::aes).has(selected_arch));
#else
static_assert((selected_arch & native::x86_feature::aes).has(selected_arch));
#endif
static_assert(native::x86_feature::avx2 < (native::x86_feature::avx2 & native::x86_feature::fma));
#if !API_NEON
import native.x86.features;
import native.x86.wait;
#endif

namespace {
void check(bool value) { if (!value) std::abort(); }

//! [vector_construction]
template<native::isa<> Arch>
void vector_construction() {
  using V = native::simd<float, 4, Arch>;
  V zero{};
  V repeated(2.f);
  auto lanes = V{1.f, 2.f, 3.f, 4.f};
  static_assert(std::same_as<decltype(lanes), V>);
  check(all(zero == V(0.f)));
  check(all(repeated == V(2.f)));
}
//! [vector_construction]

//! [masks]
template<native::isa<> Arch>
void masks() {
  using V = native::simd<float, 4, Arch>;
  V x{1.f, 2.f, 3.f, 4.f};
  typename V::mask active = x < V(3.f);
  auto chosen = select(active, x, V(-1.f));
  check(any(active) && !all(active));
  check(all(chosen == V{1.f, 2.f, -1.f, -1.f}));
  auto words = mask_bits<std::uint32_t>(active);
  std::array<std::uint32_t, 4> bits{};
  words.store(bits.data());
  check(bits[0] == 0xffffffffu && bits[3] == 0);
}
//! [masks]

//! [memory]
template<native::isa<> Arch>
void memory() {
  using V = native::simd<float, 4, Arch>;
  std::array<float, 4> input{1.f, 2.f, 3.f, 4.f};
  auto full = native::load_simd<V>(input);
  auto tail = native::load_simd_partial<V>(input.data(), 3, -1.f);
  std::array<float, 4> output{9.f, 9.f, 9.f, 9.f};
  native::store_simd_partial(output.data(), tail, 3);
  check(output == std::array<float, 4>{1.f, 2.f, 3.f, 9.f});
  native::store_simd(output.data(), full);
  check(output == input);
}
//! [memory]

//! [compaction]
template<native::isa<> Arch>
void compaction() {
  using V = native::simd<std::uint32_t, 4, Arch>;
  auto active = V::mask::from_bitset(0b1010);
  V input{10u, 20u, 30u, 40u};
  auto packed = native::compress(active, input, 99u);
  check(packed.count == 2 && all(packed.value == V{20u, 40u, 99u, 99u}));
  auto restored = native::expand(active, packed.value, V(77u));
  check(all(restored == V{77u, 20u, 77u, 40u}));
  std::array<std::uint32_t, 2> output{0u, 123u};
  auto written = native::compress_store(output.data(), 1, active, input);
  check(written == 1 && output[0] == 20u && output[1] == 123u);
}
//! [compaction]

//! [swizzles]
template<native::isa<> Arch>
void swizzles() {
  using V = native::simd<float, 3, Arch>;
  V position{1.f, 2.f, 3.f};
  auto saved = position.xy;             // an owning two-lane value
  position.xyz = position.zyx;          // snapshot, then scatter
  position.x = 4.f;
  check(all(saved == native::simd<float, 2, Arch>{1.f, 2.f}));
  check(all(position == V{4.f, 2.f, 1.f}));
}
//! [swizzles]

//! [arithmetic]
template<native::isa<> Arch>
void arithmetic() {
  using V = native::simd<float, 4, Arch>;
  auto y = fma(V(2.f), V(3.f), V(1.f));
  check(all(y == V(7.f)));
  check(all(sqrt(V(4.f)) == V(2.f)));
  check(all(native::abs(V(-2.f)) == V(2.f)));
  check(all(native::scaleb(V(1.f), V(3.5f)) == V(8.f)));
  auto integers = native::convert<std::int32_t>(V(3.75f));
  check(all(native::convert<float>(integers) == V(3.f)));
}
//! [arithmetic]

//! [rounding]
template<native::isa<> Arch>
void rounding() {
  using V = native::simd<float, 4, Arch>;
  V x{-1.75f, -0.25f, 0.25f, 1.75f};
  check(all(native::floor(x) == V{-2.f, -1.f, 0.f, 1.f}));
  check(all(native::ceil(x) == V{-1.f, -0.f, 1.f, 2.f}));
  check(all(native::trunc(x) == V{-1.f, -0.f, 0.f, 1.f}));
}
//! [rounding]

//! [exponential]
template<native::isa<> Arch>
void exponential() {
  using V = native::simd<float, 4, Arch>;
  std::array<V, 2> registers{V(0.f), V(1.f)};
  auto result = native::exp(registers);
  check(all(result[0] == V(1.f)));
  check(all(result[1] > V(2.718f)) && all(result[1] < V(2.719f)));
}
//! [exponential]

//! [bit_transport]
template<native::isa<> Arch>
void bit_transport() {
  using V = native::simd<float, 4, Arch>;
  auto tiny = V::from_bits(0x80000001u);
  auto zero = native::flush_to_zero(tiny);
  std::array<std::uint32_t, 4> words{};
  zero.store_bits(words.data());
  check(words[0] == 0x80000000u);         // signed zero, not a float conversion
}
//! [bit_transport]

//! [wide_values]
template<native::isa<> Arch>
void wide_values() {
  using V = native::simd<float, 4, Arch>;
  native::wide batch{V(1.f), V(2.f), V(3.f)};
  auto result = fma(batch, batch, batch);
  static_assert(std::tuple_size_v<decltype(result)> == 3);
  check(all(result.template get<2>() == V(12.f)));
  native::wide<float, 3> scalar_values{1.f, -2.f, 3.f};
  auto finite = native::isfinite(scalar_values);
  static_assert(std::same_as<decltype(finite), native::wide<bool, 3>>);
  check(finite.template get<0>() && finite.template get<1>());
  native::wide<float, 0> empty{};
  check(native::floor(empty).registers.empty());
}
//! [wide_values]

//! [common_utilities]
void common_utilities() {
  using namespace native;
  auto first = "vector"_ss;
  auto second = "vector"_ss;
  check(first == second && first.data() == second.data());
  check(std::hash<static_string>{}(first) == std::hash<static_string>{}(second));
  auto owned = native::dup(first.data());
  if (owned) check(std::string_view(owned.get()) == "vector");
  check(!native::type_of(first).empty());
  auto half = native::fp16(1.5f);
  check(static_cast<float>(half) == 1.5f);
}
//! [common_utilities]

#if !API_NEON
//! [capabilities]
void capabilities() {
  auto maximum_leaf = native::cpuid(0, 0).eax;
  check(maximum_leaf >= 0);
  // A real feature test must also admit the required OS vector state.
  // Waiting is independent of SIMD architecture selection.
  int ready = 1;
  native::wait_until<native::spin>(&ready, [](volatile int * p) { return *p != 0; });
}
//! [capabilities]
#endif
}
int main() {
  vector_construction<selected_arch>(); masks<selected_arch>(); memory<selected_arch>();
  compaction<selected_arch>();
  swizzles<selected_arch>(); arithmetic<selected_arch>(); rounding<selected_arch>();
  exponential<selected_arch>(); bit_transport<selected_arch>(); wide_values<selected_arch>();
  common_utilities();
#if !API_NEON
  capabilities();
#endif
}
