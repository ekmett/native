// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#pragma once
// Included after importing native.wasm.relaxed or its architecture/main hub.

#include <array>
#include <bit>
#include <cstdint>
#include <limits>

namespace wasm_relaxed_test {
  constexpr auto strong = native::feature_closure(native::wasm_feature::relaxed_simd);
  constexpr auto weak = native::feature_closure(native::wasm_feature::simd128);

  template<class V>
  __attribute__((target("simd128"), always_inline))
  constexpr auto lanes(V value) noexcept {
    std::array<typename V::value_type, V::lanes> result{};
    value.store(result.data());
    return result;
  }

  template<class T, native::isa<native::wasm> A>
  using vector = native::simd<T, 16 / sizeof(T), A>;

  template<native::isa<native::wasm> A>
  consteval bool constant_checks() {
    using namespace native;
    using bytes = vector<std::uint8_t, A>;
    using signed_bytes = vector<std::int8_t, A>;
    using halves = vector<std::int16_t, A>;
    using words = vector<std::int32_t, A>;
    using floats = vector<float, A>;
    using doubles = vector<double, A>;
    std::array<std::uint8_t, 16> data{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    std::array<std::uint8_t, 16> indices{15, 14, 16, 127, 128, 255};
    auto swizzle = lanes(i8x16_relaxed_swizzle(bytes::load(data.data()), bytes::load(indices.data())));
    if (swizzle[0] != 16 || swizzle[1] != 15 || swizzle[2] || swizzle[3] || swizzle[4] || swizzle[5]) {
      return false;
    }
    auto nan32 = std::bit_cast<float>(std::uint32_t{0x7fc00000});
    auto nan64 = std::bit_cast<double>(std::uint64_t{0x7ff8000000000000});
    std::array<float, 4> trunc32{nan32, std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity(), -0.99f};
    auto input32 = floats::load(trunc32.data());
    if (lanes(i32x4_relaxed_trunc_f32x4(input32)) != std::array<std::int32_t, 4>{0, INT32_MAX, INT32_MIN, 0} ||
        lanes(u32x4_relaxed_trunc_f32x4(input32)) != std::array<std::uint32_t, 4>{0, UINT32_MAX, 0, 0}) {
      return false;
    }
    std::array<double, 2> trunc64{4294967295.75, -2147483648.75};
    auto input64 = doubles::load(trunc64.data());
    if (lanes(i32x4_relaxed_trunc_f64x2_zero(input64)) != std::array<std::int32_t, 4>{INT32_MAX, INT32_MIN, 0, 0} ||
        lanes(u32x4_relaxed_trunc_f64x2_zero(input64)) != std::array<std::uint32_t, 4>{UINT32_MAX, 0, 0, 0}) {
      return false;
    }
    auto x32 = floats(1.0f + 0x1p-23f);
    auto y32 = floats(1.0f - 0x1p-23f);
    auto x64 = doubles(1.0 + 0x1p-52);
    auto y64 = doubles(1.0 - 0x1p-52);
    if (lanes(f32x4_relaxed_madd(x32, y32, floats(-1)))[0] != -0x1p-46f ||
        lanes(f32x4_relaxed_nmadd(x32, y32, floats(1)))[0] != 0x1p-46f ||
        lanes(f64x2_relaxed_madd(x64, y64, doubles(-1)))[0] != -0x1p-104 ||
        lanes(f64x2_relaxed_nmadd(x64, y64, doubles(1)))[0] != 0x1p-104) {
      return false;
    }
    auto inf32 = floats(std::numeric_limits<float>::infinity());
    auto inf64 = doubles(std::numeric_limits<double>::infinity());
    if (std::bit_cast<std::uint32_t>(lanes(f32x4_relaxed_madd(inf32, floats(0), floats(1)))[0]) != 0x7fc00000 ||
        std::bit_cast<std::uint64_t>(lanes(f64x2_relaxed_nmadd(inf64, doubles(0), doubles(1)))[0]) != 0x7ff8000000000000 ||
        lanes(f32x4_relaxed_madd(floats(std::numeric_limits<float>::max()), floats(2),
          floats(-std::numeric_limits<float>::max())))[0] != std::numeric_limits<float>::max() ||
        lanes(f64x2_relaxed_madd(doubles(std::numeric_limits<double>::max()), doubles(2),
          doubles(-std::numeric_limits<double>::max())))[0] != std::numeric_limits<double>::max() ||
        lanes(f32x4_relaxed_min(floats(3), floats(1)))[0] != 1 ||
        lanes(f64x2_relaxed_max(doubles(3), doubles(1)))[0] != 3) {
      return false;
    }
    if (lanes(i8x16_relaxed_laneselect(bytes(0xaa), bytes(0x55), bytes(0x0f)))[0] != 0x5a ||
        lanes(i16x8_relaxed_laneselect(vector<std::uint16_t, A>(0xaaaa), vector<std::uint16_t, A>(0x5555), vector<std::uint16_t, A>(0x00ff)))[0] != 0x55aa ||
        lanes(i32x4_relaxed_laneselect(vector<std::uint32_t, A>(0xaaaaaaaa), vector<std::uint32_t, A>(0x55555555), vector<std::uint32_t, A>(0x0000ffff)))[0] != 0x5555aaaa ||
        lanes(i64x2_relaxed_laneselect(vector<std::uint64_t, A>(0xaaaaaaaaaaaaaaaa), vector<std::uint64_t, A>(0x5555555555555555), vector<std::uint64_t, A>(0x00000000ffffffff)))[0] != 0x55555555aaaaaaaa) {
      return false;
    }
    if (std::bit_cast<std::uint32_t>(lanes(f32x4_relaxed_min(floats(0.0f), floats(-0.0f)))[0]) != 0x80000000 ||
        std::bit_cast<std::uint32_t>(lanes(f32x4_relaxed_max(floats(-0.0f), floats(0.0f)))[0]) != 0 ||
        std::bit_cast<std::uint64_t>(lanes(f64x2_relaxed_min(doubles(0.0), doubles(-0.0)))[0]) != 0x8000000000000000 ||
        std::bit_cast<std::uint64_t>(lanes(f64x2_relaxed_max(doubles(-0.0), doubles(0.0)))[0]) != 0 ||
        std::bit_cast<std::uint32_t>(lanes(f32x4_relaxed_min(floats(nan32), floats(1)))[0]) != 0x7fc00000 ||
        std::bit_cast<std::uint64_t>(lanes(f64x2_relaxed_max(doubles(1), doubles(nan64)))[0]) != 0x7ff8000000000000) {
      return false;
    }
    if (lanes(i16x8_relaxed_q15mulr(halves(INT16_MIN), halves(INT16_MIN)))[0] != INT16_MAX ||
        lanes(i16x8_relaxed_q15mulr(halves(-16384), halves(16384)))[0] != -8192 ||
        lanes(i16x8_relaxed_dot_i8x16_i7x16(signed_bytes(-128), bytes(128)))[0] != 32767 ||
        lanes(i16x8_relaxed_dot_i8x16_i7x16(signed_bytes(-2), bytes(127)))[0] != -508 ||
        lanes(i32x4_relaxed_dot_i8x16_i7x16_add(signed_bytes(1), bytes(1), words(INT32_MAX)))[0] != INT32_MIN + 3) {
      return false;
    }
    return true;
  }

  static_assert(constant_checks<strong>());
  static_assert(constant_checks<weak>());
}
