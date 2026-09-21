// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <type_traits>
#include <wasm_simd128.h>
import native;
#include "constant.h"

namespace wasm_relaxed_test {
  using namespace native;
  template<class T> using vector_type = vector<T, strong>;
  unsigned failures = 0;
  std::uint64_t random_state = 0x9e3779b97f4a7c15;

  std::uint64_t random() {
    random_state ^= random_state >> 12;
    random_state ^= random_state << 25;
    random_state ^= random_state >> 27;
    return random_state * 0x2545f4914f6cdd1d;
  }

  void check(bool condition, char const * operation, unsigned iteration, unsigned lane) {
    if (!condition) {
      if (failures < 12) {
        std::printf("%s: case %u lane %u is outside the permitted result set\n", operation, iteration, lane);
      }
      ++failures;
    }
  }

  template<class T>
  using word = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;

  template<class T>
  bool same(T a, T b) {
    return std::bit_cast<word<T>>(a) == std::bit_cast<word<T>>(b);
  }

  template<class T, std::size_t N>
  bool arithmetic_nan(T actual, std::array<T, N> const & inputs) {
    constexpr unsigned fraction_width = sizeof(T) == 4 ? 23 : 52;
    constexpr word<T> quiet = word<T>{1} << (fraction_width - 1);
    constexpr word<T> fraction_mask = (word<T>{1} << fraction_width) - 1;
    bool canonical_inputs = true;
    for (auto input : inputs) {
      if (std::isnan(input) && (std::bit_cast<word<T>>(input) & fraction_mask) != quiet) {
        canonical_inputs = false;
      }
    }
    auto fraction = std::bit_cast<word<T>>(actual) & fraction_mask;
    return std::isnan(actual) && (canonical_inputs ? fraction == quiet : (fraction & quiet) != 0);
  }

  // Scalar libc arithmetic is independent of the library's integer-based
  // constexpr evaluator. Volatile temporaries and -ffp-contract=off retain the
  // separately rounded alternative; std::fma supplies the fused alternative.
  template<class T>
  bool multiply_add_allowed(T actual, T a, T b, T c) {
    volatile T product = a * b;
    T separate = product + c;
    auto fused = std::fma(a, b, c);
    bool nan_result = (std::isnan(separate) || std::isnan(fused)) &&
      arithmetic_nan(actual, std::array<T, 3>{a, b, c});
    return nan_result || same(actual, separate) || same(actual, fused);
  }

  template<class T>
  bool minmax_allowed(T actual, T a, T b, bool maximum) {
    if (std::isnan(a) || std::isnan(b)) {
      return arithmetic_nan(actual, std::array<T, 2>{a, b}) || same(actual, a) || same(actual, b);
    }
    if (a == 0 && b == 0) {
      return actual == 0;
    }
    return same(actual, maximum ? (a > b ? a : b) : (a < b ? a : b));
  }

  // Current core semantics leave invalid truncation results unspecified. Valid
  // truncated integers must agree exactly; f64 zero forms always clear lanes2/3.
  template<class I, class T>
  bool truncation_allowed(I actual, T input) {
    if (!std::isfinite(input)) {
      return true;
    }
    auto truncated = std::trunc(static_cast<double>(input));
    auto low = static_cast<double>(std::numeric_limits<I>::min());
    auto high = static_cast<double>(std::numeric_limits<I>::max());
    return truncated < low || truncated > high || actual == static_cast<I>(truncated);
  }

  __attribute__((target("relaxed-simd")))
  void swizzle_checks() {
    using vector_value = vector_type<std::uint8_t>;
    std::array<std::uint8_t, 16> input{};
    std::array<std::uint8_t, 16> indices{};
    for (unsigned i = 0; i < 16; ++i) {
      input[i] = static_cast<std::uint8_t>(i + 19);
    }
    for (unsigned offset = 0; offset < 256; offset += 16) {
      for (unsigned i = 0; i < 16; ++i) {
        indices[i] = static_cast<std::uint8_t>(offset + i);
      }
      auto result = lanes(i8x16_relaxed_swizzle(vector_value::load(input.data()), vector_value::load(indices.data())));
      for (unsigned i = 0; i < 16; ++i) {
        auto index = indices[i];
        bool allowed = index < 16 ? result[i] == input[index]
          : index >= 128 ? result[i] == 0 : result[i] == 0 || result[i] == input[index % 16];
        check(allowed, "swizzle", offset, i);
      }
    }
  }

