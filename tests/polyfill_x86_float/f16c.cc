// SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0
#include <array>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <immintrin.h>
import native.x86.f16c;
#include "../x86_f16c/oracle.h"

constexpr auto scalar = native::isa<native::x86>(native::polyfill);
constexpr auto split = native::isa<native::x86>{native::x86_feature::sse2} | native::polyfill;
static_assert(native::cvtss_sh<scalar, 4>(std::bit_cast<float>(0x3f801000u)) == 0x3c00);
static_assert(native::cvtss_sh<2, split>(std::bit_cast<float>(1u)) == 1);
static_assert(std::bit_cast<std::uint32_t>(native::cvtsh_ss<scalar>(1)) == 0x33800000u);
template<unsigned I> concept valid_immediate = requires(float x) { native::cvtss_sh<scalar, I>(x); };
static_assert(valid_immediate<255> && !valid_immediate<256>);
template<class V> concept valid_narrow = requires(V x) { native::cvtps_ph<scalar, 0>(x); };
static_assert(!valid_narrow<native::simd<std::uint32_t, 4, scalar>>);
static_assert(!valid_narrow<native::simd<float, 5, scalar>>);
static_assert([] {
  constexpr std::array<float, 4> values{0.f, -0.f, 1.f, -2.f};
  auto narrowed = native::cvtps_ph<scalar, 0>(native::simd<float, 4, scalar>::load(values.data()));
  std::array<std::uint16_t, 4> bits{};
  narrowed.store_bits(bits.data());
  return bits == std::array<std::uint16_t, 4>{0, 0x8000, 0x3c00, 0xc000};
}());

struct mxcsr_guard {
  unsigned saved = _mm_getcsr();
  ~mxcsr_guard() { _mm_setcsr(saved); }
};
using singles = std::array<std::uint32_t, 8>;
using halves = std::array<std::uint16_t, 8>;

template<native::isa<native::x86> A, unsigned Imm8>
bool check_narrow(singles const & input, unsigned csr) {
  auto values = std::bit_cast<std::array<float, 8>>(input);
  halves expected{}, actual{}, four{};
  unsigned mode = (Imm8 & 4) ? (csr >> 13) & 3 : Imm8 & 3;
  for (unsigned i = 0; i < 8; ++i) {
    expected[i] = f16c_fixture::narrow(input[i], mode, (csr & 0x40) != 0);
    if (native::cvtss_sh<A, Imm8>(values[i]) != expected[i] ||
        native::cvtss_sh<Imm8, A>(values[i]) != expected[i]) return false;
  }
  native::cvtps_ph<A, Imm8>(native::simd<float, 8, A>::load(values.data())).store_bits(actual.data());
  for (unsigned i : {0u, 4u})
    native::cvtps_ph<A, Imm8>(native::simd<float, 4, A>::load(values.data() + i)).store_bits(four.data() + i);
  if (actual != expected || four != expected) {
    std::printf("F16C polyfill mismatch: immediate=%u csr=%x\n", Imm8, csr);
    return false;
  }
  // Fallback flags are unspecified; architectural controls must be preserved.
  return (_mm_getcsr() & ~0x3fu) == (csr & ~0x3fu);
}
template<native::isa<native::x86> A> bool modes(singles const & input, unsigned csr) {
  return check_narrow<A, 0>(input, csr) && check_narrow<A, 1>(input, csr) &&
    check_narrow<A, 2>(input, csr) && check_narrow<A, 3>(input, csr) &&
    check_narrow<A, 4>(input, csr) && check_narrow<A, 8>(input, csr) &&
    check_narrow<A, 255>(input, csr);
}
template<native::isa<native::x86> A> bool check_widen(halves const & input) {
  singles expected{}, actual{}, four{};
  std::array<float, 8> values{};
  for (unsigned i = 0; i < 8; ++i) {
    expected[i] = f16c_fixture::widen(input[i]);
    if (std::bit_cast<std::uint32_t>(native::cvtsh_ss<A>(input[i])) != expected[i]) return false;
  }
  native::cvtph_ps<A, 8>(native::simd<native::fp16, 8, A>::load_bits(input.data())).store(values.data());
  actual = std::bit_cast<singles>(values);
  for (unsigned i : {0u, 4u})
    native::cvtph_ps<A, 4>(native::simd<native::fp16, 4, A>::load_bits(input.data() + i)).store(values.data() + i);
  four = std::bit_cast<singles>(values);
  return actual == expected && four == expected;
}
int main() {
  mxcsr_guard guard;
  constexpr singles corners{0x80000000, 1, 0x80000001, 0x3f801000, 0xbf803000,
    0x7f800001, 0xffc12345, 0x47800000};
  constexpr halves half_corners{0, 0x8000, 1, 0x3ff, 0x7c01, 0xfc01, 0x7fff, 0xfc00};
  for (unsigned controls = 0; controls < 16; ++controls) {
    unsigned csr = 0x1f80u | ((controls & 3u) << 13) |
      ((controls & 4u) << 4) | ((controls & 8u) << 12);
    _mm_setcsr(csr);
    if (!modes<scalar>(corners, csr) || !modes<split>(corners, csr) ||
        !check_widen<scalar>(half_corners) || !check_widen<split>(half_corners)) return 1;
    std::uint32_t state = 0x19753921;
    for (unsigned trial = 0; trial < 96; ++trial) {
      singles input{};
      for (auto & bits : input) { state ^= state << 13; state ^= state >> 17; state ^= state << 5; bits = state; }
      if (!modes<scalar>(input, csr) || !modes<split>(input, csr)) return 1;
    }
    // Half boundary neighbors and both signs exercise directed under/overflow.
    for (unsigned h : {0u, 1u, 0x3ffu, 0x400u, 0x3bffu, 0x3c00u, 0x7bfdu, 0x7bffu}) {
      auto bits = f16c_fixture::widen(std::uint16_t(h));
      singles input{bits, bits ? bits - 1 : 0, bits + 1, bits + 0xfff,
        bits | 0x80000000u, (bits + 1) | 0x80000000u, 0x33000000, 0xb3000000};
      if (!modes<scalar>(input, csr) || !modes<split>(input, csr)) return 1;
    }
  }
  // Widening is independent of rounding and DAZ, including every NaN payload.
  _mm_setcsr(0xffc0);
  for (unsigned base = 0; base < 65536; base += 8) {
    halves input{};
    for (unsigned i = 0; i < 8; ++i) input[i] = std::uint16_t(base + i);
    if (!check_widen<scalar>(input) || !check_widen<split>(input)) return 1;
  }
  std::puts("F16C permission fallback values, rounding, DAZ, FTZ and scalar/split shapes passed.");
  return 0;
}
