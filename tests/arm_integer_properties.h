// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included after the tested public instruction module.
#include "property_check.h"
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace arm_integer_properties {
  using input = std::array<std::array<std::uint64_t, 2>, 3>;
  using output = std::array<std::uint64_t, 2>;
  template<class T, unsigned N, native::isa<native::arm> A>
  constexpr auto vector(std::array<std::uint64_t, 2> bits) {
    using V = native::simd<T, N, A>;
    auto words = std::bit_cast<std::array<T, 16 / sizeof(T)>>(bits);
    std::array<T, sizeof(typename V::native_type) / sizeof(T)> storage{};
    for (unsigned i = 0; i < N; ++i) storage[i] = words[i];
    return V::from_native(std::bit_cast<typename V::native_type>(storage));
  }
  template<class T> constexpr T scalar(std::array<std::uint64_t, 2> bits) {
    return std::bit_cast<T>(static_cast<std::make_unsigned_t<T>>(bits[0]));
  }
  template<class V> constexpr output result(V value) {
    std::array<std::uint8_t, 16> bytes{};
    if constexpr (std::is_integral_v<V>) {
      auto data = std::bit_cast<std::array<std::uint8_t, sizeof(V)>>(value);
      for (unsigned i = 0; i < sizeof(V); ++i) bytes[i] = data[i];
    } else {
      auto data = std::bit_cast<std::array<std::uint8_t, sizeof(typename V::native_type)>>(value.to_native());
      for (unsigned i = 0; i < sizeof(typename V::value_type) * V::lanes; ++i) bytes[i] = data[i];
    }
    return std::bit_cast<output>(bytes);
  }
  // A wide accumulator and one final truncation provide an independent dot
  // oracle, including signed/unsigned byte products and modulo-2^32 overflow.
  template<class T, unsigned N, class L, class R, bool Matrix, int Lane>
  constexpr output dot_reference(input const &in) {
    auto a = std::bit_cast<std::array<T, 4>>(in[0]);
    auto b = std::bit_cast<std::array<L, 16>>(in[1]);
    auto c = std::bit_cast<std::array<R, 16>>(in[2]);
    std::array<std::uint32_t, 4> out{};
    for (unsigned row = 0; row < (Matrix ? 2 : N); ++row) {
      for (unsigned col = 0; col < (Matrix ? 2 : 1); ++col) {
        unsigned i = Matrix ? 2 * row + col : row;
        std::int64_t sum = a[i];
        for (unsigned k = 0; k < (Matrix ? 8 : 4); ++k) {
          unsigned left = Matrix ? row * 8 + k : i * 4 + k;
          unsigned right = Matrix ? col * 8 + k : Lane < 0 ? i * 4 + k : Lane * 4 + k;
          sum += std::int64_t{b[left]} * c[right];
        }
        out[i] = static_cast<std::uint32_t>(sum);
      }
    }
    return std::bit_cast<output>(out);
  }
  // Model the complete doubled product and accumulator in 128 bits, followed
  // by a single rounding shift and saturation. Production uses a distributed
  // 64-bit formulation, so this independently checks intermediate overflow.
  template<class T, unsigned N, bool Subtract, int Lane>
  constexpr output rdm_reference(input const &in) {
    constexpr unsigned width = sizeof(T) * 8;
    auto a = std::bit_cast<std::array<T, 16 / sizeof(T)>>(in[0]);
    auto b = std::bit_cast<std::array<T, 16 / sizeof(T)>>(in[1]);
    auto c = std::bit_cast<std::array<T, 16 / sizeof(T)>>(in[2]);
    std::array<T, 16 / sizeof(T)> out{};
    for (unsigned i = 0; i < N; ++i) {
      auto product = __int128{2} * b[i] * c[Lane < 0 ? i : Lane];
      auto wide = __int128{a[i]} * (__int128{1} << width) +
        (Subtract ? -product : product) + (__int128{1} << (width - 1));
      auto value = wide >> width;
      out[i] = value > std::numeric_limits<T>::max() ? std::numeric_limits<T>::max() :
        value < std::numeric_limits<T>::min() ? std::numeric_limits<T>::min() : static_cast<T>(value);
    }
    return std::bit_cast<output>(out);
  }
  template<class T> constexpr input operands(native_test::property_rng &rng, std::size_t index) {
    input in{};
    if constexpr (!std::is_void_v<T>) {
      constexpr T lo = std::numeric_limits<T>::min(), hi = std::numeric_limits<T>::max();
      constexpr T half = T{1} << (sizeof(T) * 8 - 2);
      constexpr T directed[][3] = {
        {lo,lo,lo}, {hi,lo,lo}, {-1,lo,lo}, {0,lo,lo},
        {lo,hi,hi}, {hi,hi,hi}, {0,1,half}, {0,-1,half},
        {7,1,half}, {-7,-1,half}, {0,hi,hi}, {lo,lo,hi},
        {hi,lo,hi}, {lo,0,hi}, {hi,0,lo}, {0,lo,hi}};
      if (index < std::size(directed)) {
        for (unsigned operand = 0; operand < 3; ++operand) {
          std::array<T, 16 / sizeof(T)> lanes{};
          lanes.fill(directed[index][operand]);
          in[operand] = std::bit_cast<output>(lanes);
        }
        return in;
      }
    }
    for (auto &v : in) for (auto &word : v) word = index == 0 ? 0 : index == 1 ? ~std::uint64_t{} : rng.next();
    return in;
  }
  struct sample { input operands; output expected; bool valid; };
  template<class Case> constexpr auto cases = native_test::property_cases<40>(native_test::property_seed,
    [index = 0u](native_test::property_rng &rng) mutable {
      auto in = operands<typename Case::edge_type>(rng, index++);
      auto strong = Case::template evaluate<Case::arch>(in);
      auto weak = Case::template evaluate<native::neon>(in);
      auto expected = Case::reference(in);
      return sample{in, expected, strong == expected && weak == expected};
    });
  template<class Case> bool check() {
    static_assert([] { for (auto const &v : cases<Case>) if (!v.valid) return false; return true; }());
    bool ok = true;
    for (unsigned i = 0; i < cases<Case>.size(); ++i) {
      auto const &v = cases<Case>[i];
      ok = native_test::property_equal(Case::name, native_test::property_seed, i,
        v.expected, Case::runtime(v.operands), v.operands) && ok;
    }
    auto config = native_test::property_config(128);
    native_test::property_rng rng{config.seed};
    for (std::size_t i = 0; i < config.cases; ++i) {
      auto in = operands<typename Case::edge_type>(rng, i);
      ok = native_test::property_equal(Case::name, config.seed, i,
        Case::reference(in), Case::runtime(in), in) && ok;
    }
    return ok;
  }
}