  template<class U>
  __attribute__((target("relaxed-simd")))
  void selection_checks() {
    using vector_value = vector_type<U>;
    std::array<U, vector_value::lanes> a{}, b{}, mask{};
    for (unsigned trial = 0; trial < 256; ++trial) {
      for (unsigned i = 0; i < vector_value::lanes; ++i) {
        a[i] = static_cast<U>(random());
        b[i] = static_cast<U>(random());
        mask[i] = trial == 0 ? U(0) : trial == 1 ? U(~U(0)) : static_cast<U>(random());
      }
      auto av = vector_value::load(a.data());
      auto bv = vector_value::load(b.data());
      auto mv = vector_value::load(mask.data());
      vector_value output;
      if constexpr (sizeof(U) == 1) {
        output = i8x16_relaxed_laneselect(av, bv, mv);
      } else if constexpr (sizeof(U) == 2) {
        output = i16x8_relaxed_laneselect(av, bv, mv);
      } else if constexpr (sizeof(U) == 4) {
        output = i32x4_relaxed_laneselect(av, bv, mv);
      } else {
        output = i64x2_relaxed_laneselect(av, bv, mv);
      }
      auto result = lanes(output);
      for (unsigned i = 0; i < vector_value::lanes; ++i) {
        auto bits = U((a[i] & mask[i]) | (b[i] & ~mask[i]));
        auto lane = mask[i] >> (8 * sizeof(U) - 1) ? a[i] : b[i];
        check(result[i] == bits || result[i] == lane, "laneselect", trial, i);
      }
    }
  }

  template<class T>
  __attribute__((target("relaxed-simd")))
  void floating_checks() {
    using vector_value = vector_type<T>;
    constexpr auto infinity = std::numeric_limits<T>::infinity();
    constexpr auto largest = std::numeric_limits<T>::max();
    constexpr auto smallest = std::numeric_limits<T>::denorm_min();
    constexpr std::array<T, 20> edges{T(0), T(-0.0), T(1), T(-1), T(1.5), T(-1.5),
      infinity, -infinity, std::numeric_limits<T>::quiet_NaN(), std::numeric_limits<T>::signaling_NaN(),
      largest, -largest, smallest, -smallest, T(2147483648.0), T(-2147483648.0),
      T(4294967296.0), T(4294967295.0), T(-0.999), T(2147483647.0)};
    std::array<T, vector_value::lanes> a{}, b{}, c{};
    for (unsigned trial = 0; trial < 1024; ++trial) {
      for (unsigned i = 0; i < vector_value::lanes; ++i) {
        a[i] = trial < 400 ? edges[(trial + i) % edges.size()]
          : std::bit_cast<T>(static_cast<word<T>>(random()));
        b[i] = trial < 400 ? edges[(trial / edges.size() + i) % edges.size()]
          : std::bit_cast<T>(static_cast<word<T>>(random()));
        c[i] = trial < 400 ? edges[(trial * 7 + i) % edges.size()]
          : std::bit_cast<T>(static_cast<word<T>>(random()));
      }
      auto av = vector_value::load(a.data());
      auto bv = vector_value::load(b.data());
      auto cv = vector_value::load(c.data());
      vector_value madd, nmadd, minimum, maximum;
      vector_type<std::int32_t> signed_trunc;
      vector_type<std::uint32_t> unsigned_trunc;
      if constexpr (sizeof(T) == 4) {
        madd = f32x4_relaxed_madd(av, bv, cv);
        nmadd = f32x4_relaxed_nmadd(av, bv, cv);
        minimum = f32x4_relaxed_min(av, bv);
        maximum = f32x4_relaxed_max(av, bv);
        signed_trunc = i32x4_relaxed_trunc_f32x4(av);
        unsigned_trunc = u32x4_relaxed_trunc_f32x4(av);
      } else {
        madd = f64x2_relaxed_madd(av, bv, cv);
        nmadd = f64x2_relaxed_nmadd(av, bv, cv);
        minimum = f64x2_relaxed_min(av, bv);
        maximum = f64x2_relaxed_max(av, bv);
        signed_trunc = i32x4_relaxed_trunc_f64x2_zero(av);
        unsigned_trunc = u32x4_relaxed_trunc_f64x2_zero(av);
      }
      auto m = lanes(madd);
      auto n = lanes(nmadd);
      auto lo = lanes(minimum);
      auto hi = lanes(maximum);
      auto si = lanes(signed_trunc);
      auto ui = lanes(unsigned_trunc);
      for (unsigned i = 0; i < vector_value::lanes; ++i) {
        check(multiply_add_allowed(m[i], a[i], b[i], c[i]), "madd", trial, i);
        check(multiply_add_allowed(n[i], -a[i], b[i], c[i]), "nmadd", trial, i);
        check(minmax_allowed(lo[i], a[i], b[i], false), "min", trial, i);
        check(minmax_allowed(hi[i], a[i], b[i], true), "max", trial, i);
        check(truncation_allowed(si[i], a[i]), "trunc_s", trial, i);
        check(truncation_allowed(ui[i], a[i]), "trunc_u", trial, i);
      }
      if constexpr (sizeof(T) == 8) {
        check(si[2] == 0 && si[3] == 0 && ui[2] == 0 && ui[3] == 0, "trunc_zero", trial, 2);
      }
    }
  }

  __attribute__((target("relaxed-simd")))
  void q15_checks() {
    using vector_value = vector_type<std::int16_t>;
    std::array<std::int16_t, 8> a{}, b{};
    for (unsigned trial = 0; trial < 1024; ++trial) {
      for (unsigned i = 0; i < 8; ++i) {
        a[i] = trial == 0 ? INT16_MIN : static_cast<std::int16_t>(random());
        b[i] = trial == 0 ? INT16_MIN : static_cast<std::int16_t>(random());
      }
      auto output = lanes(i16x8_relaxed_q15mulr(vector_value::load(a.data()), vector_value::load(b.data())));
      for (unsigned i = 0; i < 8; ++i) {
        auto scaled = (static_cast<double>(a[i]) * b[i] + 16384.0) / 32768.0;
        auto expected = static_cast<std::int32_t>(std::floor(scaled));
        check(expected == 32768 ? output[i] == INT16_MIN || output[i] == INT16_MAX
          : output[i] == expected, "q15mulr", trial, i);
      }
    }
  }

  // Core numerics specifies saturation of each adjacent product pair, for both
  // signed and unsigned interpretations of b. Both dot forms share the choice.
  std::int16_t dot_pair(std::int8_t const * a, std::uint8_t const * b, bool unsigned_b) {
    auto first = static_cast<int>(a[0]) * (unsigned_b ? int(b[0]) : int(std::bit_cast<std::int8_t>(b[0])));
    auto second = static_cast<int>(a[1]) * (unsigned_b ? int(b[1]) : int(std::bit_cast<std::int8_t>(b[1])));
    auto sum = first + second;
    return static_cast<std::int16_t>(sum < -32768 ? -32768 : sum > 32767 ? 32767 : sum);
  }

  __attribute__((target("relaxed-simd")))
  void dot_checks() {
    using signed_bytes = vector_type<std::int8_t>;
    using unsigned_bytes = vector_type<std::uint8_t>;
    using accumulator = vector_type<std::int32_t>;
    std::array<std::int8_t, 16> a{};
    std::array<std::uint8_t, 16> b{};
    std::array<std::int32_t, 4> c{};
    for (unsigned trial = 0; trial < 1024; ++trial) {
      for (unsigned i = 0; i < 16; ++i) {
        a[i] = trial == 0 ? INT8_MIN : static_cast<std::int8_t>(random());
        b[i] = trial == 0 ? 128 : static_cast<std::uint8_t>(random() & (trial < 512 ? 127 : 255));
      }
      for (auto & value : c) {
        value = static_cast<std::int32_t>(random());
      }
      auto av = signed_bytes::load(a.data());
      auto bv = unsigned_bytes::load(b.data());
      auto output = lanes(i16x8_relaxed_dot_i8x16_i7x16(av, bv));
      auto added = lanes(i32x4_relaxed_dot_i8x16_i7x16_add(av, bv, accumulator::load(c.data())));
      std::array<std::int16_t, 8> raw_output{};
      std::array<std::int32_t, 4> raw_added{};
      wasm_v128_store(raw_output.data(), wasm_i16x8_relaxed_dot_i8x16_i7x16(av.to_native(), bv.to_native()));
      wasm_v128_store(raw_added.data(), wasm_i32x4_relaxed_dot_i8x16_i7x16_add(
        av.to_native(), bv.to_native(), accumulator::load(c.data()).to_native()));
      check(output == raw_output && added == raw_added, "raw/public dot agreement", trial, 0);
      // The separate raw-engine gate checks full-bit operand conformance and the
      // globally fixed interpretation, including the overflowing signed pair.
      if (trial != 0 && trial < 512) {
        std::array<std::int16_t, 8> pairs{};
        for (unsigned i = 0; i < 8; ++i) {
          pairs[i] = dot_pair(a.data() + 2 * i, b.data() + 2 * i, false);
          check(output[i] == pairs[i], "dot seven-bit operand", trial, i);
        }
        for (unsigned i = 0; i < 4; ++i) {
          auto sum = static_cast<std::uint32_t>(int(pairs[2 * i]) + int(pairs[2 * i + 1]));
          check(static_cast<std::uint32_t>(added[i]) == static_cast<std::uint32_t>(c[i]) + sum,
            "dot_add seven-bit operand", trial, i);
        }
      }
    }
  }
}

int main() {
  using namespace wasm_relaxed_test;
  swizzle_checks();
  selection_checks<std::uint8_t>();
  selection_checks<std::uint16_t>();
  selection_checks<std::uint32_t>();
  selection_checks<std::uint64_t>();
  floating_checks<float>();
  floating_checks<double>();
  q15_checks();
  dot_checks();
  if (failures) {
    std::printf("%u relaxed SIMD conformance failures\n", failures);
    return 1;
  }
  std::puts("All 20 relaxed SIMD wrappers pass; full-bit dot engine conformance is a separate gate");
}
